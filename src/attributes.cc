#include <nan.h>

#include "macros.h"
#include "hdf5.h"
#include "hdf5_hl.h"

#include "reference.hpp"
#include "attributes.hpp"

using v8::Object;
using v8::String;
using v8::Local;
using v8::Value;
using Nan::MaybeLocal;

namespace NodeHDF5 {

  static void make_attribute_from_typed_array(const hid_t&               group_id,
                                              const char*                attribute_name,
                                              v8::Local<v8::TypedArray> buffer,
                                              hid_t                      type_id) {
    std::unique_ptr<hsize_t[]> currentDims(new hsize_t[1]);
    currentDims.get()[0] = buffer->Length();
    hid_t attr_space     = H5Screate_simple(1, currentDims.get(), NULL);
    if (attr_space >= 0) {
      hid_t attr_type = H5Tcopy(type_id);
      hid_t attr_id   = H5Acreate(group_id, attribute_name, attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);

#if NODE_VERSION_AT_LEAST(8,0,0)
      H5Awrite(attr_id, attr_type, node::Buffer::Data(buffer->ToObject()));
#else
      H5Awrite(attr_id, attr_type, buffer->Buffer()->Externalize().Data());
#endif
      H5Aclose(attr_id);
      H5Tclose(attr_type);
      H5Sclose(attr_space);
    }
  }

  static void make_attribute_from_array(const hid_t& group_id, const char* attribute_name, v8::Local<v8::Array> array) {
    int                        rank = 1;
    std::unique_ptr<hsize_t[]> countSpace(new hsize_t[rank]);
    countSpace.get()[0] = array->Length();
    std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
    count.get()[0]    = array->Length();
    hid_t memspace_id = H5Screate_simple(rank, countSpace.get(), NULL);
    if(array->Length()>0 && array->Get(0)->IsObject() && std::strncmp("Int64", (*Nan::Utf8String(array->Get(0)->ToObject()->GetConstructorName())), 5)==0){
      hid_t type_id     =  H5Tcopy(H5T_NATIVE_INT64);
//      H5Tset_size(type_id, array->Length());
//      hid_t                     arraytype_id = H5Tarray_create(type_id, rank, count.get());
      hid_t                     attr_id      = H5Acreate(group_id, attribute_name, type_id, memspace_id, H5P_DEFAULT, H5P_DEFAULT);
      std::unique_ptr<long long []> vl(new long long[array->Length()]);
      for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
        Int64* valueWrap = node::ObjectWrap::Unwrap<Int64>(array->Get(arrayIndex)->ToObject());
        int64_t value = valueWrap->Value();
        vl.get()[arrayIndex] = value;
      }

      herr_t err = H5Awrite(attr_id, type_id, vl.get());

      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make long long array attribute")));
      }

//      H5Tclose(arraytype_id);
      H5Tclose(type_id);
      H5Aclose(attr_id);

    }
    else if(array->Length()>0 && array->Get(0)->IsObject() && std::strncmp("Uint64", (*Nan::Utf8String(array->Get(0)->ToObject()->GetConstructorName())), 5)==0){
      hid_t type_id     =  H5Tcopy(H5T_NATIVE_UINT64);
//      H5Tset_size(type_id, array->Length());
//      hid_t                     arraytype_id = H5Tarray_create(type_id, rank, count.get());
      hid_t                     attr_id      = H5Acreate(group_id, attribute_name, type_id, memspace_id, H5P_DEFAULT, H5P_DEFAULT);
      std::unique_ptr<unsigned long long []> vl(new unsigned long long[array->Length()]);
      for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
        Uint64* valueWrap = node::ObjectWrap::Unwrap<Uint64>(array->Get(arrayIndex)->ToObject());
        uint64_t value = valueWrap->Value();
        vl.get()[arrayIndex] = value;
      }

      herr_t err = H5Awrite(attr_id, type_id, vl.get());

      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make long long array attribute")));
      }

//      H5Tclose(arraytype_id);
      H5Tclose(type_id);
      H5Aclose(attr_id);

    }
    else{
    hid_t type_id     = H5Tcopy(H5T_C_S1);
    H5Tset_size(type_id, H5T_VARIABLE);
//    hid_t                     arraytype_id = H5Tarray_create(type_id, rank, count.get());
    hid_t                     attr_id      = H5Acreate(group_id, attribute_name, type_id, memspace_id, H5P_DEFAULT, H5P_DEFAULT);
    std::unique_ptr<char* []> vl(new char*[array->Length()]);
    for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
      Nan::Utf8String buffer(array->Get(arrayIndex)->ToString());
      std::string           s(*buffer);
      vl.get()[arrayIndex] = new char[s.length()+1];
      std::memset(vl.get()[arrayIndex], 0, s.length()+1);
      std::strncpy(vl.get()[arrayIndex], s.c_str(), s.length());
    }

    herr_t err = H5Awrite(attr_id, type_id, vl.get());

    if (err < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len array attribute")));
    }

