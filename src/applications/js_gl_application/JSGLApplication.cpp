// Include GLEW first
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <cstdio>
#include <glm/glm.hpp>
#include <stdio.h>

#include "JSGLApplication.h"
#include "V8Runner.h"

#include <common_from_ogl/shader.hpp>
#include <common_from_ogl/text2D.hpp>

int JSGLApplication::Initialize() {
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

    return 0;
}

JSGLApplication::JSGLApplication(V8Runner *runner) {
    _runner = runner;
}

JSGLApplication::~JSGLApplication() {
    _window = NULL;
    _runner = NULL;
}

int JSGLApplication::OpenWindow() {
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

void JSGLApplication::Run() {

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("SimpleVertexShader.vertexshader",
                                   "SimpleFragmentShader.fragmentshader");

    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                 g_vertex_buffer_data, GL_STATIC_DRAW);

    // Initialize our little text library with the Holstein font
    initText2D("Holstein.DDS");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    _runner->Run();
    _runner->Prepare();

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        _runner->Loop();

        // Swap buffers
        glfwSwapBuffers(_window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(_window) == 0);
}

void JSGLApplication::Stop() {
    cleanupText2D();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
