#pragma once
#include <map>

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <cstring>
#include <memory>

namespace NodeHDF5 {

    static void getLibVersion (const v8::FunctionCallbackInfo<v8::Value>& args);

}
