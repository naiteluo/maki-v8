//
// Created by Jiawei Tan on 2021/5/16.
//

#include "canvas.h"

namespace w8 {
    namespace gfx {
        void Canvas::Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::FunctionTemplate> constructor_tpl = v8::FunctionTemplate::New(isolate, ConstructorCallback);
            constructor_tpl->SetClassName(v8::String::NewFromUtf8(isolate, Canvas::ClassName()).ToLocalChecked());
            // use this count latter
            constructor_tpl->InstanceTemplate()->SetInternalFieldCount(1);

            ADD_PROTO_METHOD(isolate, constructor_tpl, "getContext", GetContextCallback);
            ADD_PROTO_METHOD(isolate, constructor_tpl, "getMsg", GetMsgCallback);
            ADD_PROTO_METHOD(isolate, constructor_tpl, "setMsg", SetMsgCallback);

            ADD_ACCESSOR(isolate, constructor_tpl, "height", GetterHeight, SetterHeight);
            ADD_ACCESSOR(isolate, constructor_tpl, "width", GetterWidth, SetterWidth);

            global->Set(v8::String::NewFromUtf8(isolate, "Canvas").ToLocalChecked(), constructor_tpl);
        }

        Canvas::Canvas(v8::Handle<v8::Object> instance) : V8Object<Canvas>(instance), _width(600), _height(400),
                                                          _msg("none") {
            webGl2RenderingContext = new WebGL2RenderingContext(_width, _height);
        }

        Canvas::~Canvas() {
            printf("Canvas destruct");
        }

        void Canvas::Destroy(const v8::WeakCallbackInfo<V8Object<Canvas>> &args) {
            printf("Canvas do Destroy");
        }

        void Canvas::ConstructorCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::HandleScope handle_scope(args.GetIsolate());
            if (args.IsConstructCall()) {
                // create instance and wrap to `this`
                new Canvas(args.This());
                args.GetReturnValue().Set(args.This());
            }
        }

        void Canvas::GetContextCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            void *ptr = args.Holder()->GetAlignedPointerFromInternalField(0);
            Canvas *canvas = static_cast<Canvas *>(ptr);

            args.GetReturnValue().Set(canvas->GetWebGLContext()->Persistent().Get(isolate));
        }

        void Canvas::GetMsgCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            Canvas *canvas = Canvas::GetInstanceFromV8(args.Holder());
            args.GetReturnValue().Set(
                    v8::String::NewFromUtf8(isolate, (canvas->_msg).c_str()).ToLocalChecked());
        }

        void Canvas::SetMsgCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            Canvas *canvas = Canvas::GetInstanceFromV8(args.Holder());
            v8::String::Utf8Value utf8(isolate, args[0]);
            canvas->_msg = *utf8;
        }

        WebGL2RenderingContext *Canvas::GetWebGLContext() {
            return webGl2RenderingContext;
        }

        void Canvas::GetterHeight(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value> &info) {
            Canvas *canvas = Canvas::UnWrap(info.Holder());
            info.GetReturnValue().Set(v8::Integer::New(info.GetIsolate(), canvas->_height));
        }

        void Canvas::SetterHeight(v8::Local<v8::String> name, v8::Local<v8::Value> value,
                                  const v8::PropertyCallbackInfo<void> &info) {
            Canvas *canvas = Canvas::UnWrap(info.Holder());
            canvas->_height = value->Int32Value(info.GetIsolate()->GetCurrentContext()).FromJust();
        }

        void Canvas::GetterWidth(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value> &info) {
            Canvas *canvas = Canvas::UnWrap(info.Holder());
            info.GetReturnValue().Set(v8::Integer::New(info.GetIsolate(), canvas->_width));
        }

        void Canvas::SetterWidth(v8::Local<v8::String> name, v8::Local<v8::Value> value,
                                  const v8::PropertyCallbackInfo<void> &info) {
            Canvas *canvas = Canvas::UnWrap(info.Holder());
            canvas->_width = value->Int32Value(info.GetIsolate()->GetCurrentContext()).FromJust();
        }
    }
}