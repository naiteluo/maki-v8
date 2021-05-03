//
// Created by Jiawei Tan on 2021/5/2.
//

#ifndef MAKI_V8RUNNER_H
#define MAKI_V8RUNNER_H

#include "libplatform/libplatform.h"
#include <v8.h>

class V8Runner {
private:
    v8::Isolate *_isolate;
    v8::Isolate::CreateParams _create_params;

public:
    V8Runner();
    ~V8Runner();
    static char **argv;
    /**
     * initial v8
     * craete platform and initialize
     * @param argv
     */
    static void Initialize(char **argv);
    /**
     * dispose v8
     */
    static void Dispose();
    static std::unique_ptr<v8::Platform> platform;
    /**
     * Main entry of JS Demos
     */
    void Run();

    void Prepare();

    void Loop();
};


#endif //MAKI_V8RUNNER_H
