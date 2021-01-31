#pragma once
#include <map>

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <memory>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "exceptions.hpp"
#include "reference.hpp"
#include "attributes.hpp"
#include "methods.hpp"

namespace NodeHDF5 {

  using namespace v8;
  using namespace node;

  class File : public Methods {
    using Attributes::name;
    using Attributes::id;
    using Attributes::gcpl_id;
    using Attributes::Refresh;
    using Attributes::Flush;

  protected:
    hid_t plist_id, gcpl, dtpl_id, dapl_id, dcpl_id;

    unsigned int compression = 0;
    bool         error       = false;

  public:
    static void Initialize(v8::Local<v8::Object> target);

    hid_t getId() {
      return id;
    };

    hid_t getGcpl() {
      return gcpl;
    };

    std::string getFileName() {
      return name;
    };

  private:
    File(const char* path);
    File(const char* path, unsigned int access);
    ~File();

    static Persistent<FunctionTemplate> Constructor;
    static void New(const v8::FunctionCallbackInfo<Value>& args);
    static void EnableSingleWriteMultiRead(const v8::FunctionCallbackInfo<Value>& args);
    static void CreateGroup(const v8::FunctionCallbackInfo<Value>& args);
    static void OpenGroup(const v8::FunctionCallbackInfo<Value>& args);
    static void Move(const v8::FunctionCallbackInfo<Value>& args);
    static void Delete(const v8::FunctionCallbackInfo<Value>& args);
    static void Close(const v8::FunctionCallbackInfo<Value>& args);
  };
};