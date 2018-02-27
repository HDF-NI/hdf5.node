#pragma once
#include <map>

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <cstring>
#include <memory>

namespace NodeHDF5 {

  class Uint64 : public node::ObjectWrap {
    friend class File;
    friend class Group;
    friend class Filters;
    friend class Attributes;
    friend class PacketTable;

  protected:
  public:
    static void Initialize(v8::Handle<v8::Object> target);
    static v8::Local<v8::Object> Instantiate(long long value);
    static v8::Local<v8::Object> Instantiate(v8::Local<v8::Object> parent, long long value = 0);
    long long Value() {
      return value;
    };

    static void toString(const v8::FunctionCallbackInfo<v8::Value>& args);

  protected:
    Uint64()
        : Uint64(0){

          };

    Uint64(long long l) {
      value = l;
    };

    ~Uint64(){};

  private:
    static v8::Persistent<v8::Function> Constructor;

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
      // Xdm value object
      Uint64* xp;
      if (args.Length() < 1)
        xp = new Uint64(-1);
      else {

        xp = new Uint64((long long)args[args.Length() - 1]->Int32Value());
      }

      xp->value = 0;
      // extend target object
      xp->Wrap(args.This());
    };

  protected:
    long long value = 0;
  };
}
