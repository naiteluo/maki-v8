// Include GLEW first
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <cstdio>
//#include <glm/glm.hpp>
#include <stdio.h>
#include "libplatform/libplatform.h"
#include <v8.h>

#include "w8.h"

#include <common_from_ogl/shader.hpp>
#include <common_from_ogl/text2D.hpp>
#include <utils/FileReader.h>
#include "v8-inspector.h"
#include "uv.h"
#include "timer.h"
#include "inspector.h"
#include "gfx/gfx.h"

namespace w8 {

    bool DO_DEBUG_LOGGING_LIFE_TIME = true;
    bool DO_DEBUG_LOGGING = true;

    std::unique_ptr<v8::Platform> App::platform = NULL;

    GLFWwindow *App::window = NULL;
    App *App::instance = NULL;
    v8::Persistent<v8::Context> App::globalContext;
    v8::Isolate *App::isolate = NULL;
    uv_loop_t *App::loop = uv_default_loop();
    uv_prepare_t *App::prepare_handle;

    Options App::options;

    int App::Initialize(int _argc, char **_argv) {
        options.Parse(_argc, _argv);
        options.Print();

        // Initialize V8.
        v8::V8::InitializeICUDefaultLocation(options.base_path.get());
        v8::V8::InitializeExternalStartupData(options.base_path.get());
        platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(platform.get());
        v8::V8::Initialize();

        return 0;
    }

    void App::Dispose() {
        v8::V8::Dispose();
        v8::V8::ShutdownPlatform();
    }

    App::App() {
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate = v8::Isolate::New(create_params);
    }

    App::~App() {
        window = NULL;
        isolate->Dispose();
        delete create_params.array_buffer_allocator;
    }

