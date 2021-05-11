//
// Created by Jiawei Tan on 2021/5/10.
//

#ifndef MAKI_V8_BINDING_H
#define MAKI_V8_BINDING_H

#include "v8.h"

namespace w8 {
    void inline
    ADD_PROTO_METHOD(v8::Isolate *isolate, v8::Local <v8::FunctionTemplate> constructor_tpl, const char *name,
                     v8::FunctionCallback callback) {
        v8::Local <v8::String> func_name = v8::String::NewFromUtf8(isolate, name).ToLocalChecked();
        v8::Local <v8::Signature> signature = v8::Signature::New(isolate, constructor_tpl);
        v8::Local <v8::FunctionTemplate> proto_method_tpl = v8::FunctionTemplate::New(isolate,
                                                                                      callback,
                                                                                      v8::Local<v8::Value>(),
                                                                                      signature);

        proto_method_tpl->SetClassName(func_name);
        constructor_tpl->PrototypeTemplate()->Set(func_name, proto_method_tpl);
    };

    /**
     * Simplest base class to help construct bi-direction bridge between v8 object and c++ object.
     * inherit it when you need binding a c++ class object to v8 object.
     *
     * inner terms' desc:
     *
     *  - instance: a cpp class instance ptr
     *  - handle: a v8 object handle (indeed dynamic ptr to ptr), we can access to handle in v8 callback
     *  - handle_: a persistent handle of v8 object `handle` handled, store it so we can access it in c++ life circle outside v8 callback
     *
     * To understand wrap and unwrap, read the References:
     *
     *  - https://v8.dev/docs/embed
     *
     * @tparam T
     */
    template<class T>
    class V8Object {
    public:

        inline static T *UnWrap(v8::Handle <v8::Object> handle) {
            return GetInstanceFromV8(handle);
        }

        inline static T *GetInstanceFromV8(v8::Handle <v8::Object> handle) {
            return static_cast<T *>(handle->GetAlignedPointerFromInternalField(0));
        }

        /**
         * manually wrap
         */
        inline void Wrap(v8::Handle <v8::Object> handle) {
            assert(handle_.IsEmpty());
            assert(handle->InternalFieldCount() > 0);
            handle->SetAlignedPointerInInternalField(0, this);
            handle_.Reset(handle->GetIsolate(), handle);
            handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
        }

    protected:
        // todo: remove inline constructor de destructor
        /**
         * automatically wrap when creating
         * @param handle
         */
        inline V8Object(v8::Handle <v8::Object> handle) {
            assert(handle->InternalFieldCount() > 0);
            // store cpp class instance to internal field;
            handle->SetAlignedPointerInInternalField(0, this);
            // keep persistent handle in cpp instance;
            handle_.Reset(handle->GetIsolate(), handle);
            handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
        };

        inline V8Object();

        virtual ~V8Object() {
            if (handle_.IsEmpty()) {
                return;
            }
            handle_.ClearWeak();
            handle_.Reset();
        };

        /**
         * implement this to handle destruction in child class
         * @param args
         */
        virtual void Destroy(const v8::WeakCallbackInfo <V8Object<T>> &args) {

        }

    private:
        v8::Persistent <v8::Value> handle_;

        inline static void WeakCallback(const v8::WeakCallbackInfo <V8Object<T>> &args) {
            printf("WeakCallback trigger");
            V8Object<T> *instance = args.GetParameter();
            instance->Destroy(args);
            // reset persistent to Empty, so it can be re-allocate by v8
            instance->handle_.Reset();
            // remove instance
            delete instance;
        }
    };
}

#endif //MAKI_V8_BINDING_H
