#include <node.h>
#include "file.h"
#include "group.h"
#include "h5_ds.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

static void init_ds(Local<Object> target) {

  // create local scope
  HandleScope scope(v8::Isolate::GetCurrent());

  // initialize wrapped objects
  H5ds::Initialize(target);
}

NODE_MODULE(h5ds, init_ds)
}