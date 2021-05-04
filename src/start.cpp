#include <string>

#include "V8Runner.h"
#include "applications/w8/w8.h"

int main(int argc, char *argv[]) {

    w8::Initialize(argv);
    w8 *app = new w8();
    app->OpenWindow();
    app->Run();
    app->Stop();
    delete app;
    V8Runner::Dispose();

    return 0;
}