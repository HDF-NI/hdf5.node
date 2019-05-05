#include "uint64.hpp"

namespace NodeHDF5 {

  v8::Persistent<v8::Function> Uint64::Constructor;

  void Uint64::Initialize(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // instantiate constructor function template
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, New);
    t->SetClassName(v8::String::NewFromUtf8(isolate, "Uint64", v8::NewStringType::kInternalized).ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(t, "toString", toString);
    Constructor.Reset(isolate, t->GetFunction(context).ToLocalChecked());

    // append this function to the target object
    v8::Maybe<bool> ret=exports->Set(context, v8::String::NewFromUtf8(isolate, "Uint64", v8::NewStringType::kInternalized).ToLocalChecked(), t->GetFunction(context).ToLocalChecked());
    if(ret.ToChecked()){
      
    }
  };

  v8::Local<v8::Object> Uint64::Instantiate(unsigned long long value) {
    auto isolate = v8::Isolate::GetCurrent();

    v8::Local<v8::Value> argv[1] = {v8::Int32::New(v8::Isolate::GetCurrent(), value)

    };
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 1, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Uint64::Instantiate(std::string value) {
    auto isolate = v8::Isolate::GetCurrent();

    v8::Local<v8::Value> argv[1] = {v8::String::NewFromUtf8(isolate, value.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()

    };
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 1, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Uint64::Instantiate(v8::Local<v8::Object> parent, unsigned long long value) {
    auto isolate = v8::Isolate::GetCurrent();

    // group name and parent reference
    v8::Local<v8::Value> argv[2] = {parent, v8::Int32::New(isolate, value)};

    // return new group instance
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 2, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Uint64::Instantiate(v8::Local<v8::Object> parent, std::string value) {
    auto isolate = v8::Isolate::GetCurrent();

    // group name and parent reference
    v8::Local<v8::Value> argv[2] = {parent, v8::String::NewFromUtf8(isolate, value.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()};

    // return new group instance
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 2, argv).ToLocalChecked();
  }

  void Uint64::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // unwrap group
    Uint64* theId = ObjectWrap::Unwrap<Uint64>(args.This());

    args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), std::to_string(theId->Value()).c_str(), v8::NewStringType::kInternalized).ToLocalChecked());
  }
}
