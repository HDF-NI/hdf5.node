#include <node.h>
#include "hdf5.h"
#include "h5_lt.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init_lt (Handle<Object> target) {
        
        // create local scope
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // initialize wrapped objects
        H5lt::Initialize(target);
        
    }
    
    NODE_MODULE(h5lt, init_lt)

}