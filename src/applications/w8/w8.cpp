// Include GLEW first
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <cstdio>
#include <glm/glm.hpp>
#include <stdio.h>
#include "libplatform/libplatform.h"
#include <v8.h>

#include "w8.h"

#include <common_from_ogl/shader.hpp>
#include <common_from_ogl/text2D.hpp>
#include "uv.h"

std::unique_ptr<v8::Platform> w8::platform = NULL;

int w8::Initialize(char **argv) {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    return 0;
}

void w8::Dispose() {
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}

w8::w8() {
    _create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    _isolate = v8::Isolate::New(_create_params);
}

w8::~w8() {
    _window = NULL;
    _isolate->Dispose();
    delete _create_params.array_buffer_allocator;
}

int w8::OpenWindow() {
    // Open a window and create its OpenGL context
    _window = glfwCreateWindow(1024, 768, "V8", NULL, NULL);
    if (_window == NULL) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are "
                "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

    // note: make context before init glew
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
    }
    return 0;
}

void w8::Run() {

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Initialize our little text library with the Holstein font
    initText2D("Holstein.DDS");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;



    while (v8::platform::PumpMessageLoop(platform.get(), _isolate)) continue;

    do {

        glClear(GL_COLOR_BUFFER_BIT);

        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >=
            1.0) { // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }


        printText2D("Hello V8 & GL", 10, 250, 30);

        // Swap buffers
        glfwSwapBuffers(_window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(_window) == 0);

    glDeleteVertexArrays(1, &VertexArrayID);
    cleanupText2D();
}

void w8::Stop() {
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
