#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <functional>

#include "hdf5.h"
#include "H5LTpublic.h"

namespace NodeHDF5 {

    class H5pt {
    public:
        static void Initialize (Handle<Object> target) {

            // append this function to the target object
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5pt::make_table)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5pt::read_table)->GetFunction());
        }

        static void make_table (const v8::FunctionCallbackInfo<Value>& args)
        {

        }
        
        static void read_table (const v8::FunctionCallbackInfo<Value>& args)
        {

        }
        
    };
}