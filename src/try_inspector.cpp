//
// Created by Jiawei Tan on 2021/5/13.
//

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8-inspector.h"
#include <string>
#include <websocketpp/common/connection_hdl.hpp>
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"
#include <chrono>
#include "try_inspector/try_inspector.h"

std::string StringViewToUtf8Value(v8::Isolate *isolate, v8_inspector::StringView string_view) {
    int length = static_cast<int>(string_view.length());
    v8::Local<v8::String> string_view_str = string_view.is8Bit() ?
                                            v8::String::NewFromOneByte(isolate,
                                                                       reinterpret_cast<const uint8_t *>(string_view.characters8()),
                                                                       v8::NewStringType::kNormal,
                                                                       length).ToLocalChecked()
                                                                 : v8::String::NewFromTwoByte(isolate,
                                                                                              reinterpret_cast<const uint16_t *>(string_view.characters16()),
                                                                                              v8::NewStringType::kNormal,
                                                                                              length).ToLocalChecked();
    v8::String::Utf8Value utf8(isolate, string_view_str);

    std::string str = *utf8;
    return str;
}

WebSocketService::WebSocketService(InspectorClient *client, short port) : port_(port) {
    inspector_client_ = client;
    debug_server_.get_alog().clear_channels(websocketpp::log::alevel::all);
    debug_server_.get_elog().clear_channels(websocketpp::log::alevel::all);

    debug_server_.set_validate_handler(
            bind(&WebSocketService::ws_validation_handler, this, websocketpp::lib::placeholders::_1));
    debug_server_.set_open_handler(bind(&WebSocketService::on_open, this, websocketpp::lib::placeholders::_1));
    debug_server_.set_close_handler(
            bind(&WebSocketService::on_close, this, websocketpp::lib::placeholders::_1));
    debug_server_.set_message_handler(
            bind(&WebSocketService::on_message, this, websocketpp::lib::placeholders::_1,
                 websocketpp::lib::placeholders::_2));
    debug_server_.set_http_handler(
            websocketpp::lib::bind(&WebSocketService::on_http, this, websocketpp::lib::placeholders::_1));
    debug_server_.init_asio();
    debug_server_.set_reuse_addr(true);

    debug_server_.listen(this->port_);
    debug_server_.start_accept();
}

WebSocketService::~WebSocketService() {}

bool WebSocketService::ws_validation_handler(websocketpp::connection_hdl hdl) {
    return connections_.size() == 0;
}

void WebSocketService::on_open(websocketpp::connection_hdl hdl) {
    printf("WebSocketService::on_open");
    assert(connections_.size() == 0);
    connections_.emplace(hdl, InspectorChannel(*this, hdl));
    inspector_client_->ConnectWithChannel(&connections_.find(hdl)->second);
    message_received_time = std::chrono::high_resolution_clock::now();
}

void WebSocketService::on_close(websocketpp::connection_hdl hdl) {
    assert(connections_.size() == 1);
    connections_.erase(hdl);
    assert(connections_.size() == 0);

    inspector_client_->ResetSession();
    inspector_client_->paused = false;
    message_received_time = std::chrono::high_resolution_clock::now();
}

void WebSocketService::on_message(websocketpp::connection_hdl hdl, DebugServer::message_ptr msg) {
    std::string message_payload = msg->get_payload();

    v8_inspector::StringView message_view((uint8_t const *) message_payload.c_str(), message_payload.length());

    // todo： create macros to do such thing
    v8::Isolate *isolate = inspector_client_->getIsolate();
//            v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = inspector_client_->GetGlobalContext().Get(isolate);
    v8::Context::Scope context_scope(context);

    inspector_client_->GetSession().dispatchProtocolMessage(message_view);
    message_received_time = std::chrono::high_resolution_clock::now();
}

void WebSocketService::on_http(websocketpp::connection_hdl hdl) {
    DebugServer::connection_ptr conn = debug_server_.get_con_from_hdl(hdl);
    std::stringstream output;
    output << "<!doctype html><html><body>You requested "
           << conn->get_resource()
           << "</body></html>";
    conn->set_status(websocketpp::http::status_code::ok);
    conn->set_body(output.str());
}

void WebSocketService::run_one() {
    debug_server_.run_one();
}

void WebSocketService::poll() {
    debug_server_.poll();
}

void WebSocketService::poll_one() {
    debug_server_.poll_one();
}

void WebSocketService::wait_for_connection(std::chrono::duration<float> sleep_between_polls) {
    while (connections_.empty()) {
        poll_one();
        if (connections_.empty()) {
            std::this_thread::sleep_for(sleep_between_polls);
        }
    }
}

