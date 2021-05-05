//
// Created by Jiawei Tan on 2021/5/5.
//

#ifndef MAKI_TIMER_H
#define MAKI_TIMER_H

#include "v8.h"
#include "uv.h"

namespace w8 {
    namespace timer {
        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);

        class Timer {
        private:
            uv_timer_t *handle;
        public:
            v8::Persistent<v8::Function> onTimeout;

            static void JSFuncSetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void OnTimeout(uv_timer_t *handle);

            static void JSFuncClearTimeout(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncRunLoop(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncIsLoopAlive(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncCloseLoop(const v8::FunctionCallbackInfo<v8::Value> &args);
        };
    }
}

#endif //MAKI_TIMER_H
