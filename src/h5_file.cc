#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    File::File (const char* path) {
        
        m_file = new H5::H5File(path, H5F_ACC_RDONLY);
        
    }
    
    File::~File () {
        
        delete m_file;
        
    }
    
    H5::H5File* File::FileObject() {
        
        return m_file;
        
    }

    void File::Initialize (Handle<Object> target) {
        
        HandleScope scope;
        
        // instantiate constructor function template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        // member method prototypes
        SetPrototypeMethod(t, "group", OpenGroup);
        
        // append this function to the target object
        target->Set(String::NewSymbol("File"), t->GetFunction());
        
    }
    
    Handle<Value> File::New (const Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected file path")));
            return scope.Close(Undefined());
            
        }
        
        String::Utf8Value path (args[0]->ToString());
        
        // fail out if file is not valid hdf5
        if (!H5::H5File::isHdf5(*path)) {
            
            ThrowException(v8::Exception::TypeError(String::New("file is not hdf5 format")));
            return scope.Close(Undefined());
            
        }
        
        // create hdf file object
        File* f = new File(*path);
        
        // extend target object with file
        f->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewSymbol("path"), String::New(f->m_file->getFileName().c_str()));
        args.This()->Set(String::NewSymbol("size"), Number::New(f->m_file->getFileSize()));
        args.This()->Set(String::NewSymbol("freeSpace"), Number::New(f->m_file->getFreeSpace()));
        args.This()->Set(String::NewSymbol("objectCount"), Number::New(f->m_file->getObjCount()));
        args.This()->Set(String::NewSymbol("id"), Number::New(f->m_file->getId()));
        
        return args.This();
        
    }
    
    Handle<Value> File::OpenGroup (const Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected name, callback")));
            return scope.Close(Undefined());
            
        }
        
        String::Utf8Value group_name (args[0]->ToString());
        
        // create callback params
        Local<Value> argv[2] = {
                
                Local<Value>::New(Null()),
                Local<Value>::New(Group::Instantiate(*group_name, args.This()))
                
        };
        
        // execute callback
        Local<Function> callback = Local<Function>::Cast(args[1]);
        callback->Call(Context::GetCurrent()->Global(), 2, argv);
        
        return scope.Close(Undefined());
        
    }

};