#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    Group::Group(H5::Group group) {
        
        m_group = group;
        
    }
    
    void Group::Initialize () {
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        
        // set properties
        t->SetClassName(String::New("Group"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        // initialize constructor reference
        Constructor = Persistent<Function>::New(t->GetFunction());
        
    }
    
    Handle<Value> Group::New (const Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
            return scope.Close(Undefined());
            
        }
        
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        
        // unwrap file object
        File* f = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        
        // create group
        Group* group = new Group(f->m_file->openGroup(*group_name));
        group->Wrap(args.This());
        
        return args.This();
        
    }
    
    Handle<Value> Group::Instantiate (const Arguments& args) {
        
        HandleScope scope;
        
        // group name and file reference
        Handle<Value> argv[2] = {
            
            args[0],
            args[1]
            
        };
        
        // return new group instance
        return scope.Close(Constructor->NewInstance(2, argv));
        
    }

};