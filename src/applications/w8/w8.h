#ifndef MAKI_W8_H
#define MAKI_W8_H

#include <GLFW/glfw3.h>
#include <memory>
#include <v8.h>


class w8 {
private:
    v8::Isolate *_isolate;
    v8::Isolate::CreateParams _create_params;
    GLFWwindow *_window;
public:
    static std::unique_ptr<v8::Platform> platform;

    static int Initialize(char **argv);

    static void Dispose();

    w8();

    ~w8();

    int OpenWindow();

    void Run();

    void Stop();
};

#endif //MAKI_W8_H