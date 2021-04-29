#ifndef MAKI_UTILS_FILEREADER_H
#define MAKI_UTILS_FILEREADER_H

#include "v8.h"
#include <string.h>

using std::string;

using v8::MaybeLocal;
using v8::String;
using v8::Isolate;

class FileReader {
    public:
        static MaybeLocal<String> read(Isolate* isolate, const string& name);
};

#endif //MAKI_UTILS_FILEREADER_H