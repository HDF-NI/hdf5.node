#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <cstring>
#include <vector>
#include <functional>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "uint64.hpp"
#include "H5Zpublic.h"

namespace NodeHDF5 {

  class Filters : public node::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports) {
      v8::Isolate* isolate = exports->GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // Prepare constructor template
      v8::Local<v8::FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Filters", v8::NewStringType::kInternalized).ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Prototype
      setPrototypeMethod(isolate, tpl, v8::String::NewFromUtf8(isolate, "isAvailable", v8::NewStringType::kInternalized).ToLocalChecked(), isAvailable);
      setPrototypeMethod(isolate, tpl, v8::String::NewFromUtf8(isolate, "getNFilters", v8::NewStringType::kInternalized).ToLocalChecked(), getNFilters);
      setPrototypeMethod(isolate, tpl, v8::String::NewFromUtf8(isolate, "getFilter", v8::NewStringType::kInternalized).ToLocalChecked(), getFilter);

      Constructor.Reset(v8::Isolate::GetCurrent(), tpl->GetFunction(context).ToLocalChecked());
      v8::Maybe<bool> ret = exports->Set(context, v8::String::NewFromUtf8(isolate, "Filters", v8::NewStringType::kInternalized).ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked());
      if(ret.ToChecked()){
        
      }
    }
  
    static Local<Object> Instantiate(hid_t parentId, std::string dset_name) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      Local<Object> idInstance         = Int64::Instantiate(parentId);
      Int64*        idWrap             = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = parentId;
      const unsigned        argc       = 2;
      v8::Local<v8::Value> argv[argc] = {idInstance, v8::String::NewFromUtf8(isolate, dset_name.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()};

      return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
    }

  private:
    explicit Filters(hid_t parentId, std::string datasetName)
        : parentId(parentId)
        , datasetName(datasetName){

          };

    ~Filters(){};

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);
      // Invoked as constructor: `new MyObject(...)`
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t  value  = args[0]->IsUndefined() ? -1 : idWrap->Value();

      std::string name = args[1]->IsUndefined() ? std::string("") : std::string(*v8::String::Utf8Value(isolate, args[1]->ToString(context).ToLocalChecked()));
      Filters*    obj  = new Filters(value, name);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }

    static void isAvailable(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);

      args.GetReturnValue().Set((H5Zfilter_avail(args[0]->Int32Value(context).ToChecked())) ? true : false);
    }

    static void getNFilters(const v8::FunctionCallbackInfo<v8::Value>& args) {
      Filters* obj = ObjectWrap::Unwrap<Filters>(args.This());
      hid_t    did = H5Dopen(obj->parentId, obj->datasetName.c_str(), H5P_DEFAULT);
      hid_t    pl  = H5Dget_create_plist(did);
      int      n   = H5Pget_nfilters(pl);
      H5Pclose(pl);
      H5Dclose(did);
      args.GetReturnValue().Set(Int32::New(v8::Isolate::GetCurrent(), n));
    }

    static void getFilter(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static v8::Persistent<v8::Function> Constructor;

  protected:
    hid_t       parentId;
    std::string datasetName;
  };
}
