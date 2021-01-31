#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <functional>
#include <memory>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "H5DSpublic.h"

namespace NodeHDF5 {

  class H5ds {
  public:
    static void Initialize(v8::Local<v8::Object> exports) {
      v8::Isolate* isolate = exports->GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // append this function to the target object
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "setScale", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::set_scale)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "attachScale", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::attach_scale)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "detachScale", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::detach_scale)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "isAttached", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::is_attached)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "isScale", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::is_scale)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "iterateScale", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::iterate_scales)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "setLabel", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::set_label)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "getLabel", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_label)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "getScaleName", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_scale_name)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "getNumberOfScales", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5ds::get_num_scales)->GetFunction(context).ToLocalChecked()).Check();
    }

    static void set_scale(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      
      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      String::Utf8Value dim_scale_name(isolate, args[2]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      herr_t            err    = H5DSset_scale(did, *dim_scale_name);
      if (err < 0) {
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed setting dimension scale", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Dclose(did);
      args.GetReturnValue().SetUndefined();
    }

    static void attach_scale(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      String::Utf8Value dim_scale_name(isolate, args[2]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t             dsid   = H5Dopen(idWrap->Value(), *dim_scale_name, H5P_DEFAULT);
      herr_t            err    = H5DSattach_scale(did, dsid, args[3]->Int32Value(context).ToChecked());
      if (err < 0) {
        H5Dclose(dsid);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed attaching dimension scale", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Dclose(dsid);
      H5Dclose(did);
      args.GetReturnValue().SetUndefined();
    }

    static void detach_scale(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      String::Utf8Value dim_scale_name(isolate, args[2]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t             dsid   = H5Dopen(idWrap->Value(), *dim_scale_name, H5P_DEFAULT);
      herr_t            err    = H5DSdetach_scale(did, dsid, args[3]->Int32Value(context).ToChecked());
      if (err < 0) {
        H5Dclose(dsid);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed attaching dimension scale", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Dclose(dsid);
      H5Dclose(did);
      args.GetReturnValue().SetUndefined();
    }

    static void is_attached(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, name, index", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      String::Utf8Value dim_scale_name(isolate, args[2]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t             dsid   = H5Dopen(idWrap->Value(), *dim_scale_name, H5P_DEFAULT);
      htri_t            status = H5DSis_attached(did, dsid, args[3]->Int32Value(context).ToChecked());
      args.GetReturnValue().Set((status) ? true : false);
      H5Dclose(dsid);
      H5Dclose(did);
    }

    static void is_scale(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      htri_t            status = H5DSis_scale(did);
      args.GetReturnValue().Set((status) ? true : false);
      H5Dclose(did);
    }

    static void iterate_scales(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      int               rank;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      hid_t                        did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      int                          idx = args[2]->Int32Value(context).ToChecked();
      v8::Persistent<v8::Function> callback;
      const unsigned               argc = 2;
      callback.Reset(v8::Isolate::GetCurrent(), args[3].As<Function>());
      std::function<herr_t(hid_t did, unsigned int dim, hid_t dsid, void* visitor_data)> f =
          [&](hid_t did, unsigned int dim, hid_t dsid, void* visitor_data) {
            v8::Local<v8::Value> argv[2] = {v8::Int32::New(isolate, dim),
                                            v8::String::NewFromUtf8(isolate, "success", v8::NewStringType::kInternalized).ToLocalChecked()};
            v8::MaybeLocal<v8::Value> ret = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
                ->Call(context, v8::Null(isolate), argc, argv);
            if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){

            }
            return (herr_t)0;
          };
      v8::Local<v8::Function> func = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback);
      herr_t                  err  = H5DSiterate_scales(did,
                                      (unsigned int)rank,
                                      &idx,
                                      [](hid_t did, unsigned int dim, hid_t dsid, void* visitor_data) -> herr_t {
                                        v8::Local<v8::Value> argv[2] = {v8::Int32::New(v8::Isolate::GetCurrent(), dim),
                                                                        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "success", v8::NewStringType::kInternalized).ToLocalChecked()};
                                        v8::MaybeLocal<v8::Value> ret = ((v8::Local<v8::Function>*)visitor_data)[0]->Call(
                                            v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(v8::Isolate::GetCurrent()), 3, argv);
                                        if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){
                                            
                                        }
                                        return (herr_t)0;
                                      },
                                      &func);
      if (err < 0) {
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed iterating through  scale indices", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      H5Dclose(did);
      args.GetReturnValue().SetUndefined();
    }

    static void set_label(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsInt32() || !args[3]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name, index, label", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      std::string       label(*String::Utf8Value(isolate, args[3]->ToString(context).ToLocalChecked()));
      H5DSset_label(did, args[2]->Int32Value(context).ToChecked(), (char*)label.c_str());
      H5Dclose(did);
      args.GetReturnValue().SetUndefined();
    }

    static void get_label(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name, index", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      size_t            size   = 0;
      size                     = H5DSget_label(did, args[2]->Int32Value(context).ToChecked(), NULL, size);
      std::string name(size + 1, '\0');
      size = H5DSget_label(did, args[2]->Int32Value(context).ToChecked(), (char*)name.c_str(), size + 1);
      args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name.c_str(), v8::NewStringType::kNormal).ToLocalChecked());
      H5Dclose(did);
    }

    static void get_scale_name(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, scale name", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      size_t            size   = 0;
      size                     = H5DSget_scale_name(did, NULL, size);
      std::string name(size + 1, '\0');
      size = H5DSget_scale_name(did, (char*)name.c_str(), size + 1);
      args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name.c_str(), v8::NewStringType::kNormal).ToLocalChecked());
      H5Dclose(did);
    }

    static void get_num_scales(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsInt32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group id, dataset name, index", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      int               num    = H5DSget_num_scales(did, args[2]->Int32Value(context).ToChecked());
      args.GetReturnValue().Set(v8::Uint32::New(v8::Isolate::GetCurrent(), num));
      H5Dclose(did);
    }
  };
}