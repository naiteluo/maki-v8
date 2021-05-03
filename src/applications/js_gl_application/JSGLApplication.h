#ifndef MAKI_JSGLAPPLICATION_H
#define MAKI_JSGLAPPLICATION_H

#include <GLFW/glfw3.h>
#include "V8Runner.h"

class JSGLApplication {
private:
    GLFWwindow *_window;

    V8Runner *_runner;

    void initializeGLEW();

public:
    static int Initialize();

    JSGLApplication(V8Runner *runner);

    ~JSGLApplication();

    int OpenWindow();

    void Run();

    void Stop();
};

#endif //MAKI_JSGLAPPLICATION_H