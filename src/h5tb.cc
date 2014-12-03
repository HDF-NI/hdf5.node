#include <node.h>
#include "hdf5.h"
#include "h5_tb.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init_tb (Handle<Object> target) {
        
        // create local scope
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // initialize wrapped objects
        H5tb::Initialize(target);
        
    }
    
    NODE_MODULE(h5tb, init_tb)

}