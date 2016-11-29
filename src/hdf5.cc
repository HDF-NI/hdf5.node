#include <node.h>
#include "file.h"
#include "group.h"
#include "filters.hpp"
#include "hdf5node.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

    static void init (Handle<Object> target) {
        
        // create local scope
        HandleScope scope(v8::Isolate::GetCurrent());

        NODE_SET_METHOD(target, "getLibVersion", getLibVersion);
        
        // initialize wrapped objects
        File::Initialize(target);
        Filters::Init(target);
        Int64::Initialize(target);
        
        // initialize wrapped object factories
        Group::Initialize();
        
    }
    
    NODE_MODULE(hdf5, init)

}

namespace NodeHDF5 {
    
    void getLibVersion (const v8::FunctionCallbackInfo<v8::Value>& args) {
        // fail out if arguments are not correct
        if (args.Length() !=0) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected empty")));
            args.GetReturnValue().SetUndefined();
            return;

        }

        // unwrap group
        //HDF5* h = ObjectWrap::Unwrap<HDF5>(args.This());
        // get
        unsigned majnum;
        unsigned minnum;
        unsigned relnum;
        herr_t err=H5get_libversion(&majnum, &minnum, &relnum);
        args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (std::to_string(majnum)+"."+std::to_string(minnum)+"."+std::to_string(relnum)).c_str()));
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get lib version")));
            args.GetReturnValue().SetUndefined();
            return;
        }
    }
    
}
