cmake_minimum_required(VERSION 3.10)
# define project name
project(maki)
# set cxx version
set(CMAKE_CXX_STANDARD 14)
# define opengl related lib
set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
        )

find_package(OpenGL REQUIRED)

# local include
include_directories(
        src
        lib/glfw/include/
        lib/glm/
        lib/glew-2.2.0/include/
        lib/v8/include/
)

# add sub
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(lib/googletest)
