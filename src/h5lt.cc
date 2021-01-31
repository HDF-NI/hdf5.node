#include <node.h>
#include "file.h"
#include "group.h"
#include "h5_lt.hpp"
#include "reference.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

static void init_lt(v8::Local<v8::Object> target) {

  // create local scope
  HandleScope scope(v8::Isolate::GetCurrent());

  // initialize wrapped objects
  Int64::Initialize(target);
  H5lt::Initialize(target);
  Reference::Init(target);
}

NODE_MODULE(h5lt, init_lt)
}