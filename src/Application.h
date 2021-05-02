#ifndef MAKI_APPLICATION_H
#define MAKI_APPLICATION_H
#include <GLFW/glfw3.h>

class Application {
private:
  GLFWwindow *_window;
  void initializeGLEW();

public:
  static int Initialize();
  Application();
  ~Application();
  int OpenWindow();
  void Run();
  void Stop();
};
#endif //MAKI_APPLICATION_H