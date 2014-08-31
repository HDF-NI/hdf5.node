#include <node.h>
#include "hdf5.h"
#include "h5a.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init_a (Handle<Object> target) {
        
        // create local scope
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // initialize wrapped objects
        H5A::Initialize(target);
        
    }
    
    NODE_MODULE(h5a, init_a)

}