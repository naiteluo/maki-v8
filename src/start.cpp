#include <string>

#include "V8Runner.h"
#include "applications/js_gl_application/JSGLApplication.h"

int main(int argc, char *argv[]) {

    V8Runner::Initialize(argv);
    V8Runner *runner = new V8Runner();

    JSGLApplication::Initialize();
    JSGLApplication *app = new JSGLApplication(runner);
    app->OpenWindow();
    app->Run();
    app->Stop();
    delete app;

    runner->Run();
    delete runner;
    V8Runner::Dispose();

    return 0;
}