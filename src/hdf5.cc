#include <node.h>
#include "hdf5.h"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init (Handle<Object> target) {
        
        // create local scope
        HandleScope scope;
        
        // initialize wrapped objects
        File::Initialize(target);
        
        // initialize wrapped object factories
        Group::Initialize();
        
    }
    
    NODE_MODULE(hdf5, init)

}