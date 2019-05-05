#pragma once

#include <v8.h>

namespace NodeHDF5 {

  inline void setMethod(v8::Isolate* isolate, v8::Local<v8::Template> recv, v8::Local<v8::String> name, v8::FunctionCallback callback){
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback);
    t->SetClassName(name);
    recv->Set(name, t);
  };
  
  inline void setMethod(v8::Isolate* isolate, v8::Local<v8::Object> recv, v8::Local<v8::String> name, v8::FunctionCallback callback){
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Function> f = v8::FunctionTemplate::New(isolate, callback)->GetFunction(context).ToLocalChecked();
    f->SetName(name);
    v8::Maybe<bool> ret = recv->Set(context, name, f);
    if(ret.ToChecked()){};
  };
  
  inline void setPrototypeMethod(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> recv, v8::Local<v8::String> name, v8::FunctionCallback callback){
    //v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Signature> s = v8::Signature::New(isolate, recv);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, v8::Local<v8::Value>(), s);
    t->SetClassName(name);
    recv->PrototypeTemplate()->Set(name, t);
  };
  
}
