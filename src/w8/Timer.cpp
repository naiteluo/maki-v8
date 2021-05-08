//
// Created by Jiawei Tan on 2021/5/5.
//

#include "Timer.h"
#include "w8.h"

namespace w8 {

    namespace timer {

        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            global->Set(isolate, "setTimeout",
                        v8::FunctionTemplate::New(isolate, timer::Timer::JSFuncSetTimeout));
            global->Set(isolate, "clearTimeout",
                        v8::FunctionTemplate::New(isolate, timer::Timer::JSFuncClearTimeout));

            // maintain event loop here temporally.

            global->Set(isolate, "runLoop",
                        v8::FunctionTemplate::New(isolate, timer::Timer::JSFuncRunLoop));
            global->Set(isolate, "isLoopAlive",
                        v8::FunctionTemplate::New(isolate, timer::Timer::JSFuncIsLoopAlive));
            global->Set(isolate, "closeLoop",
                        v8::FunctionTemplate::New(isolate, timer::Timer::JSFuncCloseLoop));
        }


        void Timer::JSFuncSetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = isolate->GetCurrentContext();
            v8::Local<v8::Function> onTimeout = v8::Local<v8::Function>::Cast(args[0]);
            int timeoutValue = args[1]->Int32Value(context).ToChecked();
            // create Timer instance handle
            timer::Timer *timerObj = new timer::Timer();
            timerObj->handle = (uv_timer_t *) calloc(1, sizeof(uv_timer_t));
            uv_timer_init(w8::App::loop, timerObj->handle);
            // store timer instance in handle
            timerObj->handle->data = timerObj;
            timerObj->onTimeout.Reset(isolate, onTimeout);
            int r = uv_timer_start(timerObj->handle, OnTimeout, timeoutValue, 0);
            args.GetReturnValue().Set(v8::Integer::New(isolate, r));
        }

        void Timer::OnTimeout(uv_timer_t *handle) {
            Timer *t = (Timer *) handle->data;
            v8::Isolate *isolate = w8::App::isolate;
            v8::HandleScope handle_scope(isolate);
            const unsigned int argc = 0;
            v8::Local<v8::Value> argv[argc] = {};
            v8::TryCatch try_catch(isolate);
            v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, t->onTimeout);
            v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
            cb->Call(ctx, ctx->Global(), 0, argv);
            if (try_catch.HasCaught()) {
                w8::PrintException(isolate, &try_catch);
            }
        }

        void Timer::JSFuncClearTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void Timer::JSFuncRunLoop(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            isolate->PerformMicrotaskCheckpoint();
            int status = uv_run(App::loop, UV_RUN_DEFAULT);
            args.GetReturnValue().Set(v8::Integer::New(isolate, status));
        }

        void Timer::JSFuncIsLoopAlive(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            int alive = uv_loop_alive(App::loop);
            args.GetReturnValue().Set(v8::Integer::New(isolate, alive));
        }

        void Timer::JSFuncCloseLoop(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::HandleScope handle_scope(isolate);
            int ok = uv_loop_close(App::loop);
            printf("Loop closed.\n");
            args.GetReturnValue().Set(v8::Integer::New(isolate, ok));
        }
    }
}