#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    Group::Group(H5::Group group) {
        
        m_group = group;
        
    }
    
    Persistent<Function> Group::Constructor;
    
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
        File* file = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        
        // create group
        Group* group = new Group(file->FileObject()->openGroup(*group_name));
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewSymbol("id"), Number::New(group->m_group.getId()));
        
        return args.This();
        
    }
    
    Handle<Value> Group::Instantiate (const char* name, Local<Object> file) {
        
        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[2] = {
                
                Local<Value>::New(String::New(name)),
                file
                
        };
        
        // return new group instance
        return scope.Close(Constructor->NewInstance(2, argv));
        
    }

};