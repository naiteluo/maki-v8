//
// Created by Jiawei Tan on 2021/5/2.
//

#include <v8.h>
#include <utils/FileReader.h>
#include "demo_1.h"

void run_demo1(v8::Isolate *isolate, char **argv) {
    // Enter context
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    v8::Local<v8::String> source;
    std::string filePath;
    if (argv[1] == NULL) {
        filePath = "index.js";
    } else {
        filePath = argv[1];
    }

    if (!FileReader::read(isolate, filePath).ToLocal(&source)) {
        fprintf(stderr, "Error reading '%s'.\n", filePath.c_str());
    } else {
        // Compile the source code.
        v8::Local<v8::Script> script =
                v8::Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        // Convert the result to an UTF8 string and print it.
        v8::String::Utf8Value utf8(isolate, result);
        printf("%s\n", *utf8);
    }
}