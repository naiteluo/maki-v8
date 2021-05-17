//
// Created by Jiawei Tan on 2021/5/9.
//

#include "gl.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common_from_ogl/text2D.hpp>
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
            ADD_PROTO_METHOD(isolate, constructor_tpl_local, "tick", TickCallback);
            ADD_PROTO_METHOD(isolate, constructor_tpl_local, "loadText2D", LoadText2DCallback);
            ADD_PROTO_METHOD(isolate, constructor_tpl_local, "drawText", DrawTextCallback);

            // add constant

            ADD_CONSTANT(isolate, constructor_tpl_local, "COLOR_BUFFER_BIT",
                         v8::Integer::New(isolate, GL_COLOR_BUFFER_BIT));
            ADD_CONSTANT(isolate, constructor_tpl_local, "ARRAY_BUFFER", v8::Integer::New(isolate, GL_ARRAY_BUFFER));
            ADD_CONSTANT(isolate, constructor_tpl_local, "DEPTH_TEST", v8::Integer::New(isolate, GL_DEPTH_TEST));
            ADD_CONSTANT(isolate, constructor_tpl_local, "LEQUAL", v8::Integer::New(isolate, GL_LEQUAL));
            ADD_CONSTANT(isolate, constructor_tpl_local, "DEPTH_BUFFER_BIT",
                         v8::Integer::New(isolate, GL_DEPTH_BUFFER_BIT));
            ADD_CONSTANT(isolate, constructor_tpl_local, "FLOAT", v8::Integer::New(isolate, GL_FLOAT));
            ADD_CONSTANT(isolate, constructor_tpl_local, "TRIANGLE_STRIP",
                         v8::Integer::New(isolate, GL_TRIANGLE_STRIP));
            ADD_CONSTANT(isolate, constructor_tpl_local, "VERTEX_SHADER", v8::Integer::New(isolate, GL_VERTEX_SHADER));
            ADD_CONSTANT(isolate, constructor_tpl_local, "FRAGMENT_SHADER",
                         v8::Integer::New(isolate, GL_FRAGMENT_SHADER));
            ADD_CONSTANT(isolate, constructor_tpl_local, "LINK_STATUS", v8::Integer::New(isolate, GL_LINK_STATUS));
            ADD_CONSTANT(isolate, constructor_tpl_local, "COMPILE_STATUS",
                         v8::Integer::New(isolate, GL_COMPILE_STATUS));
            ADD_CONSTANT(isolate, constructor_tpl_local, "FLOAT", v8::Integer::New(isolate, GL_FLOAT));
            ADD_CONSTANT(isolate, constructor_tpl_local, "FLOAT", v8::Integer::New(isolate, GL_FLOAT));

            constructor_tpl.Reset(isolate, constructor_tpl_local);

            global->Set(v8::String::NewFromUtf8(isolate, ClassName()).ToLocalChecked(), constructor_tpl_local);
        }

        WebGL2RenderingContext::WebGL2RenderingContext(int w, int h)
                : V8Object<WebGL2RenderingContext>(), width_(w), height_(h) {

            // Initialise GLFW
            if (!glfwInit()) {
                fprintf(stderr, "Failed to initialize GLFW\n");
                getchar();
                return;
            }
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                           GL_TRUE); // To make MacOS happy; should not be needed
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            context_env_ = glfwCreateWindow(width_, height_, "WebGL2RenderingContext", NULL, NULL);
            if (context_env_ == NULL) {
                fprintf(stderr,
                        "Failed to open GLFW window. If you have an Intel GPU, they are "
                        "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
                getchar();
                glfwTerminate();
            }
            EnterContext();
        }

        void WebGL2RenderingContext::ClearCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            int mask = args[0]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromJust();
            glClear(mask);
        }

        void WebGL2RenderingContext::ClearColorCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            int r = args[0]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromJust();
            int g = args[1]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromJust();
            int b = args[2]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromJust();
            int a = args[3]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromJust();
            glClearColor(r, g, b, a);
        }

        void WebGL2RenderingContext::TickCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            WebGL2RenderingContext *context = WebGL2RenderingContext::UnWrap(args.Holder());
            // Swap buffers
            glfwSwapBuffers(context->context_env_);
            glfwPollEvents();
            int f;
            if (!(glfwGetKey(context->context_env_, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                  glfwWindowShouldClose(context->context_env_) == 0)) {
                f = 0;
            } else {
                f = 1;
            }
            args.GetReturnValue().Set(v8::Integer::New(isolate, f));
        }

        WebGL2RenderingContext::~WebGL2RenderingContext() {
            printf("WebGL2RenderingContext do Destroy");
        }

        void WebGL2RenderingContext::Destroy(const v8::WeakCallbackInfo<V8Object<WebGL2RenderingContext>> &args) {
            printf("WebGL2RenderingContext do Destroy");
            glDeleteVertexArrays(1, &TextVertexArrayID);
            cleanupText2D();
            glfwTerminate();
        }

        void WebGL2RenderingContext::EnterContext() {
            assert(context_env_ != NULL);
            glfwMakeContextCurrent(context_env_);
            // Ensure we can capture the escape key being pressed below
            glfwSetInputMode(context_env_, GLFW_STICKY_KEYS, GL_TRUE);

            // todo: need init glew per glfw instance?
            // note: make context before init glew
            // Initialize GLEW
            if (glewInit() != GLEW_OK) {
                fprintf(stderr, "Failed to initialize GLEW\n");
                getchar();
                glfwTerminate();
            }
        }

        void WebGL2RenderingContext::LoadText2DCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::String::Utf8Value path(args.GetIsolate(),
                                       args[0]->ToString(args.GetIsolate()->GetCurrentContext()).ToLocalChecked());
            v8::Isolate *isolate = args.GetIsolate();
            WebGL2RenderingContext *context = WebGL2RenderingContext::UnWrap(args.Holder());
            glGenVertexArrays(1, &context->TextVertexArrayID);
            glBindVertexArray(context->TextVertexArrayID);
            initText2D(*path);
        }


        void WebGL2RenderingContext::DrawTextCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
            v8::Isolate *isolate = args.GetIsolate();
            v8::Local<v8::Context> context = isolate->GetCurrentContext();

            v8::Local<v8::Value> arg = args[0];
            v8::String::Utf8Value str(isolate, arg);

            v8::Local<v8::Value> arg1 = args[1];
            int x = arg1->Int32Value(context).ToChecked();
            v8::Local<v8::Value> arg2 = args[2];
            int y = arg2->Int32Value(context).ToChecked();
            v8::Local<v8::Value> arg3 = args[3];
            int size = arg3->Int32Value(context).ToChecked();

            printText2D(*str, x, y, size);
        }

        void WebGL2RenderingContext::CreateShaderCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::ShaderSourceCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::CompileShaderCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::GetShaderParameterCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::GetShaderInfoLogCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::DeleteShaderCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::CreateProgramCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::AttachShaderCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::LinkProgramCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::GetProgramParameterCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::ClearDepthCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::EnableCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::BindBufferCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::VertexAttribPointerCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::EnableVertexAttribArrayCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::UseProgramCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::UniformMatrix4fvCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }

        void WebGL2RenderingContext::DrawArraysCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {

        }
    }
}
