# maki

Makise Kurisu, a v8 embed opengl renderer.

## Setups (first time step)

1. Get gtest and opengl related module from submodule

   ```bash
   git submodule init
   git submodule updata --init --recursive
   ```

2. Download glew and extra to lib

   [GLEW 2.1.0](https://sourceforge.net/projects/glew/files/glew/2.1.0/)

3. Get v8

   ```bash
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

   ```bash
   # link build output of v8 by directories
   link_directories(${CMAKE_SOURCE_DIR}/lib/v8/out.gn/x64.release.sample/obj)
   # link your app to v8
   target_link_libraries(${BINARY}_run v8_monolith)
   ```

   If you already have your v8 project checked out in other directory, you can also change linkable path to your v8 build results path. But **don't commit!!**

   References:

   - [get v8 srouce code](https://v8.dev/docs/source-code)
   - [build for embed](https://v8.dev/docs/embed#advanced-guide)
   
4. Get `Boost`

We use `websocketpp` which alias `Boost::asio` to handle ws connection between chrome inspector, and the runtime. Install boost as local library deps.

```bash
brew install boost
```

## IDE setups

### CLion (Recommended)

Open project and wait for initializing and file indexing. [Get CLion](https://www.jetbrains.com/clion/).

**notes for debug:**

Create new CMake profile in Preferences panel, set build type to "Debug".
Otherwise, your debugger will not be triggered.

### vscode

vscode recommended extensions for c/c++:

- [C/C++ extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake Tools extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [clangd: C and C++ completion, navigation, and insights](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)

Using clangd in cmake project, you'll need to generate `compile_commands.json` file. 
clangd will look for it to understanding your project build flags or other building context. [Read for more informations.](https://prereleases.llvm.org/8.0.0/rc3/tools/clang/tools/extra/docs/clangd/Installation.html)

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```

**Regenerate** `compile_commands.json` after big changes of the project structure or add new library.

## Build and run

js file, and some assets are in `./resouces`, cd into this directory as you working directory. 

### build and run in Clion (Recommended)

open project, build and run.

### build and run in cli

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

   helps:

   - `CMAKE_BUILD_TYPE:STRING` define build mode.
   - `-G Ninja` specify build system generator.

2. Build

   ```bash
   cmake --build ./build --config Debug --target all -j 14 --
   ```

3. Run

   run w8 app

   ```bash
   ./build-*/src/maki_w8_run
   ```

### build and run in vscode

Or just use vscode extensions to configure, build and run in vscode.

- [C/C++ extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake Tools extensions](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

## Debug

Use vscode or clion for life-saving.

### JavaScript debugging with inspector

With the integration of inspector client and websocket service, we now support js debugging in chrome inspector.

1. Run `maki_w8_run` with `--inspector_enabled` flag. Javascript execution will wait until inspector connection opened.
2. Run `open -n -a /Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome --args --user-data-dir="/tmp/chrome_dev_test" --remote-debugging-port=9222 http://localhost:9222/devtools/inspector.html?ws=localhost:9229`



