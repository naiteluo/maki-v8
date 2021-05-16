//
// Created by Jiawei Tan on 2021/5/9.
//

#include "gl.h"
#include "../v8-binding.h"

namespace w8 {
    namespace gfx {

        void WebGL2RenderingContext::Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::FunctionTemplate> constructor_tpl_local = v8::FunctionTemplate::New(isolate);
            constructor_tpl_local->SetClassName(v8::String::NewFromUtf8(isolate, ClassName()).ToLocalChecked());
            constructor_tpl_local->InstanceTemplate()->SetInternalFieldCount(1);
            // add proto method

            ADD_PROTO_METHOD(isolate, constructor_tpl_local, "clear", ClearCallback);
            ADD_PROTO_METHOD(isolate, constructor_tpl_local, "clearColor", ClearColorCallback);

            // add constant

            ADD_CONSTANT(isolate, constructor_tpl_local, "COLOR_BUFFER_BIT", v8::Integer::New(isolate, 0x00004000));

            constructor_tpl.Reset(isolate, constructor_tpl_local);

            global->Set(v8::String::NewFromUtf8(isolate, ClassName()).ToLocalChecked(), constructor_tpl_local);
        }

        WebGL2RenderingContext::WebGL2RenderingContext() : V8Object<WebGL2RenderingContext>() {
            printf("1231");
        }

        void WebGL2RenderingContext::ClearCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::ClearColorCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::ConstructorCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::HandleScope handle_scope(args.GetIsolate());
            if (args.IsConstructCall()) {
                // create instance and wrap to `this`
                new WebGL2RenderingContext();
                args.GetReturnValue().Set(args.This());
            }
        }

        WebGL2RenderingContext::~WebGL2RenderingContext() {
            printf("WebGL2RenderingContext do Destroy");
        }

        void WebGL2RenderingContext::Destroy(const v8::WeakCallbackInfo<V8Object<WebGL2RenderingContext>> &args) {
            printf("WebGL2RenderingContext do Destroy");
        }

    }
}
