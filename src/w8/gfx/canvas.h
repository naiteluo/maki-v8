//
// Created by Jiawei Tan on 2021/5/16.
//

#ifndef MAKI_CANVAS_H
#define MAKI_CANVAS_H

#include "v8.h"
#include "./gl.h"
#include "../v8-binding.h"

namespace w8 {
    namespace gfx {
        class Canvas : public V8Object<Canvas> {
        public:
            static void Initialize(v8::Isolate *isolate, v8::Local <v8::ObjectTemplate> global);

            static void ConstructorCallback(const v8::FunctionCallbackInfo <v8::Value> &args);

            static void GetContextCallback(const v8::FunctionCallbackInfo <v8::Value> &args);

            static void SayHiCallback(const v8::FunctionCallbackInfo <v8::Value> &args);

            static void SetHiCallback(const v8::FunctionCallbackInfo <v8::Value> &args);

            static const char *const ClassName() { return "Canvas"; }

            WebGL2RenderingContext * GetWebGLContext();

        protected:
            Canvas(v8::Handle <v8::Object> instance);

            ~Canvas();

            void Destroy(const v8::WeakCallbackInfo <V8Object<Canvas>> &args) override;

        private:
            v8::Persistent <v8::Object> instance_;
            int _width;
            int _height;
            std::string _hi_str;

            WebGL2RenderingContext *webGl2RenderingContext;
        };
    }
}

#endif //MAKI_CANVAS_H
