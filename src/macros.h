#pragma once


#define THROW_EXCEPTION(message) \
  v8::Isolate::GetCurrent()->ThrowException( \
    v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), message)))
