#pragma once

#include <v8.h>

namespace NodeHDF5 {

  static void getLibVersion(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void isHDF5(const v8::FunctionCallbackInfo<v8::Value>& args);
  
}
