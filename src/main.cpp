#include <string>

#include "V8Runner.h"
#include "Application.h"

int main(int argc, char *argv[]) {

    V8Runner::Initialize(argv);
    V8Runner *runner = new V8Runner();

    runner->Run();
    delete runner;
    V8Runner::Dispose();

    Application::Initialize();
    Application *app = new Application();
    app->OpenWindow();
    app->Run();
    app->Stop();
    delete app;

    return 0;
}