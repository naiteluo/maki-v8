//
// Created by Jiawei Tan on 2021/5/2.
//

#include <v8.h>
#include <utils/FileReader.h>
#include "demo_1.h"


static void LogCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if (args.Length() < 1)
        return;
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Value> arg = args[0];
    v8::String::Utf8Value value(isolate, arg);
    printf("C++ LOG FUNC A: %s\n", *value);
}

void run_demo1(v8::Isolate *isolate, char **argv) {
    // Create a template for the global object where we set the
    // built-in global functions.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "log_a",
                v8::FunctionTemplate::New(isolate, LogCallback));

    // Create a new context and set global template.
    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

    // **important** Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
        v8::Local<v8::String> source;
        std::string filePath;
        if (argv[1] == NULL) {
            filePath = "index.js";
        } else {
            filePath = argv[1];
        }

        if (!FileReader::read(isolate, filePath).ToLocal(&source)) {
            fprintf(stderr, "Error reading '%s'.\n", filePath.c_str());
        } else {
            // Compile the source code.
            v8::Local<v8::Script> script =
                    v8::Script::Compile(context, source).ToLocalChecked();

            // Run the script to get the result.
            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

            // Convert the result to an UTF8 string and print it.
            v8::String::Utf8Value utf8(isolate, result);
            printf("%s\n", *utf8);
        }
    }
}