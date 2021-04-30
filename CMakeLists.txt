cmake_minimum_required(VERSION 3.10)
# define project name
project(maki)
# set cxx version
set(CMAKE_CXX_STANDARD 14)
set(ALL_LIBS

)

find_package(OpenGL REQUIRED)

# local include
include_directories(
    src
    lib/glfw/include/
    lib/glm/glm/
    lib/glew/include/
)
# add v8 include
include_directories(${CMAKE_SOURCE_DIR}/lib/v8/include)

# debug log
message(${CMAKE_SOURCE_DIR}/../../v8/v8/include)

# add sub
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(lib/googletest)
