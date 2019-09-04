#include <node.h>
#include "file.h"
#include "group.h"
#include "filters.hpp"
#include "macros.h"
#include "reference.hpp"
#include "hdf5node.hpp"

using namespace v8;
using namespace NodeHDF5;

extern "C" {

static void init(Handle<Object> target) {

  // create local scope
  HandleScope scope(v8::Isolate::GetCurrent());

  NODE_SET_METHOD(target, "getLibVersion", getLibVersion);
  NODE_SET_METHOD(target, "isHDF5", isHDF5);

  // initialize wrapped objects
  File::Initialize(target);
  Filters::Init(target);
  Reference::Init(target);
  Int64::Initialize(target);
  Uint64::Initialize(target);

  // initialize wrapped object factories
  Group::Initialize();
}

NODE_MODULE(hdf5, init)
}

namespace NodeHDF5 {

  void getLibVersion(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 0) {

      THROW_EXCEPTION("expected empty");
      args.GetReturnValue().SetUndefined();
      return;
    }

    unsigned majnum;
    unsigned minnum;
    unsigned relnum;
    herr_t   err = H5get_libversion(&majnum, &minnum, &relnum);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(
        v8::Isolate::GetCurrent(), (std::to_string(majnum) + "." + std::to_string(minnum) + "." + std::to_string(relnum)).c_str()));
    if (err < 0) {
      THROW_EXCEPTION("failed to get lib version");
      args.GetReturnValue().SetUndefined();
      return;
    }
  }
  
  void isHDF5(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString() ) {

      THROW_EXCEPTION("expected string");
      args.GetReturnValue().SetUndefined();
      return;
    }
      String::Utf8Value path(args[0]->ToString());

    htri_t ret_value = H5Fis_hdf5( *path );
   if( ret_value > 0 ){
     args.GetReturnValue().Set(v8::Boolean::New(
        v8::Isolate::GetCurrent(), true));
   }else if( ret_value == 0 ){
     args.GetReturnValue().Set(v8::Boolean::New(
        v8::Isolate::GetCurrent(), false));
       
   } else // Raise exception when H5Fis_hdf5 returns a negative value
   {
      THROW_EXCEPTION("failed to check");
      args.GetReturnValue().SetUndefined();
   }
  }
  
}
