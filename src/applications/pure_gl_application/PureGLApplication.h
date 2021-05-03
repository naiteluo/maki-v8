#ifndef MAKI_PUREGLAPPLICATION_H
#define MAKI_PUREGLAPPLICATION_H

#include <GLFW/glfw3.h>
#include "V8Runner.h"

class PureGLApplication {
private:
    GLFWwindow *_window;

    V8Runner *_runner;

    void initializeGLEW();

public:
    static int Initialize();

    PureGLApplication(V8Runner *runner);

    ~PureGLApplication();

    int OpenWindow();

    void Run();

    void Stop();
};

#endif //MAKI_PUREGLAPPLICATION_H