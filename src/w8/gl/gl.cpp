//
// Created by Jiawei Tan on 2021/5/9.
//

#include "gl.h"

namespace w8 {
    namespace gl {
        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            Canvas::Initialize(isolate, global);
        }

        void Canvas::Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::FunctionTemplate> constructor_tpl = v8::FunctionTemplate::New(isolate, ConstructorCallback);
            constructor_tpl->SetClassName(v8::String::NewFromUtf8(isolate, Canvas::ClassName()).ToLocalChecked());
            // use this count latter
            constructor_tpl->InstanceTemplate()->SetInternalFieldCount(1);
            {
                v8::Local<v8::String> func_name = v8::String::NewFromUtf8(isolate, "getContext").ToLocalChecked();
                v8::Local<v8::Signature> signature = v8::Signature::New(isolate, constructor_tpl);
                v8::Local<v8::FunctionTemplate> proto_method_tpl = v8::FunctionTemplate::New(isolate,
                                                                                             GetContextCallback,
                                                                                             v8::Local<v8::Value>(),
                                                                                             signature);

                proto_method_tpl->SetClassName(func_name);
                constructor_tpl->PrototypeTemplate()->Set(func_name, proto_method_tpl);
            }

            {
                v8::Local<v8::String> func_name = v8::String::NewFromUtf8(isolate, "sayHi").ToLocalChecked();
                v8::Local<v8::Signature> signature = v8::Signature::New(isolate, constructor_tpl);
                v8::Local<v8::FunctionTemplate> proto_method_tpl = v8::FunctionTemplate::New(isolate,
                                                                                             SayHiCallback,
                                                                                             v8::Local<v8::Value>(),
                                                                                             signature);

                proto_method_tpl->SetClassName(func_name);
                constructor_tpl->PrototypeTemplate()->Set(func_name, proto_method_tpl);
            }

            {
                v8::Local<v8::String> func_name = v8::String::NewFromUtf8(isolate, "setHi").ToLocalChecked();
                v8::Local<v8::Signature> signature = v8::Signature::New(isolate, constructor_tpl);
                v8::Local<v8::FunctionTemplate> proto_method_tpl = v8::FunctionTemplate::New(isolate,
                                                                                             SetHiCallback,
                                                                                             v8::Local<v8::Value>(),
                                                                                             signature);

                proto_method_tpl->SetClassName(func_name);
                constructor_tpl->PrototypeTemplate()->Set(func_name, proto_method_tpl);
            }

            global->Set(v8::String::NewFromUtf8(isolate, "Canvas").ToLocalChecked(), constructor_tpl);
        }

        Canvas::Canvas(v8::Handle<v8::Object> instance) {
            assert(instance->InternalFieldCount() > 0);
            instance->SetAlignedPointerInInternalField(0, this);
            instance_.Reset(instance->GetIsolate(), instance);
            _hi_str = "canvas hi";
        }

        Canvas::~Canvas() {

        }

        void Canvas::ConstructorCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::HandleScope handle_scope(args.GetIsolate());
            if (args.IsConstructCall()) {
                // new and wrap
                new Canvas(args.This());
                args.GetReturnValue().Set(args.This());
            }
        }

        void Canvas::GetContextCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::HandleScope handle_scope(args.GetIsolate());
            void *ptr = args.Holder()->GetAlignedPointerFromInternalField(0);
            Canvas *canvas = static_cast<Canvas *>(ptr);

        }

        void Canvas::SayHiCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            void *ptr = args.Holder()->GetAlignedPointerFromInternalField(0);
            Canvas *canvas = static_cast<Canvas *>(ptr);
            args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, canvas->_hi_str.c_str()).ToLocalChecked());
        }

        void Canvas::SetHiCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            void *ptr = args.Holder()->GetAlignedPointerFromInternalField(0);
            Canvas *canvas = static_cast<Canvas *>(ptr);
            v8::String::Utf8Value utf8(isolate, args[0]);
            canvas->_hi_str = *utf8;
        }


    }
}
