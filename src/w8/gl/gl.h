//
// Created by Jiawei Tan on 2021/5/9.
//

#ifndef MAKI_GL_H
#define MAKI_GL_H

#include "v8.h"

namespace w8 {
    namespace gl {

        void inline ADD_PROTO_METHOD(v8::Isolate *isolate,v8::Local<v8::FunctionTemplate> constructor_tpl, const char * name, v8::FunctionCallback callback) {
            v8::Local<v8::String> func_name = v8::String::NewFromUtf8(isolate, name).ToLocalChecked();
            v8::Local<v8::Signature> signature = v8::Signature::New(isolate, constructor_tpl);
            v8::Local<v8::FunctionTemplate> proto_method_tpl = v8::FunctionTemplate::New(isolate,
                                                                                         callback,
                                                                                         v8::Local<v8::Value>(),
                                                                                         signature);

            proto_method_tpl->SetClassName(func_name);
            constructor_tpl->PrototypeTemplate()->Set(func_name, proto_method_tpl);
        }

        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);

        class Canvas {
        public:
            static void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);

            static void ConstructorCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void GetContextCallback(const v8::FunctionCallbackInfo<v8::Value> &args);
            static void SayHiCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void SetHiCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static const char *const ClassName() { return "Canvas"; }

        protected:
            Canvas(v8::Handle<v8::Object> instance);

            ~Canvas();

        private:
            v8::Persistent<v8::Object> instance_;
            int _width;
            int _height;
            std::string _hi_str;
        };

    }
}


#endif //MAKI_GL_H
