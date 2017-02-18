#include <node.h>
#include "file.h"
#include "group.h"
#include "h5_pt.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

static void init_pt(Handle<Object> target) {

  // create local scope
  HandleScope scope(v8::Isolate::GetCurrent());

  // initialize wrapped objects
  Int64::Initialize(target);
  H5pt::Initialize(target);
  PacketTable::Init(target);
}

NODE_MODULE(h5pt, init_pt)
}