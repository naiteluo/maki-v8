#ifndef MAKI_APP_H
#define MAKI_APP_H
#include <GLFW/glfw3.h>

class App {
private:
  GLFWwindow *_window;
  void initializeGLEW();

public:
  static int Intialize();
  App();
  ~App();
  int OpenWindow();
  void Run();
  void Stop();
};
#endif //MAKI_APP_H