
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
    static void Init(v8::Local<v8::Object> exports) {
      v8::Isolate* isolate = exports->GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // Prepare constructor template
      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Reference", v8::NewStringType::kInternalized).ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Prototype
      NODE_SET_PROTOTYPE_METHOD(tpl, "dereference", dereference);
      NODE_SET_PROTOTYPE_METHOD(tpl, "getRegion", getRegion);
      NODE_SET_PROTOTYPE_METHOD(tpl, "getName", getName);

      Constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
      v8::Maybe<bool> ret=exports->Set(context, v8::String::NewFromUtf8(isolate, "Reference", v8::NewStringType::kInternalized).ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked());
      if(ret.ToChecked()){
      
      }
    }

    static v8::Local<v8::Object> Instantiate(hid_t objectId, int nmembers) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      v8::Local<v8::Object> idInstance         = Int64::Instantiate(objectId);
      Int64*        idWrap             = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = objectId;
      const unsigned        argc       = 2;
      v8::Local<v8::Value> argv[argc] = {idInstance, v8::Uint32::New(isolate, nmembers)};
      return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
    }

    hid_t getObjectId(){return objectId;};
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
      if(args.Length() == 3 && args[1]->IsString()){
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        hid_t  locId  = args[0]->IsUndefined() ? -1 : idWrap->Value();
        v8::String::Utf8Value name(isolate, args[1]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
        size_t size=sizeof(hid_t);
        std::unique_ptr<char[]> ref(new char[size]);
        std::memset(ref.get(), 0, size);
        herr_t err=H5Rcreate( (void *)ref.get(), locId, *name,(H5R_type_t)args[2]->Int32Value(isolate->GetCurrentContext()).ToChecked(), -1 );
        if(err<0){
            return;
        }
        hid_t objectId=((hid_t*)ref.get())[0];
        int          nmembers = 1;
        Reference* obj      = new Reference(objectId, nmembers);
        obj->Wrap(args.This());
      }
      else{
        // Invoked as constructor: `new MyObject(...)`
        hid_t        value    = args[0]->IsUndefined() ? -1 : ObjectWrap::Unwrap<Int64>(args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked())->Value();
        int          nmembers = args[1]->IsUndefined() ? -1 : args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked();
        Reference* obj      = new Reference(value, nmembers);
        obj->Wrap(args.This());
      }
      args.GetReturnValue().Set(args.This());
    }

    static void dereference(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      Reference* obj = ObjectWrap::Unwrap<Reference>(args.This());
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
      hid_t  locId  = args[0]->IsUndefined() ? -1 : idWrap->Value();
      
#if  H5_VERSION_GE(1,10,0)
      //H5Rdereference(locId, H5P_DEFAULT, (H5R_type_t)args[1]->Int32Value(), (void *)obj->objectId);
      H5Rdereference2(locId, H5P_DEFAULT, (H5R_type_t)args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked(), (void *)obj->objectId);
#else
      H5Rdereference(locId, (H5R_type_t)args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked(), (void *)(intptr_t)obj->objectId);
#endif
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
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
      hid_t  locId  = args[0]->IsUndefined() ? -1 : idWrap->Value();
      
      ssize_t size=0;
      size=H5Rget_name(locId, (H5R_type_t)args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked(), (void *)&obj->objectId, NULL, size)+1;
      std::unique_ptr<char[]> name(new char[size+1]);
       size=H5Rget_name(locId, (H5R_type_t)args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked(), (void *)&obj->objectId, name.get(), size);
      args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (const char*)name.get(), v8::NewStringType::kNormal, size).ToLocalChecked());
    }

    static v8::Persistent<v8::Function> Constructor;

  protected:
    hid_t                     objectId;
    unsigned int              nmembers;
    std::unique_ptr<char* []> p_data;
  };

}
