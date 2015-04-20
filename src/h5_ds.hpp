#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <memory>
#include <iostream>

#include "file.h"
#include "group.h"
#include "H5DSpublic.h"

namespace NodeHDF5 {

    class H5ds {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "setScale"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::set_scale)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "attachScale"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::attach_scale)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "detachScale"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::detach_scale)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "isAttached"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::is_attached)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "isScale"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::is_scale)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "iterateScale"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::iterate_scales)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "setLabel"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::set_label)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getLabel"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_label)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getScaleName"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_scale_name)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getNumberOfScales."), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_num_scales)->GetFunction());
        
    }

static void set_scale (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void attach_scale (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
}

static void detach_scale (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
}

static void is_attached (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
}

static void is_scale (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void iterate_scales (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void set_label (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void get_label (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void get_scale_name (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

static void get_num_scales (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    args.GetReturnValue().SetUndefined();
}

    };
}