#include <node.h>
#include "node_h5file.h"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init (Handle<Object> target) {
        
        HandleScope scope;
        File::Initialize(target);
        
    }
    
    NODE_MODULE(hdf5, init)

}