//
// Created by Jiawei Tan on 2021/5/9.
//

#ifndef MAKI_GL_H
#define MAKI_GL_H

// Include GLEW first
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <w8/v8-binding.h>
#include "v8.h"

namespace w8 {
    namespace gfx {
        class WebGL2RenderingContext : public V8Object<WebGL2RenderingContext> {
        public:
            static void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);

            static void ClearCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void ClearColorCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            /**
             * @deprecated
             * @param args
             */
            static void TickCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void LoadText2DCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void DrawTextCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

            static const char *const ClassName() { return "WebGL2RenderingContext"; }

            void EnterContext();

        protected:

            WebGL2RenderingContext(int width, int height);

            ~WebGL2RenderingContext();

            void Destroy(const v8::WeakCallbackInfo<V8Object<WebGL2RenderingContext>> &args) override;

        private:
            v8::Persistent<v8::Object> instance_;
            int width_;
            int height_;
            GLFWwindow *context_env_;

            GLuint TextVertexArrayID;

            friend class Canvas;
        };


    }
}


#endif //MAKI_GL_H
