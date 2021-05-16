//
// Created by Jiawei Tan on 2021/5/16.
//

#ifndef MAKI_GFX_H
#define MAKI_GFX_H

#include <w8/v8-binding.h>
#include "v8.h"

namespace w8 {
    namespace gfx {

        void Initialize(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global);


    }
}


#endif //MAKI_GFX_H
