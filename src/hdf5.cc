#include <node.h>
#include "hdf5.h"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init (Handle<Object> target) {
        
        // create local scope
        HandleScope scope;
        
        // initialize objects
        File::Initialize(target);
        Group::Initialize(target);
        
    }
    
    NODE_MODULE(hdf5, init)

}