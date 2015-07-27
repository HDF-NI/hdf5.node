#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <functional>
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
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getNumberOfScales"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_num_scales)->GetFunction());
        
    }

static void set_scale (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 3 || !args[0]->IsUint32() || !args[1]->IsString() || !args[2]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    String::Utf8Value dim_scale_name (args[2]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    herr_t err=H5DSset_scale(did, *dim_scale_name);
    if(err<0){
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed setting dimension scale")));
        args.GetReturnValue().SetUndefined();
        return;
        
    }
    H5Dclose(did);
    args.GetReturnValue().SetUndefined();
}

static void attach_scale (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 4 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    String::Utf8Value dim_scale_name (args[2]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    hid_t dsid=H5Dopen(args[0]->ToInt32()->Value(), *dim_scale_name, H5P_DEFAULT);
    herr_t err=H5DSattach_scale(did, dsid, args[3]->ToInt32()->Value());
    if(err<0){
        H5Dclose(dsid);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed attaching dimension scale")));
        args.GetReturnValue().SetUndefined();
        return;
        
    }
    H5Dclose(dsid);
    H5Dclose(did);
    args.GetReturnValue().SetUndefined();
}

static void detach_scale (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 4 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    String::Utf8Value dim_scale_name (args[2]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    hid_t dsid=H5Dopen(args[0]->ToInt32()->Value(), *dim_scale_name, H5P_DEFAULT);
    herr_t err=H5DSdetach_scale(did, dsid, args[3]->ToInt32()->Value());
    if(err<0){
        H5Dclose(dsid);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed attaching dimension scale")));
        args.GetReturnValue().SetUndefined();
        return;
        
    }
    H5Dclose(dsid);
    H5Dclose(did);
    args.GetReturnValue().SetUndefined();
}

static void is_attached (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 4 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    String::Utf8Value dim_scale_name (args[2]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    hid_t dsid=H5Dopen(args[0]->ToInt32()->Value(), *dim_scale_name, H5P_DEFAULT);
    htri_t status=H5DSis_attached(did, dsid, args[3]->ToInt32()->Value());
    args.GetReturnValue().Set((status) ? true : false);
    H5Dclose(dsid);
    H5Dclose(did);
}

static void is_scale (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 2 || !args[0]->IsInt32() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    htri_t status=H5DSis_scale(did);
    args.GetReturnValue().Set((status) ? true : false);
    H5Dclose(did);
}

static void iterate_scales (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    int rank;
    H5LTget_dataset_ndims (args[0]->ToInt32()->Value(), *dset_name, &rank);
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    int idx=args[2]->ToInt32()->Value();
        v8::Persistent<v8::Function> callback;
        const unsigned argc = 2;
        callback.Reset(v8::Isolate::GetCurrent(), args[3].As<Function>());
        std::function<herr_t(hid_t did, unsigned int dim, hid_t dsid, void *visitor_data)> f=[&](hid_t did, unsigned int dim, hid_t dsid, void *visitor_data){
        v8::Local<v8::Value> argv[2] = { v8::Int32::New(v8::Isolate::GetCurrent(), dim),v8:: String::NewFromUtf8(v8::Isolate::GetCurrent(), "success") };
        v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
            return (herr_t)0;
        };
        v8::Local<v8::Function> func=v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback);
    herr_t err=H5DSiterate_scales( did, (unsigned int)rank, &idx, [&](hid_t did, unsigned int dim, hid_t dsid, void *visitor_data) -> herr_t {
        v8::Local<v8::Value> argv[2] = { v8::Int32::New(v8::Isolate::GetCurrent(), dim),v8:: String::NewFromUtf8(v8::Isolate::GetCurrent(), "success") };
        std::cout<<"iter "<<std::endl;
//        ((v8::Local<v8::Function>*)visitor_data)[0]->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
        return (herr_t)0;
    }, &func);
    if(err<0){
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed iterating through  scale indices")));
        args.GetReturnValue().SetUndefined();
        return;
        
    }
//        callback.Reset();
    H5Dclose(did);
        args.GetReturnValue().SetUndefined();
}

static void set_label (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 4 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsInt32() || !args[3]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name, index, label")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    std::string label(*String::Utf8Value(args[3]->ToString()));
    herr_t err=H5DSset_label(did, args[2]->ToInt32()->Value(), (char*)label.c_str());
    H5Dclose(did);
    args.GetReturnValue().SetUndefined();
}

static void get_label (const v8::FunctionCallbackInfo<Value>& args)
{

    // fail out if arguments are not correct
    if (args.Length() != 3 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name, index")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    size_t size=0;
    size=H5DSget_label(did, args[2]->ToInt32()->Value(), NULL, size);
    std::string name(size+1, '\0');
    size=H5DSget_label(did, args[2]->ToInt32()->Value(), (char*)name.c_str(), size+1);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name.c_str()));
    H5Dclose(did);
}

static void get_scale_name (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() != 2 || !args[0]->IsInt32() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    size_t size=0;
    size=H5DSget_scale_name(did, NULL, size);
    std::string name(size+1, '\0');
    size=H5DSget_scale_name(did, (char*)name.c_str(), size+1);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name.c_str()));
    H5Dclose(did);
}

static void get_num_scales (const v8::FunctionCallbackInfo<Value>& args)
{

    // fail out if arguments are not correct
    if (args.Length() != 3 || !args[0]->IsInt32() || !args[1]->IsString() || !args[2]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, index")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
    int num=H5DSget_num_scales(did, args[2]->ToInt32()->Value());
    args.GetReturnValue().Set(v8::Uint32::New(v8::Isolate::GetCurrent(), num));
    H5Dclose(did);
}

    };
}