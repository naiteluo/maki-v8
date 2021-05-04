//
// Created by Jiawei Tan on 2021/5/2.
//

#include <v8.h>
#include <common_from_ogl/text2D.hpp>
#include <uv.h>
#include <queue>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "demo_3.h"

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if (args.Length() < 1)
        return;
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Value> arg = args[0];
    v8::String::Utf8Value value(isolate, arg);
    printf("C++ LOG FUNC C: %s\n", *value);
}

static void DrawTextCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
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

    glClear(GL_COLOR_BUFFER_BIT);
    printText2D(*str, x, y, size);
    glfwPollEvents();
}


uv_loop_t *loop;

int count = 0;

v8::Global<v8::Function> cb;

std::queue<v8::Global<v8::Function>> set_timeout_callbacks_;
std::queue<v8::Global<v8::Context>> set_timeout_contexts_;

class Looper {
public:
    static v8::Isolate *isolate_;

    static void Loop(const v8::FunctionCallbackInfo<v8::Value> &args);

    static void Callback(uv_timer_t *handle);

    static uv_timer_t _timer_req;
};

v8::Isolate *Looper::isolate_;

uv_timer_t Looper::_timer_req;

void Looper::Loop(const v8::FunctionCallbackInfo<v8::Value> &args) {
    v8::Isolate *isolate = args.GetIsolate();
    if (args.Length() != 1 || !args[0]->IsFunction()) {
        fprintf(stderr, "Args[0] is not function\n");
        return;
    }
    v8::Local<v8::Function> callback = args[0].As<v8::Function>();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    set_timeout_callbacks_.emplace(isolate, callback);
    set_timeout_contexts_.emplace(isolate, context);
    uv_timer_init(loop, &Looper::_timer_req);
    uv_timer_start(&Looper::_timer_req, Looper::Callback, 0, 1000);
    uv_run(loop, UV_RUN_DEFAULT);
}

void Looper::Callback(uv_timer_t *handle) {
    count++;
    fprintf(stderr, "Looping: %i\n", count);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Function> callback = set_timeout_callbacks_.front().Get(isolate_);
    v8::Local<v8::Context> context = set_timeout_contexts_.front().Get(isolate_);
    v8::Context::Scope context_scope(context);
    callback->Call(context, Undefined(isolate_), 1, {});
};


void run_demo3(v8::Isolate *isolate, char **argv) {
    loop = uv_default_loop();

    Looper::isolate_ = isolate;

    // Create a template for the global object where we set the
    // built-in global functions.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "log_c",
                v8::FunctionTemplate::New(isolate, LogCallback));
    global->Set(isolate, "drawText",
                v8::FunctionTemplate::New(isolate, DrawTextCallback));
    global->Set(isolate, "loop",
                v8::FunctionTemplate::New(isolate, Looper::Loop));

    // Create a new context and set global template.
    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

    // **important** Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
        const char csource[] = R"(
        let i = 50;
        function run () {
             drawText(
                'Text From JS',
                Math.floor(500 * Math.random()),
                Math.floor(500 * Math.random()),
                Math.floor(100 * Math.random())
            );
//            loop(run);
        }
        loop(run);
//        while (i-- > 0) {
//            drawText(
//                'Text From JS',
//                Math.floor(500 * Math.random()),
//                Math.floor(500 * Math.random()),
//                Math.floor(100 * Math.random())
//            );
//        }
      )";

        // Create a string containing the JavaScript source code.
        v8::Local<v8::String> source =
                v8::String::NewFromUtf8Literal(isolate, csource);

        // Compile the source code.
        v8::Local<v8::Script> script =
                v8::Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        printf("END");
    }

}