#include <node.h>
#include "file.h"
#include "group.h"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init (Handle<Object> target) {
        
        // create local scope
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // initialize wrapped objects
        File::Initialize(target);
        
        // initialize wrapped object factories
        Group::Initialize();
        
    }
    
    NODE_MODULE(hdf5, init)

}