    v8::Local<v8::Context> App::CreateAppContext(v8::Isolate *isolate) {
        // Create a template for the global object where we set the
        // built-in global functions.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        v8::Local<v8::String> version_str = v8::String::NewFromUtf8(isolate, "version 1.0").ToLocalChecked();

        // try to add a nested object template
        v8::Local<v8::ObjectTemplate> inner = v8::ObjectTemplate::New(isolate);
        v8::Local<v8::String> inner_version = v8::String::NewFromUtf8(isolate, "version 3.0").ToLocalChecked();
        inner->Set(isolate, "version", inner_version);

        global->Set(isolate, "inner", inner);

        global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, App::JSFuncRequire));

        global->Set(isolate, "version", version_str);
        global->Set(isolate, "print",
                    v8::FunctionTemplate::New(isolate, App::JSFuncPrint));

        global->Set(isolate, "__w8__bootstrap", v8::FunctionTemplate::New(isolate, App::JSFuncBootstrap));
        global->Set(isolate, "__w8__sleep", v8::FunctionTemplate::New(isolate, App::JSFuncSleep));
        global->Set(isolate, "__w8__poll", v8::FunctionTemplate::New(isolate, App::JSFuncPoll));

        timer::Initialize(isolate, global);
        gfx::Initialize(isolate, global);

        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

        return context;
    }

    void App::JSFuncPrint(const v8::FunctionCallbackInfo<v8::Value> &args) {
        printf("JSFuncPrint ");
        if (args.Length() < 1)
            return;
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Value> arg = args[0];
        v8::String::Utf8Value value(isolate, arg);
        printf("Print: %s\n", *value);
    }

    const char *ToCString(const v8::String::Utf8Value &value) {
        return *value ? *value : "<string conversion failed>";
    }

    void PrintException(v8::Isolate *isolate, v8::TryCatch *try_catch) {
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception_utf8(isolate, try_catch->Exception());
        const char *exception_string = ToCString(exception_utf8);
        fprintf(stderr, "Exception: %s\n", exception_string);
        // todo print more message. see example shell.cc in v8
    }

    void PrintException(v8::Isolate *isolate, v8::Local<v8::Value> exception) {
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception_utf8(isolate, exception);
        const char *exception_string = ToCString(exception_utf8);
        fprintf(stderr, "Exception: %s\n", exception_string);
        // todo print more message. see example shell.cc in v8
    }

    v8::MaybeLocal<v8::Module> LoadJSModule(v8::Isolate *isolate, std::string filePath) {
        v8::EscapableHandleScope handle_scope(isolate);
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::String> source_str;
        if (!FileReader::read(isolate, filePath).ToLocal(&source_str)) {
            fprintf(stderr, "Error reading file: '%s'.\n", filePath.c_str());
            return handle_scope.Escape(v8::Local<v8::Module>());
        }
        v8::ScriptOrigin origin(
                v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked(),
                v8::Integer::New(isolate, 0),
                v8::Integer::New(isolate, 0),
                v8::True(isolate),
                v8::Local<v8::Integer>(),
                v8::Local<v8::Value>(),
                v8::False(isolate),
                v8::False(isolate),
                v8::True(isolate)
        );

        v8::Local<v8::Module> module;
        v8::ScriptCompiler::Source source(source_str, origin);
        if (!v8::ScriptCompiler::CompileModule(isolate, &source).ToLocal(&module)) {
            assert(try_catch.HasCaught());
            PrintException(isolate, &try_catch);
            return handle_scope.Escape(v8::Local<v8::Module>());
        }


        // AOT check modules alias
        for (int i = 0; i < (*module)->GetModuleRequestsLength(); i++) {
            v8::Local<String> specifier = (*module)->GetModuleRequest(i);
            v8::String::Utf8Value s(isolate, specifier);
        }

        module->InstantiateModule(isolate->GetCurrentContext(),
                                  [](v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
                                     v8::Local<v8::Module> referrer) {
                                      v8::Isolate *isolate_inner = context->GetIsolate();
                                      v8::String::Utf8Value specifier_utf8(isolate_inner, specifier);
                                      v8::Local<v8::Module> module;
                                      LoadJSModule(isolate_inner, ToCString(specifier_utf8)).ToLocal(&module);
                                      return v8::MaybeLocal<v8::Module>(module);
                                  }).Check();
        // todo support async callback, or dynamic module resolving.

        return handle_scope.Escape(module);
    }

    void LogExecution(const char *status, const char *file_path, const char *module_or_script) {
        if (!DO_DEBUG_LOGGING_LIFE_TIME) {
            return;
        }
        const char *tpl = "**************************************************** W8 Execution / %s / %s / %s \n";
        if (strcmp(status, "Fail") == 0 || strcmp(status, "LoadFail") == 0 || strcmp(status, "CompileFail") == 0 ||
            strcmp(status, "ModuleInitFail") == 0) {
            fprintf(stderr, tpl, module_or_script, status, file_path);
            return;
        }
        printf(tpl, module_or_script, status, file_path);
    }

    /**
     * modules are evaluated only once
     * re-evaluate evaluated module won't evaluate again
     * @param isolate
     * @param module
     * @param filePath
     * @return
     */
    v8::Local<v8::Value> ExecuteModule(v8::Isolate *isolate, v8::Local<v8::Module> module, std::string filePath) {
        // Setup scope and context bla bla
        v8::EscapableHandleScope handle_scope(isolate);
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        v8::Context::Scope context_scope(context);
        v8::Local<v8::Value> result;
        const char *file_path_c = filePath.c_str();
        const char *module_or_script = "module";
        LogExecution("Start", file_path_c, module_or_script);
        if (!module->Evaluate(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            PrintException(isolate, &try_catch);
            LogExecution("Fail", file_path_c, module_or_script);
            return handle_scope.Escape(v8::Local<v8::Value>());
        } else {
            assert(!try_catch.HasCaught());
            // get module evaluate exception;
            if (module->GetStatus() == v8::Module::Status::kErrored) {
                if (!module->GetException().IsEmpty()) {
                    PrintException(isolate, module->GetException());
                }
            }
            v8::String::Utf8Value utf8(isolate, result);
            if (DO_DEBUG_LOGGING) {
                printf("Execute result: %s\n", *utf8);
            }
            LogExecution("Success", file_path_c, module_or_script);
            return handle_scope.Escape(result);
        }
    }

    bool LoadAndExecuteFile(v8::Isolate *isolate, std::string filePath, bool isModule = true) {
        // Setup scope and context bla bla
        v8::HandleScope handle_scope(isolate);
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        // read from file
        v8::Local<v8::String> source_str;
        if (!FileReader::read(isolate, filePath).ToLocal(&source_str)) {
            fprintf(stderr, "** Error reading file: '%s'. **\n", filePath.c_str());
            return false;
        }

        const char *file_path_c = filePath.c_str();
        const char *module_or_script = isModule ? "Module" : "Script";

        // read success, try to compile and execute
        if (isModule) {
            // js module Instantiate
            v8::Local<v8::Module> module;
            if (!LoadJSModule(isolate, filePath).ToLocal(&module)) {
                LogExecution("ModuleInitFail", file_path_c, module_or_script);
                return false;
            }
            v8::Context::Scope context_scope(context);
            v8::Local<v8::Value> result;
            LogExecution("Start", file_path_c, module_or_script);
            if (!module->Evaluate(context).ToLocal(&result)) {
                assert(try_catch.HasCaught());
                PrintException(isolate, &try_catch);
                LogExecution("Fail", file_path_c, module_or_script);
                return false;
            } else {
                assert(!try_catch.HasCaught());
                // get module evaluate exception;
                if (module->GetStatus() == v8::Module::Status::kErrored) {
                    if (!module->GetException().IsEmpty()) {
                        PrintException(isolate, module->GetException());
                    }
                }
                v8::String::Utf8Value utf8(isolate, result);
                if (DO_DEBUG_LOGGING) {
                    printf("Execute result: %s\n", *utf8);
                }
                LogExecution("Success", file_path_c, module_or_script);
                return true;
            }
        } else {
            v8::Local<v8::Script> script;
            if (!v8::Script::Compile(context, source_str).ToLocal(&script)) {
                PrintException(isolate, &try_catch);
                LogExecution("CompileFail", file_path_c, module_or_script);
                return false;
            } else {
                v8::Local<v8::Value> result;
                LogExecution("Start", file_path_c, module_or_script);
                if (!script->Run(context).ToLocal(&result)) {
                    assert(try_catch.HasCaught());
                    PrintException(isolate, &try_catch);
                    LogExecution("Fail", file_path_c, module_or_script);
                    return false;
                } else {
                    assert(!try_catch.HasCaught());
                    v8::String::Utf8Value utf8(isolate, result);
                    if (DO_DEBUG_LOGGING) {
                        printf("Execute result: %s\n", *utf8);
                    }
                    LogExecution("Success", file_path_c, module_or_script);
                    return true;
                }
            }
        }
    }

    void App::Run() {
        // **important** Missing it will cause ptr issue when setting breaking.
        // Guessing breakpoint intercept need isolate scope to handle staffs or ranges？
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        {

            v8::Local<v8::Context> context = CreateAppContext(isolate);
            globalContext.Reset(isolate, context);

            // **important** Enter the context for compiling and running the hello world script.
            v8::Context::Scope context_scope(context);
            {
                inspector::InspectorClient inspector_client(context, true);
                // initialize inspector for current context
                if (options.inspector_enabled) {
                    uv_loop_set_data(loop, &inspector_client);
                    inspector_client.GetWSChannel().wait_for_connection();
                    context->SetAlignedPointerInEmbedderData(1, &inspector_client);
                    inspector_client.GetWSChannel().poll();
                    prepare_handle = (uv_prepare_t *) calloc(1, sizeof(uv_prepare_t));
                    uv_prepare_init(loop, prepare_handle);
                    prepare_handle->data = &inspector_client;
                    uv_prepare_start(prepare_handle, OnUVPrepareCallback);
                }

                std::string filePath(options.entry.get());
                v8::Local<v8::Module> module;
                if (!LoadJSModule(isolate, filePath).ToLocal(&module)) {
                    LogExecution("ModuleInitFail", filePath.c_str(), "module");
                }

                v8::Local<v8::Value> result;
                result = ExecuteModule(isolate, module, filePath);

                if (result.IsEmpty()) {
                    printf("ExecuteJS Fail.\n");
                }

                if (DO_DEBUG_LOGGING) {
                    printf("Event Looping Start:\n");
                }

                // todo: macro and micro task handling need reviewing
                while (v8::platform::PumpMessageLoop(platform.get(), isolate)) continue;
                uv_run(loop, UV_RUN_DEFAULT);
                while (isLoopContinue()) {
                    while (v8::platform::PumpMessageLoop(platform.get(), isolate)) continue;
                    uv_run(loop, UV_RUN_DEFAULT);
                }
                // todo: uv_default_loop is not thread safe. close this loop may crash. init loop manually.
                uv_loop_close(loop);
            }
        }
    }

    void App::Stop() {

    }

    void App::JSFuncBootstrap(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();

        LoadAndExecuteFile(isolate, "app.mjs");
    }

    void App::JSFuncSleep(const v8::FunctionCallbackInfo<v8::Value> &args) {
        usleep(10000000);
    }

    void App::JSFuncPoll(const v8::FunctionCallbackInfo<v8::Value> &args) {
        inspector::InspectorClient *client = (inspector::InspectorClient *) loop->data;
        (*client).GetWSChannel().poll();
    }

    void App::OnUVPrepareCallback(uv_prepare_t *handle) {
        inspector::InspectorClient *client = (inspector::InspectorClient *) handle->data;
        (*client).GetWSChannel().poll();
        if (!isLoopContinue()) {
            // remove prepare handle
            uv_close((uv_handle_t *) prepare_handle, [](uv_handle_t *handle) {
                free(handle);
            });
        }
    }

    bool App::isLoopContinue() {
        bool isLoopAlive = uv_loop_alive(loop);
        bool hasUnhandledTimer = timer::Timer::timer_pool.size() > 0;
        return (isLoopAlive && hasUnhandledTimer);
    }

    void App::JSFuncRequire(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handle_scope(isolate);
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Value> path = args[0];
        v8::String::Utf8Value path_str(isolate, path);
        v8::Local<v8::Context> context = globalContext.Get(isolate);

        const char *file_path_c = ToCString(path_str);
        // read from file
        v8::Local<v8::String> source_str;
        if (!FileReader::read(isolate, file_path_c).ToLocal(&source_str)) {
            fprintf(stderr, "** Error reading file: '%s'. **\n", file_path_c);
            return;
        }

        const char *module_or_script = "Script";

        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(context, source_str).ToLocal(&script)) {
            PrintException(isolate, &try_catch);
            LogExecution("CompileFail", file_path_c, module_or_script);
        } else {
            v8::Local<v8::Value> result;
            LogExecution("Start", file_path_c, module_or_script);
            if (!script->Run(context).ToLocal(&result)) {
                assert(try_catch.HasCaught());
                PrintException(isolate, &try_catch);
                LogExecution("Fail", file_path_c, module_or_script);
            } else {
                assert(!try_catch.HasCaught());
                v8::String::Utf8Value utf8(isolate, result);
                if (DO_DEBUG_LOGGING) {
                    printf("Execute result: %s\n", *utf8);
                }
                LogExecution("Success", file_path_c, module_or_script);
                args.GetReturnValue().Set(result);
            }
        }
        return;
    }

}