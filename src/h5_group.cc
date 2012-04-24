#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    using namespace H5;

    Group::Group () {
        
        /* stub */
        
    }
    
    Group::~Group () {
        
        delete m_group;
        
    }
    
    void Group::Initialize (Handle<Object> target) {
        
        HandleScope scope;
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        // specify constructor function
        target->Set(String::NewSymbol("Group"), t->GetFunction());
        
    }
    
    Handle<Value> Group::New (const Arguments& args) {
        
        HandleScope scope;
        
        Group* g = new Group();
        g->Wrap(args.This());
        
        return args.This();
        
    }

};