void WebSocketService::send(websocketpp::connection_hdl &connection, const v8_inspector::StringView &message) {
    if (message.is8Bit()) {
        this->send_message(connection, (void *) message.characters8(), message.length());
    } else {
        size_t length = message.length();
        uint16_t const *source = message.characters16();
        auto buffer = std::make_unique<char[]>(length);
        for (int i = 0; i < length; i++) {
            buffer[i] = source[i];
        }
        send_message(connection, static_cast<void *>(buffer.get()), length);
    }
}

void WebSocketService::send_message(websocketpp::connection_hdl connection, void *data, size_t length) {
    if (!connections_.empty()) {
        debug_server_.send(connection, data, length, websocketpp::frame::opcode::TEXT);
    }
    message_sent_time = std::chrono::high_resolution_clock::now();
}

v8::MaybeLocal<v8::String> read_file(v8::Isolate *isolate, const std::string &name) {
    FILE *file = fopen(name.c_str(), "rb");
    if (file == NULL)
        return v8::MaybeLocal<v8::String>();
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    std::unique_ptr<char> chars(new char[size + 1]);
    chars.get()[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars.get()[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return v8::MaybeLocal<v8::String>();
        }
    }

    fclose(file);

    v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(
            isolate, chars.get(), v8::NewStringType::kNormal, static_cast<int>(size));

    return result;
}

int main(int argc, char *argv[]) {

    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        global->Set(isolate, "__w8__poll",
                    v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value> &args) {
                        v8::Local<v8::Context> context = args.GetIsolate()->GetCurrentContext();
                        InspectorClient *client = static_cast<InspectorClient *>(context->GetAlignedPointerFromEmbedderData(
                                1));
                        (client)->GetWSChannel().poll();
                    }));
        global->Set(isolate, "__w8__sleep",
                    v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value> &args) {
                        usleep(10000000);
                    }));


        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);

        InspectorClient inspector_client(context, true);
        inspector_client.GetWSChannel().wait_for_connection();
        context->SetAlignedPointerInEmbedderData(1, &inspector_client);

        inspector_client.GetWSChannel().poll();

        {

            // pure script without origin

//            const char csource[] = R"(
//function doNothing(a, b) {
//    return a + b;
//}
//// runTaskTest();
//while(true) {
//    console.log("aaa");
//    __sleep();
//    // __w8__sleep();
//    // __w8__sleep();
//    let a = 1 + 2;
//    // __w8__sleep();
//    let c = 2 + 3;
//    for (let i = 1; i < 100; i++) {
//        doNothing(a, c);
//    }
//    console.log(11233);
//    __poll();
//}
//      )";
//
//            // Create a string containing the JavaScript source code.
//            v8::Local<v8::String> source =
//                    v8::String::NewFromUtf8Literal(isolate, csource);

//            // Compile the source code.
//            v8::Local<v8::Script> script =
//                    v8::Script::Compile(context, source).ToLocalChecked();

            // load file as script

//            std::string filePath = "test.js";
//            v8::ScriptOrigin origin(
//                    isolate, v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked()
//            );
//            v8::Local<v8::String> source = read_file(isolate, filePath).ToLocalChecked();
//
//            // Compile the source code.
//            v8::Local<v8::Script> script =
//                    v8::Script::Compile(context, source, &origin).ToLocalChecked();

            // load file as module

            std::string filePath = "test.js";
            v8::ScriptOrigin origin(
                    isolate, v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked(),
                    0, 0,
                    false, -1,
                    v8::Local<v8::Value>(),
                    false, false, true
//                    v8::True(isolate), v8::Local<v8::Integer>(),
//                    v8::Local<v8::Value>(), v8::False(isolate),
//                    v8::False(isolate), v8::True(isolate)
            );
            v8::Local<v8::String> source_str = read_file(isolate, filePath).ToLocalChecked();
            v8::ScriptCompiler::Source source(source_str, origin);

            v8::Local<v8::Module> module = v8::ScriptCompiler::CompileModule(isolate, &source).ToLocalChecked();
            module->InstantiateModule(isolate->GetCurrentContext(),
                                      [](v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
                                         v8::Local<v8::Module> referrer) {
                                          return v8::MaybeLocal<v8::Module>();
                                      });

            inspector_client.GetWSChannel().poll();

            v8::Local<v8::Value> result = module->Evaluate(context).ToLocalChecked();


            // Run the script to get the result.
//            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();



            // Convert the result to an UTF8 string and print it.
            v8::String::Utf8Value utf8(isolate, result);
            printf("%s\n", *utf8);
        }
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}