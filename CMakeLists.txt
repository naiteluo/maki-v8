cmake_minimum_required(VERSION 3.10)
project(maki)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

# add v8 include
include_directories(${CMAKE_SOURCE_DIR}/../../v8/v8/include)

message(${CMAKE_SOURCE_DIR}/../../v8/v8/include)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
