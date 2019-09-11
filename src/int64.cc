#include "int64.hpp"

namespace NodeHDF5 {

  v8::Persistent<v8::Function> Int64::Constructor;

  void Int64::Initialize(v8::Local<v8::Object> target) {
    // instantiate constructor function template
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
    t->SetClassName(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Int64"));
    t->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(t, "toString", toString);
    Constructor.Reset(v8::Isolate::GetCurrent(), t->GetFunction());

    // append this function to the target object
    target->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Int64"), t->GetFunction());
  };

  v8::Local<v8::Object> Int64::Instantiate(long long value) {
    auto isolate = v8::Isolate::GetCurrent();

    v8::Local<v8::Value> argv[1] = {v8::Int32::New(v8::Isolate::GetCurrent(), value)

    };
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 1, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Int64::Instantiate(std::string value) {
    auto isolate = v8::Isolate::GetCurrent();

    v8::Local<v8::Value> argv[1] = {v8::String::NewFromUtf8(isolate, value.c_str())

    };
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 1, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Int64::Instantiate(v8::Local<v8::Object> parent, long long value) {
    auto isolate = v8::Isolate::GetCurrent();

    // group name and parent reference
    v8::Local<v8::Value> argv[2] = {parent, v8::Int32::New(isolate, value)};

    // return new group instance
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 2, argv).ToLocalChecked();
  }

  v8::Local<v8::Object> Int64::Instantiate(v8::Local<v8::Object> parent, std::string value) {
    auto isolate = v8::Isolate::GetCurrent();

    // group name and parent reference
    v8::Local<v8::Value> argv[2] = {parent, v8::String::NewFromUtf8(isolate, value.c_str())};

    // return new group instance
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 2, argv).ToLocalChecked();
  }

  void Int64::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // unwrap group
    Int64* theId = ObjectWrap::Unwrap<Int64>(args.This());

    args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), std::to_string(theId->Value()).c_str()));
  }
}
