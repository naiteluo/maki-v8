//
// Created by Jiawei Tan on 2021/5/9.
//

#ifndef MAKI_GL_H
#define MAKI_GL_H

#include "v8.h"

namespace w8 {
    namespace gl {

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
