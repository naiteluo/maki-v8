#include "FileReader.h"
#include "v8.h"
#include <cstdio>
#include <memory>
#include <string.h>

using std::string;

using v8::Isolate;
using v8::MaybeLocal;
using v8::NewStringType;
using v8::String;

/**
 * @brief 
 * 
 * @param isolate 
 * @param name 
 * @return MaybeLocal<String> 
 */
MaybeLocal<String> FileReader::read(Isolate *isolate, const string &name) {
  FILE *file = fopen(name.c_str(), "rb");
  if (file == NULL)
    return MaybeLocal<String>();
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  std::unique_ptr<char> chars(new char[size + 1]);
  chars.get()[size] = '\0';
  for (size_t i = 0; i < size;) {
    i += fread(&chars.get()[i], 1, size - i, file);
    if (ferror(file)) {
      fclose(file);
      return MaybeLocal<String>();
    }
  }

  fclose(file);

  MaybeLocal<String> result = String::NewFromUtf8(
      isolate, chars.get(), NewStringType::kNormal, static_cast<int>(size));

  return result;
}