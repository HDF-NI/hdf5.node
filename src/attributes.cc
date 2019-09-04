#include <nan.h>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "reference.hpp"
#include "attributes.hpp"

namespace NodeHDF5 {

  void Attributes::make_attribute_from_typed_array(const hid_t&               group_id,
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

  void Attributes::make_attribute_from_array(const hid_t& group_id, const char* attribute_name, v8::Local<v8::Array> array) {
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
        Int64* valueWrap = ObjectWrap::Unwrap<Int64>(array->Get(arrayIndex)->ToObject());
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
        Uint64* valueWrap = ObjectWrap::Unwrap<Uint64>(array->Get(arrayIndex)->ToObject());
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

  void Attributes::Refresh(const v8::FunctionCallbackInfo<v8::Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() > 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Attributes*              group = ObjectWrap::Unwrap<Attributes>(args.This());
    try{
        v8::Local<v8::Object> focus=args.This();
      refreshAttributes(focus, group->id);
    } catch (Exception& ex) {
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
      args.GetReturnValue().SetUndefined();
      return;
    } catch (std::exception& ex) {
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
      args.GetReturnValue().SetUndefined();
      return;
    }

    return;
  };

  void Attributes::Flush(const v8::FunctionCallbackInfo<v8::Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() > 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Attributes* group = Unwrap<Attributes>(args.This());

    v8::Local<v8::Array> propertyNames = args.This()->GetPropertyNames();
    for (unsigned int index = 0; index < propertyNames->Length(); index++) {
      v8::Local<v8::Value> name = propertyNames->Get(index);
      if (!args.This()->Get(name)->IsFunction() && strncmp("id", (*Nan::Utf8String(name->ToString())), 2) != 0) {
        htri_t attrExists = H5Aexists(group->id, *Nan::Utf8String(name->ToString()));
        if (args.This()->Get(name)->IsFloat64Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_DOUBLE);
        } else if (args.This()->Get(name)->IsFloat32Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_FLOAT);
        } else if (args.This()->Get(name)->IsInt32Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_INT);
        } else if (args.This()->Get(name)->IsUint32Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_UINT);
        } else if (args.This()->Get(name)->IsInt16Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_SHORT);
        } else if (args.This()->Get(name)->IsUint16Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_USHORT);
        } else if (args.This()->Get(name)->IsInt8Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_INT8);
        } else if (args.This()->Get(name)->IsUint8Array()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_typed_array(group->id,
                                          *Nan::Utf8String(name->ToString()),
                                          v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)),
                                          H5T_NATIVE_UINT8);
        } else if (args.This()->Get(name)->IsUint32()) {
          uint32_t value = args.This()->Get(name)->Uint32Value();
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type  = H5Tcopy(H5T_NATIVE_UINT);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, &value);
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsInt32()) {
          int32_t value = args.This()->Get(name)->Int32Value();
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type  = H5Tcopy(H5T_NATIVE_INT);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, &value);
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsNumber()) {
          double value = args.This()->Get(name)->NumberValue();

          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type  = H5Tcopy(H5T_NATIVE_DOUBLE);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, &value);
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsStringObject()) {
          std::string value((*Nan::Utf8String(v8::StringObject::Cast(*args.This()->Get(name))->ValueOf())));
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type = H5Tcopy(H5T_C_S1);
          H5Tset_size(attr_type, H5T_VARIABLE);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          std::unique_ptr<char* []> vl(new char*[1]);
          vl.get()[0] = new char[value.length() + 1];
          std::strncpy(vl.get()[0], value.c_str(), value.length() + 1);
          H5Awrite(attr_id, attr_type, vl.get());
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsString()) {
          std::string value((*Nan::Utf8String(args.This()->Get(name)->ToString())));
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t  attr_type = H5Tcopy(H5T_C_S1);
          size_t s         = std::strlen(value.c_str());
          if (s) {
            H5Tset_size(attr_type, s);
          }
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, value.c_str());
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsArray()) {
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          make_attribute_from_array(
              group->id, *Nan::Utf8String(name->ToString()), v8::Local<v8::Array>::Cast(args.This()->Get(name)));
        } else if (args.This()->Get(name)->IsObject() && std::strncmp("Int64", (*Nan::Utf8String(args.This()->Get(name)->ToObject()->GetConstructorName())), 5)==0) {
          Int64* valueWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(name)->ToObject());
          int64_t value = valueWrap->Value();
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type  = H5Tcopy(H5T_NATIVE_INT64);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, &value);
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        } else if (args.This()->Get(name)->IsObject() && std::strncmp("Uint64", (*Nan::Utf8String(args.This()->Get(name)->ToObject()->GetConstructorName())), 6)==0) {
          Uint64* valueWrap = ObjectWrap::Unwrap<Uint64>(args.This()->Get(name)->ToObject());
          uint64_t value = valueWrap->Value();
          if (attrExists) {
            H5Adelete(group->id, *Nan::Utf8String(name->ToString()));
          }
          hid_t attr_type  = H5Tcopy(H5T_NATIVE_UINT64);
          hid_t attr_space = H5Screate(H5S_SCALAR);
          hid_t attr_id = H5Acreate(group->id, *Nan::Utf8String(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
          if (attr_id < 0) {
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Awrite(attr_id, attr_type, &value);
          H5Sclose(attr_space);
          H5Tclose(attr_type);
          H5Aclose(attr_id);

        }
        
      }
    }

    return;
  };
  
    //void refreshAttributes(v8::Local<v8::Object>& focus, hid_t id)
    
}
