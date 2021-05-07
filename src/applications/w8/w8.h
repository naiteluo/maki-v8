#ifndef MAKI_W8_H
#define MAKI_W8_H

#include <GLFW/glfw3.h>
#include <memory>
#include <v8.h>
#include "uv.h"

#include "utils/common.h"

namespace w8 {

    const char *ToCString(const v8::String::Utf8Value &value);

    void PrintException(v8::Isolate *isolate, v8::TryCatch *try_catch);

    class App {
    private:
        v8::Isolate::CreateParams create_params;
        GLuint VertexArrayID;
        static App *instance;

    public:

        static char **argv;
        static double lastTime;
        static int nbFrames;
        static v8::Isolate *isolate;
        static GLFWwindow *window;
        static std::unique_ptr<v8::Platform> platform;
        static uv_loop_t *loop;

        static inline void SetInstance(App *_instance) {
            instance = _instance;
        };

        static inline App *GetInstance() {
            return instance;
        }

        static int Initialize(char **argv);

        static void Dispose();

        static void JSFuncGLText(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncGLClear(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncGLFWTick(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncPrint(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncLog(const v8::FunctionCallbackInfo<v8::Value> &args);

        App();

        ~App();

        int OpenWindow();

        void Run();

        void Stop();

        v8::Local<v8::Context> CreateAppContext(v8::Isolate *isolate);

    };
}
#endif //MAKI_W8_H