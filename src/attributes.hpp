#pragma once

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>
#include <string>
#include <cstring>
#include <vector>
#include <memory>

#include "H5Tpublic.h"
#include "int64.hpp"
#include "uint64.hpp"
#include "exceptions.hpp"

namespace NodeHDF5 {
    
inline  v8::Local<v8::Value>   readAttributeByName(v8::Local<v8::Object> focus, hid_t id, std::string name){
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Value> value;
      bool indexedArray=true;
      hid_t attr_id   = H5Aopen(id, name.c_str(), H5P_DEFAULT);
      hid_t attr_type = H5Aget_type(attr_id);
      hid_t space     = H5Aget_space(attr_id);
      hssize_t num_elements = H5Sget_simple_extent_npoints(space);
      switch (H5Sget_simple_extent_type(space)) {
        case H5S_SIMPLE: {
          hssize_t    numberOfElements = H5Sget_simple_extent_npoints(space);
          H5T_class_t class_id         = H5Tget_class(attr_type);
          switch (class_id) {
        case H5T_REFERENCE:{
            std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
            H5Aread(attr_id, attr_type, buf.get());
            hid_t objectId=((hid_t*)buf.get())[0];
            v8::Local<v8::Object>&& ref = Reference::Instantiate(objectId, 1);
            value = ref;
          }
            break;
            case H5T_ARRAY: {
              hid_t basetype_id = H5Tget_super(attr_type);
              if (H5Tis_variable_str(basetype_id)>0) {
                int                        arrayRank = H5Tget_array_ndims(attr_type);
                std::unique_ptr<hsize_t[]> arrayDims(new hsize_t[arrayRank]);
                H5Tget_array_dims(attr_type, arrayDims.get());
                std::unique_ptr<char* []> vl(new char*[arrayDims.get()[0]]);
                herr_t                    err = H5Aread(attr_id, attr_type, vl.get());
                if (err < 0) {
                    throw Exception("failed to read array dataset");
                }
                v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), arrayDims.get()[0]);
                for (unsigned int arrayIndex = 0; arrayIndex < arrayDims.get()[0]; arrayIndex++) {
                  std::string s(vl.get()[arrayIndex]);

                  v8::Maybe<bool> ret = array->Set(context,
                      arrayIndex,
                      v8::String::NewFromUtf8(
                          v8::Isolate::GetCurrent(), vl.get()[arrayIndex], v8::NewStringType::kNormal, std::strlen(vl.get()[arrayIndex])).ToLocalChecked());
                  if(ret.ToChecked()){
                    
                  }
                }
                value = array;
              }
            } break;
            case H5T_STRING:
                if (H5Tis_variable_str(attr_type) > 0) {

                  if (num_elements > 1) {
                    std::unique_ptr<char*[]> buf(new char*[num_elements]);
                    /*herr_t err=*/H5Aread(attr_id, attr_type, buf.get());
                    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
                    for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                      std::string attrValue = "";
                      if (buf.get()[elementIndex] != NULL)
                        attrValue = std::string(buf.get()[elementIndex]);
                      v8::Maybe<bool> ret = array->Set(context, elementIndex,
                               v8::String::NewFromUtf8(
                                   v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::NewStringType::kNormal, attrValue.length()).ToLocalChecked());
                      if(ret.ToChecked()){
                        
                      }
                    }
                    value = array;

                  } else {
                  std::unique_ptr<char*[]>data(new char*[1]);
                  //std::memset(data.get(), 0, H5Aget_storage_size(attr_id) + 1); // clear buffer
                    H5Aread(attr_id, attr_type, data.get());
                    std::string attrValue = "";
                    if (data.get()[0] != NULL)
                        attrValue = std::string(data.get()[0]);
                    value =  v8::String::NewFromUtf8(
                                   v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::NewStringType::kNormal, attrValue.length()).ToLocalChecked();
                  }
                } else {

                    hsize_t     storeSize = H5Aget_storage_size(attr_id);
                  std::unique_ptr<char[]> data(new char[storeSize]);
                  std::memset(data.get(), 0, storeSize); // clear buffer

                  H5Aread(attr_id, attr_type, (void*)data.get());
                  if(num_elements>0){
                      hsize_t     offset=storeSize/num_elements;
                    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
                    for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                        hsize_t     trimOffset=offset;
                      if (data.get()[elementIndex*offset+trimOffset-1] ==0){
                          trimOffset=std::strlen((char*)(data.get()+elementIndex*offset));
                      }
                      v8::Maybe<bool> ret = array->Set(context, elementIndex,
                                 v8::String::NewFromUtf8(
                                     v8::Isolate::GetCurrent(), (char*)(data.get()+elementIndex*offset), v8::NewStringType::kNormal, trimOffset).ToLocalChecked());
                      if(ret.ToChecked()){
                        
                      }
                    }
                    value = array;
                  }
                  else{
                    value = v8::String::NewFromUtf8(
                                 v8::Isolate::GetCurrent(), (char*)data.get(), v8::NewStringType::kNormal, storeSize).ToLocalChecked();
                  }
                }
                break;
            default:
              size_t                     size = H5Tget_size(attr_type);
              v8::Local<v8::ArrayBuffer> arrayBuffer;
              v8::Local<v8::TypedArray>  buffer;
              if (class_id == H5T_FLOAT && size == 8 && numberOfElements>1) {
                arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 8 * numberOfElements);
                buffer      = v8::Float64Array::New(arrayBuffer, 0, numberOfElements);
              } else if (class_id == H5T_FLOAT && size == 4 && numberOfElements>1) {
                arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * numberOfElements);
                buffer      = v8::Float32Array::New(arrayBuffer, 0, numberOfElements);
              } else if (class_id == H5T_FLOAT && size == 8) {
                  double dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                value = v8::Number::New(v8::Isolate::GetCurrent(), dValue);
                indexedArray=false;
              } else if (class_id == H5T_FLOAT && size == 4) {
                  float dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                value = v8::Number::New(v8::Isolate::GetCurrent(), dValue);
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 8 && numberOfElements>1) {
                v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), numberOfElements);
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  std::unique_ptr<int64_t[]> intValue(new int64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, intValue.get());
                  for (unsigned int i = 0; i < numberOfElements; i++) {
                    v8::Local<v8::Object> int64Instance = Int64::Instantiate(focus, intValue[i]);
                    Int64*        idWrap   = node::ObjectWrap::Unwrap<Int64>(int64Instance);
                    idWrap->setValue(intValue[i]);
                    v8::Maybe<bool> ret = array->Set(context, i, int64Instance);
                    if(ret.ToChecked()){};
                  }
                } else {
                  std::unique_ptr<uint64_t[]> uintValue(new uint64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, uintValue.get());
                  for (unsigned int i = 0; i < numberOfElements; i++) {
                    v8::Local<v8::Object> uint64Instance = Uint64::Instantiate(focus, uintValue[i]);
                    Uint64*        idWrap   = node::ObjectWrap::Unwrap<Uint64>(uint64Instance);
                    idWrap->setValue(uintValue[i]);
                    v8::Maybe<bool> ret = array->Set(context, i, uint64Instance);
                    if(ret.ToChecked()){};
                  }
                }
                value = array;
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 8) {
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  std::unique_ptr<int64_t[]> intValue(new int64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, intValue.get());
                  v8::Local<v8::Object> int64Instance = Int64::Instantiate(focus, intValue[0]);
                  Int64*        idWrap   = node::ObjectWrap::Unwrap<Int64>(int64Instance);
                  idWrap->setValue(intValue[0]);
                  value = int64Instance;
                } else {
                  std::unique_ptr<uint64_t[]> uintValue(new uint64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, uintValue.get());
                  v8::Local<v8::Object> uint64Instance = Uint64::Instantiate(focus, uintValue[0]);
                  Uint64*        idWrap   = node::ObjectWrap::Unwrap<Uint64>(uint64Instance);
                  idWrap->setValue(uintValue[0]);
                  value = uint64Instance;
                }
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 4 && numberOfElements>1) {
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * numberOfElements);
                  buffer      = v8::Int32Array::New(arrayBuffer, 0, numberOfElements);
                } else {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * numberOfElements);
                  buffer      = v8::Uint32Array::New(arrayBuffer, 0, numberOfElements);
                }
              } else if (class_id == H5T_INTEGER && size == 2 && numberOfElements>1) {
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 2 * numberOfElements);
                  buffer      = v8::Int16Array::New(arrayBuffer, 0, numberOfElements);
                } else {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 2 * numberOfElements);
                  buffer      = v8::Uint16Array::New(arrayBuffer, 0, numberOfElements);
                }
              } else if (class_id == H5T_INTEGER && size == 1 && numberOfElements>1) {
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), numberOfElements);
                  buffer      = v8::Int8Array::New(arrayBuffer, 0, numberOfElements);
                } else {
                  arrayBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), numberOfElements);
                  buffer      = v8::Uint8Array::New(arrayBuffer, 0, numberOfElements);
                }
              } else if (class_id == H5T_INTEGER && size == 4) {
                  int dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                value = v8::Number::New(v8::Isolate::GetCurrent(), dValue);
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 2) {
                  short dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                value = v8::Number::New(v8::Isolate::GetCurrent(), dValue);
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 1) {
                  char dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                value = v8::Number::New(v8::Isolate::GetCurrent(), dValue);
                indexedArray=false;
              } else {
                H5Sclose(space);
                H5Tclose(attr_type);
                H5Aclose(attr_id);
                throw Exception("unsupported data type ");
              }
              if(indexedArray){
#if NODE_VERSION_AT_LEAST(8,0,0)
                H5Aread(attr_id, attr_type, node::Buffer::Data(buffer->ToObject(isolate->GetCurrentContext()).ToLocalChecked()));
#else
                H5Aread(attr_id, attr_type, buffer->Buffer()->Externalize().Data());
#endif
                value = buffer;
              }
              break;
          }

        } break;
        case H5S_SCALAR:
          switch (H5Tget_class(attr_type)) {
            case H5T_INTEGER:
              long long intValue;
              H5Aread(attr_id, attr_type, &intValue);
              value = v8::Int32::New(v8::Isolate::GetCurrent(), intValue);
              break;
            case H5T_FLOAT: {
              size_t size = H5Tget_size(attr_type);
              switch (size) {
                case 8: {
                  double nativeValue;
                  H5Aread(attr_id, attr_type, &nativeValue);
                  value = v8::Number::New(v8::Isolate::GetCurrent(), nativeValue);
                } break;
                default: {
                  float nativeValue;
                  H5Aread(attr_id, attr_type, &nativeValue);
                  value = v8::Number::New(v8::Isolate::GetCurrent(), nativeValue);
                } break;
              }
            } break;
            case H5T_STRING: {
              htri_t isVlen = H5Tis_variable_str(attr_type);
              if (isVlen == 0) {
                /*
                 * Do whatever was done before I came along.
                 */
                hsize_t     storeSize = H5Aget_storage_size(attr_id);
                std::string strValue(storeSize, '\0');
                H5Aread(attr_id, attr_type, (void*)strValue.c_str());
                value = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str(), v8::NewStringType::kInternalized).ToLocalChecked();
              } else if (isVlen != -1) {

                H5A_info_t ainfo;
                H5Aget_info(attr_id, &ainfo);
                std::unique_ptr<char* []> buffer(new char*[2]);

                /*
                 * Create the memory datatype.
                 */
                hid_t  memtype = H5Tcopy(H5T_C_S1);
                herr_t status  = H5Tset_size(memtype, H5T_VARIABLE);
                if (status < 0) {
                  throw Exception("failed to set size variable");
                }

                hid_t type = H5Tget_native_type(attr_type, H5T_DIR_ASCEND);
                /*
                 * Read the data.
                 */
                status                                = H5Aread(attr_id, type, buffer.get());
                v8::Local<v8::String> varLenStr       = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.get()[0], v8::NewStringType::kInternalized).ToLocalChecked();
                v8::Local<v8::Value>  varLenStrObject = v8::StringObject::New(v8::Isolate::GetCurrent(), varLenStr);
                bool ret = varLenStrObject->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked(),
                                                 v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "variable-length", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked();
                if(ret){};
                value = varLenStrObject;

                /*
                 * Clean up the mess I made
                 */
                status = H5Tclose(memtype);
              }
            } break;
            case H5T_NO_CLASS:
            default: break;
          }
          break;
        case H5T_NO_CLASS:
        default: break;
      }
      H5Sclose(space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);
      return value;
};

    inline void refreshAttributes(v8::Local<v8::Object>& focus, hid_t id){
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      hsize_t                  index = 0;
      std::vector<std::string> holder;
      H5Aiterate(id,
               H5_INDEX_NAME,
               H5_ITER_INC,
               &index,
               [](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void* operator_data) -> herr_t {
                 ((std::vector<std::string>*)operator_data)->push_back(attr_name);
                 return 0;
               },
               &holder);
      for (index = 0; index < (uint32_t)holder.size(); index++) {
          v8::Local<v8::Value>&& value = readAttributeByName(focus, id, holder[index]);
          v8::Maybe<bool> ret = focus->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), value);
          if(ret.ToChecked()){
            
          }
      }
        
    };

  class Attributes : public node::ObjectWrap {
  protected:
    std::string name;
    hid_t       id      = 0;
    hid_t       gcpl_id = 0;

  public:
    Attributes(){};
    Attributes(hid_t id)
        : id(id){};
    Attributes(const Attributes& orig) = delete;
    virtual ~Attributes(){};

    static void make_attribute_from_typed_array(const hid_t&               group_id,
                                                const char*                attribute_name,
                                                v8::Local<v8::TypedArray> buffer,
                                                hid_t                      type_id);
    static void make_attribute_from_array(const hid_t& group_id, const char* attribute_name, v8::Local<v8::Array> array);
    static void Refresh(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Flush(const v8::FunctionCallbackInfo<v8::Value>& args);

  protected:
    virtual int getNumAttrs() = 0;
    
  };
  
}
