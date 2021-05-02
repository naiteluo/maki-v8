//
// Created by Jiawei Tan on 2021/5/2.
//

#include "libplatform/libplatform.h"
#include <v8.h>
#include "V8Runner.h"

#include "v8_demos/demo_1/demo_1.h"
#include "v8_demos/demo_2/demo_2.h"

char **V8Runner::argv = NULL;
std::unique_ptr<v8::Platform> V8Runner::platform = NULL;

V8Runner::V8Runner() {}

V8Runner::~V8Runner() {
    _isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete _create_params.array_buffer_allocator;
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    if (args.Length() < 1)
        return;
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Value> arg = args[0];
    v8::String::Utf8Value value(isolate, arg);
    printf("C++ LOG: %s\n", *value);
}

void V8Runner::Initialize(char **_argv) {
    argv = _argv;
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(V8Runner::argv[0]);
    v8::V8::InitializeExternalStartupData(V8Runner::argv[0]);
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
}

void V8Runner::Run() {
    // Create a new Isolate and make it the current one.
    _create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    _isolate = v8::Isolate::New(_create_params);
    {
        v8::Isolate::Scope isolate_scope(_isolate);

        // **important** Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(_isolate);

//        // Create a template for the global object where we set the
//        // built-in global functions.
//        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(_isolate);
//        global->Set(_isolate, "log",
//                    v8::FunctionTemplate::New(_isolate, LogCallback));
//
//        // Create a new context and set global template.
//        v8::Local<v8::Context> context = v8::Context::New(_isolate, NULL, global);
//
//        // **important** Enter the context for compiling and running the hello world script.
//        v8::Context::Scope context_scope(context);


        run_demo1(_isolate, argv);

        run_demo2(_isolate, argv);
    }
}

void V8Runner::Dispose() {
    platform.release();
}
