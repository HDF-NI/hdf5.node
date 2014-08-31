#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <memory>
#include <iostream>

#include "hdf5.h"
#include "H5IMpublic.h"

namespace NodeHDF5 {
    class H5a {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
//        target->Set(String::New("makeImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_image)->GetFunction());
//        target->Set(String::New("readImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::read_image)->GetFunction());
//        target->Set(String::New("isImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::is_image)->GetFunction());
        
    }
        
    };
}