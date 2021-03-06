#include <string>

#include "V8Runner.h"
#include "applications/pure_gl_application/PureGLApplication.h"

int main(int argc, char *argv[]) {

    V8Runner::Initialize(argv);
    V8Runner *runner = new V8Runner();

    PureGLApplication::Initialize();
    PureGLApplication *app = new PureGLApplication(runner);
    app->OpenWindow();
    app->Run();
    app->Stop();
    delete app;

    runner->Run();
    delete runner;
    V8Runner::Dispose();

    return 0;
}