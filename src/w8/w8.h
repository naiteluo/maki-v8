#ifndef MAKI_W8_H
#define MAKI_W8_H

#include <GLFW/glfw3.h>
#include <memory>
#include <v8.h>
#include "uv.h"
#include "options.h"

#include "utils/common.h"

namespace w8 {

    const char *ToCString(const v8::String::Utf8Value &value);

    void PrintException(v8::Isolate *isolate, v8::TryCatch *try_catch);

    void PrintException(v8::Isolate *isolate, v8::Local<v8::Value> exception);

    class App {
    private:
        v8::Isolate::CreateParams create_params;
        GLuint VertexArrayID;
        static App *instance;

    public:

        static double lastTime;
        static int nbFrames;
        static v8::Isolate *isolate;
        static GLFWwindow *window;
        static std::unique_ptr<v8::Platform> platform;
        static uv_loop_t *loop;
        static uv_prepare_t *prepare_handle;

        static Options options;

        static inline void SetInstance(App *_instance) {
            instance = _instance;
        };

        static inline App *GetInstance() {
            return instance;
        }

        static int Initialize(int argc, char *argv[]);

        static void Dispose();

        static void JSFuncGLText(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncGLClear(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncGLFWTick(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncPrint(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncLog(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncBootstrap(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncSleep(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void JSFuncPoll(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void OnUVPrepareCallback(uv_prepare_t *handle);

        static bool isLoopContinue();

        App();

        ~App();

        int OpenWindow();

        void Run();

        void Stop();

        v8::Local<v8::Context> CreateAppContext(v8::Isolate *isolate);

    };
}
#endif //MAKI_W8_H