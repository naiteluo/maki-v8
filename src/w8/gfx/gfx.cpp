//
// Created by Jiawei Tan on 2021/5/16.
//

#include "gfx.h"
#include "./canvas.h"

namespace w8 {
    namespace gfx {
        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global) {
            Canvas::Initialize(isolate, global);
            WebGL2RenderingContext::Initialize(isolate, global);
        }

    }
}
