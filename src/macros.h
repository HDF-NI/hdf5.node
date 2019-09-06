#pragma once


#define THROW_EXCEPTION(message) \
  v8::Isolate::GetCurrent()->ThrowException( \
    v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), message)))

#define OBJECT_HAS(object, key) Nan::HasOwnProperty(object, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), key)).ToChecked()
#define OBJECT_GET(object, key) Nan::Get(object, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), key)).ToLocalChecked()
