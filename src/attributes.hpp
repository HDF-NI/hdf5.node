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
    
    inline void refreshAttributes(v8::Local<v8::Object>& focus, hid_t id){
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
      bool indexedArray=true;
      hid_t attr_id   = H5Aopen(id, holder[index].c_str(), H5P_DEFAULT);
      hid_t attr_type = H5Aget_type(attr_id);
      hid_t space     = H5Aget_space(attr_id);
      hssize_t num_elements = H5Sget_simple_extent_npoints(space);
      switch (H5Sget_simple_extent_type(space)) {
        case H5S_SIMPLE: {
          hssize_t    numberOfElements = H5Sget_simple_extent_npoints(space);
          H5T_class_t class_id         = H5Tget_class(attr_type);
          switch (class_id) {
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

                  array->Set(
                      arrayIndex,
                      v8::String::NewFromUtf8(
                          v8::Isolate::GetCurrent(), vl.get()[arrayIndex], v8::String::kNormalString, std::strlen(vl.get()[arrayIndex])));
                }
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), array);
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
                      array->Set(elementIndex,
                                 v8::String::NewFromUtf8(
                                     v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::String::kNormalString, attrValue.length()));
                    }
                    focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), array);

                  } else {
                  std::unique_ptr<char*[]>data(new char*[1]);
                  //std::memset(data.get(), 0, H5Aget_storage_size(attr_id) + 1); // clear buffer
                    H5Aread(attr_id, attr_type, data.get());
                    std::string attrValue = "";
                    if (data.get()[0] != NULL)
                        attrValue = std::string(data.get()[0]);
                    focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::String::NewFromUtf8(
                                   v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::String::kNormalString, attrValue.length()));
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
                      array->Set(elementIndex,
                                 v8::String::NewFromUtf8(
                                     v8::Isolate::GetCurrent(), (char*)(data.get()+elementIndex*offset), v8::String::kNormalString, trimOffset));
                    }
                    focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), array);
                  }
                  else{
                    focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                             v8::String::NewFromUtf8(
                                 v8::Isolate::GetCurrent(), (char*)data.get(), v8::String::kNormalString, storeSize));
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
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Number::New(v8::Isolate::GetCurrent(), dValue));
                indexedArray=false;
              } else if (class_id == H5T_FLOAT && size == 4) {
                  float dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Number::New(v8::Isolate::GetCurrent(), dValue));
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
                    array->Set(i, int64Instance);
                  }
                } else {
                  std::unique_ptr<uint64_t[]> uintValue(new uint64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, uintValue.get());
                  for (unsigned int i = 0; i < numberOfElements; i++) {
                    v8::Local<v8::Object> uint64Instance = Uint64::Instantiate(focus, uintValue[i]);
                    Uint64*        idWrap   = node::ObjectWrap::Unwrap<Uint64>(uint64Instance);
                    idWrap->setValue(uintValue[i]);
                    array->Set(i, uint64Instance);
                  }
                }
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               array);
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 8) {
                if (H5Tget_sign(attr_type) == H5T_SGN_2) {
                  std::unique_ptr<int64_t[]> intValue(new int64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, intValue.get());
                  v8::Local<v8::Object> int64Instance = Int64::Instantiate(focus, intValue[0]);
                  Int64*        idWrap   = node::ObjectWrap::Unwrap<Int64>(int64Instance);
                  idWrap->setValue(intValue[0]);
                  focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               int64Instance);
                } else {
                  std::unique_ptr<uint64_t[]> uintValue(new uint64_t[numberOfElements]);
                  H5Aread(attr_id, attr_type, uintValue.get());
                  v8::Local<v8::Object> uint64Instance = Uint64::Instantiate(focus, uintValue[0]);
                  Uint64*        idWrap   = node::ObjectWrap::Unwrap<Uint64>(uint64Instance);
                  idWrap->setValue(uintValue[0]);
                  focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               uint64Instance);
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
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Number::New(v8::Isolate::GetCurrent(), dValue));
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 2) {
                  short dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Number::New(v8::Isolate::GetCurrent(), dValue));
                indexedArray=false;
              } else if (class_id == H5T_INTEGER && size == 1) {
                  char dValue;
                  H5Aread(attr_id, attr_type, &dValue);
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Number::New(v8::Isolate::GetCurrent(), dValue));
                indexedArray=false;
              } else {
                H5Sclose(space);
                H5Tclose(attr_type);
                H5Aclose(attr_id);
                throw Exception("unsupported data type ");
              }
              if(indexedArray){
#if NODE_VERSION_AT_LEAST(8,0,0)
                H5Aread(attr_id, attr_type, node::Buffer::Data(buffer->ToObject()));
#else
                H5Aread(attr_id, attr_type, buffer->Buffer()->Externalize().Data());
#endif
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), buffer);
              }
              break;
          }

        } break;
        case H5S_SCALAR:
          switch (H5Tget_class(attr_type)) {
            case H5T_INTEGER:
              long long intValue;
              H5Aread(attr_id, attr_type, &intValue);
              focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                               v8::Int32::New(v8::Isolate::GetCurrent(), intValue));
              break;
            case H5T_FLOAT: {
              size_t size = H5Tget_size(attr_type);
              switch (size) {
                case 8: {
                  double value;
                  H5Aread(attr_id, attr_type, &value);
                  focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                                   v8::Number::New(v8::Isolate::GetCurrent(), value));
                } break;
                default: {
                  float value;
                  H5Aread(attr_id, attr_type, &value);
                  focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                                   v8::Number::New(v8::Isolate::GetCurrent(), value));
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
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()),
                                 v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
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
                v8::Local<v8::String> varLenStr       = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.get()[0]);
                v8::Local<v8::Value>  varLenStrObject = v8::StringObject::New(varLenStr);
                varLenStrObject->ToObject()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"),
                                                 v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "variable-length"));
                focus->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), varLenStrObject);

                /*
                 * Clean up the mess I made
                 */
                status = H5Tclose(memtype);
              }
            } break;
            case H5T_NO_CLASS:
            default: return;
          }
          break;
        case H5T_NO_CLASS:
        default: break;
      }
      H5Sclose(space);
      H5Tclose(attr_type);
      H5Aclose(attr_id);
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
                                                v8::Handle<v8::TypedArray> buffer,
                                                hid_t                      type_id);
    static void make_attribute_from_array(const hid_t& group_id, const char* attribute_name, v8::Handle<v8::Array> array);
    static void Refresh(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Flush(const v8::FunctionCallbackInfo<v8::Value>& args);

  protected:
    virtual int getNumAttrs() = 0;
    
  };
}
