#include <node.h>
#include "hdf5.h"
#include "h5_im.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init_im (Handle<Object> target) {
        
        // create local scope
        v8::HandleScope scope;
        
        // initialize wrapped objects
        H5im::Initialize(target);
        
    ArrayBuffer::Initialize();
        Uint8Array::Initialize();
        Int8Array::Initialize();
        
    }
    
    NODE_MODULE(h5im, init_im)

}