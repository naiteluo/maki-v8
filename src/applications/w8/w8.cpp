// Include GLEW first
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <cstdio>
#include <glm/glm.hpp>
#include <stdio.h>
#include "libplatform/libplatform.h"
#include <v8.h>

#include "w8.h"

#include <common_from_ogl/shader.hpp>
#include <common_from_ogl/text2D.hpp>
#include <utils/FileReader.h>
#include "uv.h"
#include "Timer.h"

namespace w8 {

    std::unique_ptr<v8::Platform> App::platform = NULL;

    GLFWwindow *App::window = NULL;
    App *App::instance = NULL;
    v8::Isolate *App::isolate = NULL;
    uv_loop_t *App::loop = uv_default_loop();

    double App::lastTime;
    int App::nbFrames;

    char **App::argv;

    int App::Initialize(char **_argv) {
        argv = _argv;
        // Initialise GLFW
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize GLFW\n");
            getchar();
            return -1;
        }
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                       GL_TRUE); // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Initialize V8.
        v8::V8::InitializeICUDefaultLocation(argv[0]);
        v8::V8::InitializeExternalStartupData(argv[0]);
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
        inner->Set(isolate, "log", v8::FunctionTemplate::New(isolate, App::JSFuncLog));
        inner->Set(isolate, "version", inner_version);

        global->Set(isolate, "inner", inner);

        global->Set(isolate, "version", version_str);
        global->Set(isolate, "print",
                    v8::FunctionTemplate::New(isolate, JSFuncPrint));
        global->Set(isolate, "glClear",
                    v8::FunctionTemplate::New(isolate, App::JSFuncGLClear));
        global->Set(isolate, "glText",
                    v8::FunctionTemplate::New(isolate, App::JSFuncGLText));
        global->Set(isolate, "glfwTick",
                    v8::FunctionTemplate::New(isolate, App::JSFuncGLFWTick));

        timer::Initialize(isolate, global);

        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);


        // switch to override default console
        bool override_console = false;

        if (override_console) {
            // console is a build in object in global,
            // which can will be delegated in v8 to places like inspector-related-module.
            // you can override console in this way:
            // add object instance named console to global
            v8::Local<v8::ObjectTemplate> consoleObj = v8::ObjectTemplate::New(isolate,
                                                                               v8::Local<v8::FunctionTemplate>());
            consoleObj->Set(isolate, "log", v8::FunctionTemplate::New(isolate, JSFuncLog));
            context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "console").ToLocalChecked(),
                                   consoleObj->NewInstance(context).ToLocalChecked()).FromJust();
        }

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
        printf("LOG: %s\n", *value);
    }

    void App::JSFuncLog(const v8::FunctionCallbackInfo<v8::Value> &args) {
        printf("JSFuncLog ");
        if (args.Length() < 1)
            return;
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Value> arg = args[0];
        v8::String::Utf8Value value(isolate, arg);
        printf("LOG: %s\n", *value);
    }

    int App::OpenWindow() {
        // Open a window and create its OpenGL context
        window = glfwCreateWindow(500, 500, "V8", NULL, NULL);
        if (window == NULL) {
            fprintf(stderr,
                    "Failed to open GLFW window. If you have an Intel GPU, they are "
                    "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
            getchar();
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        // Ensure we can capture the escape key being pressed below
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

        // note: make context before init glew
        // Initialize GLEW
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
        }
        return 0;
    }

    const char *ToCString(const v8::String::Utf8Value &value) {
        return *value ? *value : "<string conversion failed>";
    }

    void PrintException(v8::Isolate *isolate, v8::TryCatch *try_catch) {
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception(isolate, try_catch->Exception());
        const char *exception_string = ToCString(exception);
        fprintf(stderr, "Exception: %s\n", exception_string);
        // todo print more message. see example shell.cc in v8
    }

    bool ExecuteJS(v8::Isolate *isolate, std::string filePath) {
        v8::HandleScope handle_scope(isolate);
        v8::TryCatch try_catch(isolate);
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        v8::Local<v8::String> source;
        if (!FileReader::read(isolate, filePath).ToLocal(&source)) {
            fprintf(stderr, "Error reading file: '%s'.\n", filePath.c_str());
            return false;
        } else {
            printf("=========== ExecuteJS '$c' Start: =========\n", filePath.c_str());
            v8::Local<v8::Script> script;
            if (!v8::Script::Compile(context, source).ToLocal(&script)) {
                PrintException(isolate, &try_catch);
                return false;
            } else {
                v8::Local<v8::Value> result;
                if (!script->Run(context).ToLocal(&result)) {
                    assert(try_catch.HasCaught());
                    PrintException(isolate, &try_catch);
                    printf("========*** ExecuteJS '$c' Fail. ***========\n", filePath.c_str());
                    return false;
                } else {
                    assert(!try_catch.HasCaught());
                    v8::String::Utf8Value utf8(isolate, result);
                    printf("Execute result: %s\n", *utf8);
                    printf("=========== ExecuteJS '$c' End. ===========\n", filePath.c_str());
                    return true;
                }
            }
        }

    }

    void App::Run() {

        // Dark blue background
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // Initialize our little text library with the Holstein font
        initText2D("Holstein.DDS");

        // For speed computation
        lastTime = glfwGetTime();
        nbFrames = 0;

        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = CreateAppContext(isolate);

        // **important** Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);
        {
            std::string filePath;
            if (argv[1] == NULL) {
                filePath = "draw.js";
            } else {
                filePath = argv[1];
            }
            bool success = ExecuteJS(isolate, filePath);
            while (v8::platform::PumpMessageLoop(platform.get(), isolate)) continue;
            if (!success) {
                printf("ExecuteJS Fail.\n");
            }
        }
    }

    void App::Stop() {
        glDeleteVertexArrays(1, &VertexArrayID);
        cleanupText2D();
        // Close OpenGL window and terminate GLFW
        glfwTerminate();
    }


    void App::JSFuncGLText(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();

        v8::Local<v8::Value> arg = args[0];
        v8::String::Utf8Value str(isolate, arg);

        v8::Local<v8::Value> arg1 = args[1];
        int x = arg1->Int32Value(context).ToChecked();
        v8::Local<v8::Value> arg2 = args[2];
        int y = arg2->Int32Value(context).ToChecked();
        v8::Local<v8::Value> arg3 = args[3];
        int size = arg3->Int32Value(context).ToChecked();

        printText2D(*str, x, y, size);
    }


    void App::JSFuncGLClear(const v8::FunctionCallbackInfo<v8::Value> &args) {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void App::JSFuncGLFWTick(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        int f;
        if (!(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
              glfwWindowShouldClose(window) == 0)) {
            f = 0;
        } else {
            f = 1;
        }
        args.GetReturnValue().Set(v8::Integer::New(isolate, f));
    }

}