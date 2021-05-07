#include <v8.h>
#include "Canvas.h"

void Canvas::Init(v8::Local<v8::ObjectTemplate> global) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate);
  ctor->SetClassName(v8::String::NewFromUtf8(isolate, "Canvas").ToLocalChecked());
  // v8::Local<v8::ObjectTemplate> proto = ctor->PrototypeTemplate();
  // proto->Set(isolate, "x", v8::Number::New(isolate, 2));
  v8::Local<v8::ObjectTemplate> object = v8::ObjectTemplate::New(isolate, ctor);
  object->Set(isolate, "x", v8::Number::New(isolate, 3));
  global->Set(isolate, "canvas", object);
  global->Set(isolate, "Canvas", ctor);
}