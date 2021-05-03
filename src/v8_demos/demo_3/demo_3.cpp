//
// Created by Jiawei Tan on 2021/5/2.
//

#include <v8.h>
#include <common_from_ogl/text2D.hpp>
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

    printText2D(*str, x, y, size);
}

void run_demo3(v8::Isolate *isolate, char **argv) {
    // Create a template for the global object where we set the
    // built-in global functions.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "log_c",
                v8::FunctionTemplate::New(isolate, LogCallback));
    global->Set(isolate, "drawText",
                v8::FunctionTemplate::New(isolate, DrawTextCallback));

    // Create a new context and set global template.
    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

    // **important** Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
        const char csource[] = R"(
        let i = 50;
        while (i-- > 0) {
            drawText(
                'Text From JS',
                Math.floor(500 * Math.random()),
                Math.floor(500 * Math.random()),
                Math.floor(100 * Math.random())
            );
        }
      )";

        // Create a string containing the JavaScript source code.
        v8::Local<v8::String> source =
                v8::String::NewFromUtf8Literal(isolate, csource);

        // Compile the source code.
        v8::Local<v8::Script> script =
                v8::Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    }
}