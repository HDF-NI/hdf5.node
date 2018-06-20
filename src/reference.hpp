
#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <cstring>
#include <vector>
#include <functional>
#include <memory>

//#include "file.h"
//#include "group.h"
#include "int64.hpp"
#include "H5Rpublic.h"
#include "H5Ppublic.h"

namespace NodeHDF5 {

  class Reference : public node::ObjectWrap {

  public:
    static void Init(v8::Handle<v8::Object> exports) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      // Prepare constructor template
      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Reference"));
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Prototype
      NODE_SET_PROTOTYPE_METHOD(tpl, "dereference", dereference);
      NODE_SET_PROTOTYPE_METHOD(tpl, "getRegion", getRegion);
      NODE_SET_PROTOTYPE_METHOD(tpl, "getName", getName);

      Constructor.Reset(v8::Isolate::GetCurrent(), tpl->GetFunction());
      exports->Set(v8::String::NewFromUtf8(isolate, "Reference"), tpl->GetFunction());
    }

    static v8::Local<v8::Object> Instantiate(hid_t objectId, int nmembers) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      v8::Local<v8::Object> idInstance         = Int64::Instantiate(objectId);
      Int64*        idWrap             = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = objectId;
      const unsigned        argc       = 2;
      v8::Handle<v8::Value> argv[argc] = {idInstance, v8::Uint32::New(isolate, nmembers)};
      return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
    }

  private:
    explicit Reference(hid_t objectId, unsigned int nmembers)
        : objectId(objectId)
        , nmembers(nmembers)
        , p_data(new char*[nmembers]){

          };

    ~Reference(){};

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      // Invoked as constructor: `new MyObject(...)`
      hid_t        value    = args[0]->IsUndefined() ? -1 : ObjectWrap::Unwrap<Int64>(args[0]->ToObject())->Value();
      int          nmembers = args[1]->IsUndefined() ? -1 : args[1]->Int32Value();
      Reference* obj      = new Reference(value, nmembers);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }

    static void dereference(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      Reference* obj = ObjectWrap::Unwrap<Reference>(args.This());
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      hid_t  locId  = args[0]->IsUndefined() ? -1 : idWrap->Value();
      
      H5Rdereference(locId, H5P_DEFAULT, (H5R_type_t)args[1]->Int32Value(), (void *)obj->objectId);

      args.GetReturnValue().SetUndefined();
    }
    
    static void getRegion(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      //Reference* obj = ObjectWrap::Unwrap<Reference>(args.This());

      args.GetReturnValue().Set(true);
    }

    static void getName(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      Reference* obj = ObjectWrap::Unwrap<Reference>(args.Holder());
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      hid_t  locId  = args[0]->IsUndefined() ? -1 : idWrap->Value();
      
      ssize_t size=0;
      size=H5Rget_name(locId, (H5R_type_t)args[1]->Int32Value(), (void *)&obj->objectId, NULL, size)+1;
      std::unique_ptr<char[]> name(new char[size+1]);
      std::memset(name.get(), 0, size+1);
      size=H5Rget_name(locId, (H5R_type_t)args[1]->Int32Value(), (void *)&obj->objectId, name.get(), size);
      args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (const char*)name.get(), v8::String::kNormalString, size));
    }

    static v8::Persistent<v8::Function> Constructor;

  protected:
    hid_t                     objectId;
    unsigned int              nmembers;
    std::unique_ptr<char* []> p_data;
  };

}