#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;

    File::File (std::string path) {
    
        m_file = new H5::H5File(path.c_str(), H5F_ACC_RDONLY);
        
    }
    
    File::~File () {
        
        // close file
        m_file->close();
        
        // free up memory
        delete m_file;
        
    }
    
    void File::Initialize (Handle<Object> target) {
        
        HandleScope scope;
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        // member method prototypes
        SetPrototypeMethod(t, "group", OpenGroup);
        
        // specify constructor function
        target->Set(String::NewSymbol("File"), t->GetFunction());
        
    }
    
    Handle<Value> File::New (const Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected file path")));
            return scope.Close(Undefined());
            
        }
        
        // store specified file path
        String::Utf8Value path (args[0]->ToString());
        
        // also store cstring version
        std::string c_path (*path);
        
        if (!H5::H5File::isHdf5(c_path.c_str())) {
            
            // fail out if file is not valid hdf5
            ThrowException(v8::Exception::TypeError(String::New("file is not hdf5 format")));
            return scope.Close(Undefined());
            
        }
        
        // instantiate h5 file object
        File* f = new File(c_path);
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
        
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Value> group_argv[] = {
                
                args[0],
                args.This()
                
        };
        
        // create callback params
        Local<Value> argv[] = {
                
                Local<Value>::New(Null()),
                Group::Instantiate(group_argv)
                
        };
        
        // execute callback
        Local<Function> callback = Local<Function>::Cast(args[1]);
        callback->Call(Context::GetCurrent()->Global(), 2, argv);
        
        return scope.Close(Undefined());
        
    }

};