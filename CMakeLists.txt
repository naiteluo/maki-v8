cmake_minimum_required(VERSION 3.10)
# define project name
project(maki)

find_package(Boost REQUIRED COMPONENTS date_time)
include_directories(
        ${Boost_INCLUDE_DIR}
)
link_directories(
        ${Boost_LIBRARY_DIRS}
)
set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)

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
        lib/libuv/include/
        lib/websocketpp/
)

# add sub
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(lib/googletest)
add_subdirectory(lib/libuv)
add_subdirectory(lib/websocketpp)
