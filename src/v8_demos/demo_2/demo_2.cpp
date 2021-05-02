//
// Created by Jiawei Tan on 2021/5/2.
//

#include <v8.h>
#include "demo_2.h"

void run_demo2(v8::Isolate *isolate, char **argv) {

    v8::Local<v8::Context> context(isolate->GetCurrentContext());

    // Use the JavaScript API to generate a WebAssembly module.
    //
    // |bytes| contains the binary format for the following module:
    //
    //     (func (export "add") (param i32 i32) (result i32)
    //       get_local 0
    //       get_local 1
    //       i32.add)
    //
    const char csource[] = R"(
        log('i am in demo2.');

        let bytes = new Uint8Array([
          0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x01,
          0x60, 0x02, 0x7f, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07,
          0x07, 0x01, 0x03, 0x61, 0x64, 0x64, 0x00, 0x00, 0x0a, 0x09, 0x01,
          0x07, 0x00, 0x20, 0x00, 0x20, 0x01, 0x6a, 0x0b
        ]);
        let module = new WebAssembly.Module(bytes);
        let instance = new WebAssembly.Instance(module);
        instance.exports.add(3, 4);
      )";

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source =
            v8::String::NewFromUtf8Literal(isolate, csource);

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to a uint32 and print it.
    uint32_t number = result->Uint32Value(context).ToChecked();
    printf("3 + 4 = %u\n", number);
}