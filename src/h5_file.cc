
#include <iostream>

#include <node.h>
#include <string>
#include <iostream>
#include <sstream>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    File::File (const char* path) {
        
        m_file = new H5::H5File(path, H5F_ACC_RDONLY);
        gcpl = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED);
        if (err < 0) {
            std::stringstream ss;
            ss << "Failed to set link creation order, with return: " << err << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            return;
        }
        
    }
    
    File::File (const char* path, unsigned long flags) {
        m_file = new H5::H5File(path, toAccessMap[flags]);
        gcpl = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED);
        if (err < 0) {
            std::stringstream ss;
            ss << "Failed to set link creation order, with return: " << err << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            return;
        }
        
    }
    
    File::~File () {
        
        m_file->close();
        delete m_file;
        
    }
    
    Persistent<FunctionTemplate> File::Constructor;
    
    H5::H5File* File::FileObject() {
        
        return m_file;
        
    }

    void File::Initialize (Handle<Object> target) {
        
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // instantiate constructor function template
        Local<FunctionTemplate> t = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
        t->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
        Constructor.Reset(v8::Isolate::GetCurrent(), t);
        // member method prototypes
        NODE_SET_PROTOTYPE_METHOD(t, "createGroup", CreateGroup);
        NODE_SET_PROTOTYPE_METHOD(t, "openGroup", OpenGroup);
        Local<Function> f=t->GetFunction();
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"), t->GetFunction());
        
    }
    
    void File::New (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() <1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected file path")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        String::Utf8Value path (args[0]->ToString());
        
        // fail out if file is not valid hdf5
        if (args.Length() <2 && !H5::H5File::isHdf5(*path)) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "file is not hdf5 format")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        // create hdf file object
        File* f;
        if(args.Length() <2)
            f=new File(*path);
        else
            f=new File(*path,args[1]->ToUint32()->IntegerValue());
        
        // extend target object with file
        f->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "path"), String::NewFromUtf8(v8::Isolate::GetCurrent(), f->m_file->getFileName().c_str()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "size"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getFileSize()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "freeSpace"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getFreeSpace()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "objectCount"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getObjCount()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getId()));
        
        return;
        
    }
    
    void File::CreateGroup (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
//        if (args.Length() != 1 || !args[0]->IsString()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, callback")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
//        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Object> instance=Group::Instantiate(args.This());
        // create callback params
//        Local<Value> argv[2] = {
//                
//                Local<Value>::New(v8::Isolate::GetCurrent(), Null(v8::Isolate::GetCurrent())),
//                Local<Value>::New(v8::Isolate::GetCurrent(), instance)
//                
//        };
////        instance->
//        // execute callback
//        Local<Function> callback = Local<Function>::Cast(args[1]);
//        callback->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), 2, argv);
        
        args.GetReturnValue().Set(instance);
        return;
        
    }
    
    void File::OpenGroup (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() < 1 || args.Length() >2 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Object> instance=Group::Instantiate(*group_name, args.This());
        // create callback params
//        Local<Value> argv[2] = {
//                
//                Local<Value>::New(v8::Isolate::GetCurrent(), Null(v8::Isolate::GetCurrent())),
//                Local<Value>::New(v8::Isolate::GetCurrent(), Group::Instantiate(*group_name, args.This()))
//                
//        };
        
        // execute callback
//        Local<Function> callback = Local<Function>::Cast(args[1]);
//        callback->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), 2, argv);
        
        args.GetReturnValue().Set(instance);
        return;
        
    }

};