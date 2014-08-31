#include <node.h>
#include "hdf5.h"
#include "h5_lt.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init_hl (Handle<Object> target) {
        
        // create local scope
        HandleScope scope;
        
        // initialize wrapped objects
        H5lt::Initialize(target);
    ArrayBuffer::Initialize();
    Float64Array::Initialize();
    Float32Array::Initialize();
    Uint32Array::Initialize();
    Int32Array::Initialize();
    Uint8Array::Initialize();
    Int8Array::Initialize();
        
    }
    
    NODE_MODULE(h5lt, init_hl)

}