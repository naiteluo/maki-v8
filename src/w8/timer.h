//
// Created by Jiawei Tan on 2021/5/5.
//

#ifndef MAKI_W8_TIMER_H
#define MAKI_W8_TIMER_H

#include "v8.h"
#include "uv.h"
#include <map>

namespace w8 {
    namespace timer {
        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);

        class Timer {
        private:
            uv_timer_t *handle;
            int tid;
        public:

            Timer();
            ~Timer();

            v8::Persistent<v8::Function> onTimeout;

            static void JSFuncSetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void OnTimeout(uv_timer_t *handle);

            static void OnTimerClose(uv_handle_t *handle);

            static void JSFuncClearTimeout(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncRunLoop(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncIsLoopAlive(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void JSFuncCloseLoop(const v8::FunctionCallbackInfo<v8::Value> &args);

            static inline std::map<int, Timer *> timer_pool;
            static inline int max_tid = 0;
        };

        class TestRawHeaderClass {
        public:
            static void Print(char *str) {
                printf("TestRawHeaderClass: %s\n", str);
            }
        };
    }
}

#endif //MAKI_W8_TIMER_H
