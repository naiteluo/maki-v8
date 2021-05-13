//
// Created by Jiawei Tan on 2021/5/7.
//

#ifndef MAKI_W8_INSPECTOR_H
#define MAKI_W8_INSPECTOR_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8-inspector.h"
#include <string>
#include <websocketpp/common/connection_hdl.hpp>
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"
#include <chrono>

namespace w8 {
    /**
     * inspector capability
     *
     * docs: https://v8.dev/docs/inspector
     *
     * todo: channel ()
     * todo: debugger support
     */

    namespace inspector {

        class InspectorChannel;

        class InspectorClient;

        typedef websocketpp::server<websocketpp::config::asio> DebugServer;
        typedef std::map<websocketpp::connection_hdl, InspectorChannel, std::owner_less<websocketpp::connection_hdl>> WebSocketConnections;

        class WebSocketService {
        public:

            WebSocketService(InspectorClient *client, short port);

            ~WebSocketService();


            void send(websocketpp::connection_hdl &connection, const v8_inspector::StringView &message);

            void
            wait_for_connection(std::chrono::duration<float> sleep_between_polls = std::chrono::duration<float>(0.1f));

            void poll();

            void poll_one();

            void poll_until_idle(float idle_time = 1.0f);

            void run_one();

            void send_message(websocketpp::connection_hdl connection, void *data, size_t length);

            float seconds_since_message_received();

            float seconds_since_message_sent();

            float seconds_since_message();


        private:

            bool ws_validation_handler(websocketpp::connection_hdl hdl);

            // wspp callback

            void on_open(websocketpp::connection_hdl hdl);

            void on_close(websocketpp::connection_hdl hdl);

            void on_message(websocketpp::connection_hdl hdl, DebugServer::message_ptr msg);

            void on_http(websocketpp::connection_hdl hdl);

            std::chrono::time_point<std::chrono::high_resolution_clock> message_received_time = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> message_sent_time = std::chrono::high_resolution_clock::now();

            unsigned short port_ = 0;
            inspector::InspectorClient *inspector_client_;
            DebugServer debug_server_;
            WebSocketConnections connections_;

        };

        enum {
            kModuleEmbedderDataIndex, kInspectorClientIndex
        };

        std::string StringViewToUtf8Value(v8::Isolate *isolate, v8_inspector::StringView string_view);

        class InspectorChannel final : public v8_inspector::V8Inspector::Channel {
        public:
            InspectorChannel(WebSocketService &service, websocketpp::connection_hdl hdl) : web_socket_service(service),
                                                                                           connection(hdl) {
            }

            ~InspectorChannel() override = default;

        private:

            void sendResponse(
                    int callId,
                    std::unique_ptr<v8_inspector::StringBuffer> message) override {
                web_socket_service.send(connection, message->string());
            }

            void sendNotification(
                    std::unique_ptr<v8_inspector::StringBuffer> message) override {
                web_socket_service.send(connection, message->string());
            }

            void flushProtocolNotifications() override {}

            WebSocketService &web_socket_service;
            websocketpp::connection_hdl connection;
        };

        class InspectorClient : public v8_inspector::V8InspectorClient {

        private:

            v8::Local<v8::Context>  ensureDefaultContextInGroup(int contextGroupId) override {
                return context_.Get(isolate_);
            }

            WebSocketService websocket_service_;
            short port_ = 9227;
            static const int kContextGroupId = 1;
            std::unique_ptr<v8_inspector::V8Inspector> inspector_;
            std::unique_ptr<v8_inspector::V8InspectorSession> session_;
            std::unique_ptr<v8_inspector::V8Inspector::Channel> channel_;

            bool is_paused = false;
            v8::Global<v8::Context> context_;
            v8::Isolate *isolate_;

        public:
            InspectorClient(v8::Local<v8::Context> context, bool connect) : websocket_service_(this, 9229) {
                if (!connect) return;
                isolate_ = context->GetIsolate();
                ResetSession();
                context->SetAlignedPointerInEmbedderData(kModuleEmbedderDataIndex, this);
                inspector_ = v8_inspector::V8Inspector::create(isolate_, this);
                session_ = inspector_->connect(kContextGroupId, channel_.get(), v8_inspector::StringView());
                // configure inspector context
                inspector_->contextCreated(v8_inspector::V8ContextInfo(
                        context, kContextGroupId, v8_inspector::StringView()
                ));

                // expose extras functions to context for inspector
                context_.Reset(isolate_, context);
            }

            bool paused = false;

            v8::Isolate *getIsolate() {
                return isolate_;
            }

            v8::Global<v8::Context> const &GetGlobalContext() {
                return context_;
            }

            void ConnectWithChannel(v8_inspector::V8Inspector::Channel *channel) {
                session_ = inspector_->connect(1, channel, v8_inspector::StringView());
            }

            void ResetSession() {
                session_.reset();
            }

            inline v8_inspector::V8InspectorSession &GetSession() {
                return *this->session_;
            }

            void runMessageLoopOnPause(int contextGroupId) override {
                printf("runMessageLoopOnPause\n");
                paused = true;
                while(paused) {
                    this->websocket_service_.run_one();
                }
            }

            void quitMessageLoopOnPause() override {
                printf("quitMessageLoopOnPause\n");
                paused = true;
            }

            WebSocketService & GetWSChannel() {
                return websocket_service_;
            }

            virtual void consoleAPIMessage(int contextGroupId, v8::Isolate::MessageErrorLevel level,
                                           const v8_inspector::StringView &message, const v8_inspector::StringView &url,
                                           unsigned int lineNumber, unsigned int columnNumber,
                                           v8_inspector::V8StackTrace *) override {
                int length = static_cast<int>(message.length());
                // todo: length check
                // DCHECK_LT(length, v8::String::kMaxLength);
                // parse StringView to string
                std::string message_str = StringViewToUtf8Value(isolate_, message);
                std::string url_str = StringViewToUtf8Value(isolate_, url);
                // better formatting for url

                char *tpl = "Log %*s > %s\n";
                url_str = url_str.substr(url_str.find_last_of("/\\") + 1) + ":" + std::to_string(lineNumber) + ":" +
                          std::to_string(columnNumber);
                if (level == v8::Isolate::MessageErrorLevel::kMessageError ||
                    level == v8::Isolate::MessageErrorLevel::kMessageWarning) {
                    fprintf(stderr, tpl,
                            20, url_str.c_str(), message_str.c_str()
                    );
                } else {
                    fprintf(stdout, tpl,
                            20, url_str.c_str(), message_str.c_str()
                    );
                }
            }
        };
    }
}


#endif //MAKI_W8_INSPECTOR_H