//    H5Tclose(arraytype_id);
    H5Tclose(type_id);
    H5Aclose(attr_id);
    }
    H5Sclose(memspace_id);
  }

  static void set_attribute(hid_t groupId, Local<String> name, Local<Value> value, MaybeLocal<Object> maybeOptions) {
    if (value->IsFunction() || strncmp("id", (*Nan::Utf8String(name)), 2) == 0) {
      return;
    }

    if (H5Aexists(groupId, *Nan::Utf8String(name))) {
      H5Adelete(groupId, *Nan::Utf8String(name));
    }

    if (value->IsFloat64Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_DOUBLE);
    } else if (value->IsFloat32Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_FLOAT);
    } else if (value->IsInt32Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_INT);
    } else if (value->IsUint32Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_UINT);
    } else if (value->IsInt16Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_SHORT);
    } else if (value->IsUint16Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_USHORT);
    } else if (value->IsInt8Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_INT8);
    } else if (value->IsUint8Array()) {
      make_attribute_from_typed_array(groupId,
                                      *Nan::Utf8String(name),
                                      v8::Local<v8::Float64Array>::Cast(value),
                                      H5T_NATIVE_UINT8);
    } else if (value->IsUint32()) {
      uint32_t uint32Value = value->Uint32Value();

      hid_t attr_type  = H5Tcopy(H5T_NATIVE_UINT);
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, &uint32Value);
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);

    } else if (value->IsInt32()) {
      int32_t int32Value = value->Int32Value();

      hid_t attr_type  = H5Tcopy(H5T_NATIVE_INT);
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, &int32Value);
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);

    } else if (value->IsNumber()) {
      double doubleValue = value->NumberValue();

      hid_t attr_type  = H5Tcopy(H5T_NATIVE_DOUBLE);
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, &doubleValue);
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);

    } else if (value->IsString() || value->IsStringObject()) {
      std::string stringValue;

      if (value->IsString())
        stringValue = std::string(*Nan::Utf8String(value->ToString()));
      else
        stringValue = std::string(*Nan::Utf8String(v8::StringObject::Cast(*value)->ValueOf()));

      hid_t  attr_type = H5Tcopy(H5T_C_S1);
      size_t s         = stringValue.length();
      if (s) {
        H5Tset_size(attr_type, s);
      }
      if (!maybeOptions.IsEmpty()) {
        Local<Object> options = maybeOptions.ToLocalChecked();
        if (OBJECT_HAS(options, "padding")) {
          H5Tset_strpad(attr_type, (H5T_str_t) Nan::To<uint32_t>(OBJECT_GET(options, "padding")).ToChecked());
        }
      }
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, stringValue.c_str());
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);

    } else if (value->IsArray()) {
      make_attribute_from_array(
          groupId, *Nan::Utf8String(name), v8::Local<v8::Array>::Cast(value));

    } else if (value->IsObject() && std::strncmp("Int64", (*Nan::Utf8String(value->ToObject()->GetConstructorName())), 5)==0) {
      Int64* valueWrap = node::ObjectWrap::Unwrap<Int64>(value->ToObject());
      int64_t int64Value = valueWrap->Value();

      hid_t attr_type  = H5Tcopy(H5T_NATIVE_INT64);
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, &int64Value);
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);

    } else if (value->IsObject() && std::strncmp("Uint64", (*Nan::Utf8String(value->ToObject()->GetConstructorName())), 6)==0) {
      Uint64* valueWrap = node::ObjectWrap::Unwrap<Uint64>(value->ToObject());
      uint64_t uint64Value = valueWrap->Value();

      hid_t attr_type  = H5Tcopy(H5T_NATIVE_UINT64);
      hid_t attr_space = H5Screate(H5S_SCALAR);
      hid_t attr_id = H5Acreate(groupId, *Nan::Utf8String(name), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        H5Sclose(attr_space);
        H5Tclose(attr_type);
        THROW_EXCEPTION("failed creating attribute");
        return;
      }
      H5Awrite(attr_id, attr_type, &uint64Value);
      H5Sclose(attr_space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);
    }
  };

  void Attributes::Refresh(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // Fail out if arguments are not correct
    if (args.Length() > 0) {
      THROW_EXCEPTION("expected arguments");
      return;
    }

    // unwrap group
    Attributes*              group = ObjectWrap::Unwrap<Attributes>(args.This());

    try{
      Local<Object> focus = args.This();
      refreshAttributes(focus, group->id);
    } catch (Exception& ex) {
      THROW_EXCEPTION(ex.what());
      return;
    } catch (std::exception& ex) {
      THROW_EXCEPTION(ex.what());
      return;
    }

    return;
  };

  void Attributes::Flush(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // Fail out if arguments are not correct
    if (args.Length() > 0) {
      THROW_EXCEPTION("expected arguments");
      return;
    }

    Attributes* group = Unwrap<Attributes>(args.This());

    Local<v8::Array> propertyNames = args.This()->GetPropertyNames();
    for (uint32_t index = 0; index < propertyNames->Length(); index++) {
      Local<String> name = Nan::To<String>(propertyNames->Get(index)).ToLocalChecked();

      set_attribute(group->id, name, args.This()->Get(name), MaybeLocal<Object>());
    }

    return;
  };

  void Attributes::SetAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {

    if (args.Length() < 2) {
      THROW_EXCEPTION("Missing arguments");
      return;
    }

    // unwrap group
    Attributes* group = Unwrap<Attributes>(args.This());

    auto name = args[0]->ToString();
    auto value = args[1];
    auto options = args.Length() == 3 ? MaybeLocal<Object>(args[2]->ToObject()) : MaybeLocal<Object>();

    set_attribute(group->id, name, value, options);
  };
}
