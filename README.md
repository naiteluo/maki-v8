# maki 

Makise Kurisu, a v8 embed opengl renderer.

## Setups (first time step)

1. get gtest from submodule

    ```bash
    git submodule init
    git submodule updata --init --recursive
    ```

2. get v8

    ```
    cd lib

    fetch v8 # it may take a long time

    cd v8

    # start to configure and build
    # create build configuration with helper script
    tools/dev/v8gen.py x64.release.sample
    # inspect configuration
    gn args out.gn/x64.release.sample
    # build (take x64 platform for example)
    ninja -C out.gn/x64.release.sample v8_monolith

    # try to build v8 example helloworld
    g++ -I. -Iinclude samples/hello-world.cc -o hello_world -lv8_monolith -Lout.gn/x64.release.sample/obj/ -pthread -std=c++14 -DV8_COMPRESS_POINTERS
    # run
    ./hello_world

    ```

    Finished! Now you have a linkable static v8 library in `./lib/v8/out.gn/x64.release.sample/obj`

    To use v8 in your cmake project, add this to the `CMakeLists.txt`

    ```
    # link build output of v8 by directories
    link_directories(${CMAKE_SOURCE_DIR}/lib/v8/out.gn/x64.release.sample/obj)
    # link your app to v8
    target_link_libraries(${BINARY}_run v8_monolith)
    ```

    If you already have your v8 project checked out in other postion, you can also change linkable path to your v8 build results path. But **don't commit!!**

    References:

    - [get v8 srouce code](https://v8.dev/docs/source-code)
    - [build for embed](https://v8.dev/docs/embed#advanced-guide)

## Build and run (Debug)

1. Configure

   ```bash
   cmake --no-warn-unused-cli \
       -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
       -DCMAKE_BUILD_TYPE:STRING=Debug \
       -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang \
       -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ \
       -H. \
       -B./build \
       -G Ninja
   ```

2. Build

   ```bash
   cmake --build ./build --config Debug --target all -j 14 --
   ```

3. Run

   run main app

   ```bash
   ./build/src/maki_run
   ```

   run test

   ```bash
   ./build/test/maki_test
   ```

Or just use vscode extensions to configure, build and run in vscode.

- [C/C++ extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake Tools extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

Debug? Use vscode.