#include <string>

#include "w8/w8.h"

int main(int argc, char *argv[]) {
    w8::App::Initialize(argc, argv);
    w8::App *app = new w8::App();
    w8::App::SetInstance(app);
    app->Run();
    app->Stop();
    delete app;
    w8::App::Dispose();
    return 0;
}