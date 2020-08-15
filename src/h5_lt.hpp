#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "H5LTpublic.h"

#include "attributes.hpp"

static herr_t H5LT_make_dataset_numerical(hid_t          loc_id,
                                          const char*    dset_name,
                                          int            rank,
                                          const hsize_t* dims,
                                          const hsize_t* maxdims,
                                          hid_t          tid,
                                          hid_t          lcpl_id,
                                          hid_t          dcpl_id,
                                          hid_t          dapl_id,
                                          const void*    data) {
  hid_t did = -1, sid = -1;

  /* check the arguments */
  if (dset_name == NULL)
    return -1;

  /* Create the data space for the dataset. */
  if ((sid = H5Screate_simple(rank, dims, maxdims)) < 0)
    return -1;

  /* Create the dataset. */
  if ((did = H5Dcreate2(loc_id, dset_name, tid, sid, lcpl_id, dcpl_id, dapl_id)) < 0)
    goto out;

  /* Write the dataset only if there is data to write */
  if (data)
    if (H5Dwrite(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
      goto out;

  /* End access to the dataset and release resources used by it. */
  if (H5Dclose(did) < 0)
    return -1;

  /* Terminate access to the data space. */
  if (H5Sclose(sid) < 0)
    return -1;

  return 0;

out:
  H5E_BEGIN_TRY {
    H5Dclose(did);
    H5Sclose(sid);
  }
  H5E_END_TRY;
  return -1;
}

namespace NodeHDF5 {

  class H5lt {
  protected:
  public:
    static void Initialize(Handle<Object> target) {

      // append this function to the target object
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeDataset"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::make_dataset)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeDataset"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::write_dataset)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetLength"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset_length)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetDatatype"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset_datatype)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDataset"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetAsBuffer"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::readDatasetAsBuffer)->GetFunction());
    }

    inline static bool is_bind_attributes(const v8::FunctionCallbackInfo<Value>& args, unsigned int argIndex){
        bool bindAttributes=false;
        Local<Array> names = args[argIndex]->ToObject()->GetOwnPropertyNames();
        for (uint32_t index = 0; index < names->Length(); index++) {
          String::Utf8Value _name(names->Get(index));
          std::string       name(*_name);
          if (name.compare("bind_attributes") == 0) {
            bindAttributes = args[argIndex]->ToObject()->Get(names->Get(index))->ToBoolean()->BooleanValue();
          }
        }
        return bindAttributes;
    }

    inline static bool get_dimensions(const v8::FunctionCallbackInfo<Value>& args, unsigned int argIndex, std::unique_ptr<hsize_t[]>& start, std::unique_ptr<hsize_t[]>& stride, std::unique_ptr<hsize_t[]>& count, int rank){
        bool subsetOn=false;
        bool gotStart=false;
        bool gotStride=false;
        bool gotCount=false;
        unsigned int size=0;
        Local<Array> names = args[argIndex]->ToObject()->GetOwnPropertyNames();
        for (uint32_t index = 0; index < names->Length(); index++) {
          String::Utf8Value _name(names->Get(index));
          std::string       name(*_name);
          if (name.compare("start") == 0) {
            Local<Object> starts = args[argIndex]->ToObject()->Get(names->Get(index))->ToObject();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < starts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();
                 arrayIndex++) {
              start.get()[arrayIndex] = starts->Get(arrayIndex)->Uint32Value();
            }
            gotStart=true;
          } else if (name.compare("stride") == 0) {
            Local<Object> strides = args[argIndex]->ToObject()->Get(names->Get(index))->ToObject();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < strides->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();
                 arrayIndex++) {
              stride.get()[arrayIndex] = strides->Get(arrayIndex)->Uint32Value();
            }
            gotStride=true;
          } else if (name.compare("count") == 0) {
            Local<Object> counts = args[argIndex]->ToObject()->Get(names->Get(index))->ToObject();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < counts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();
                 arrayIndex++) {
              count.get()[arrayIndex] = counts->Get(arrayIndex)->Uint32Value();
              size++;
            }
            gotCount=true;
            subsetOn = true;
          }
        }
        if(!gotStart && !gotStride && !gotCount)
          return false;
        if(!gotCount)v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Need to supply the subspace count dimensions. Start and stride are optional.")));
        if(!gotStart){
          for (unsigned int arrayIndex = 0;
             arrayIndex < size;
             arrayIndex++) {
                start.get()[arrayIndex] = 0;
          }
        }
        if(!gotStride){
          for (unsigned int arrayIndex = 0;
             arrayIndex < size;
             arrayIndex++) {
                stride.get()[arrayIndex] = 1;
          }
        }
              
        return subsetOn;
    }

    inline static int get_array_rank(Handle<Array> array){
        int rank=1;
        Handle<Array> arrayCheck=array;
        bool look=true;
        while(look){
        Local<Array> names = arrayCheck->ToObject()->GetOwnPropertyNames();
        bool hit=false;
        for (uint32_t index = 0; !hit && index < names->Length(); index++) {
          if (arrayCheck->ToObject()->Get(names->Get(index))->ToObject()->IsArray()) {
              arrayCheck=Local<v8::Array>::Cast(arrayCheck->ToObject()->Get(names->Get(index))->ToObject());
              hit=true;
              rank++;
          }
        }
        if(!hit)look=false;
        }
        return rank;
    }
    
    inline static void get_array_dimensions(Handle<Array> array, std::unique_ptr<hsize_t[]>& dims, int rank){
        Handle<Array> arrayCheck=array;
        int count=0;
        dims[count++]=arrayCheck->Length();
        bool look=true;
        while(look && count<rank){
        Local<Array> names = arrayCheck->ToObject()->GetOwnPropertyNames();
        bool hit=false;
        for (uint32_t index = 0; !hit && index < names->Length(); index++) {
          if (arrayCheck->ToObject()->Get(names->Get(index))->ToObject()->IsArray()) {
            arrayCheck=Local<v8::Array>::Cast(arrayCheck->ToObject()->Get(names->Get(index))->ToObject());
            hit=true;
            dims[count++]=arrayCheck->Length();
          }
        }
        if(!hit)look=false;
        }
    }

    static void fill_buffer_from_multi_array(Handle<Array> array, std::unique_ptr<char[]>& vl, unsigned int fixedWidth, unsigned int& index, int rank){
        Local<Array> names = array->ToObject()->GetOwnPropertyNames();
        for (uint32_t arrayIndex = 0; arrayIndex < names->Length(); arrayIndex++) {
          if (array->ToObject()->Get(names->Get(arrayIndex))->ToObject()->IsArray()) {
            Handle<Array> arrayCheck=Local<v8::Array>::Cast(array->ToObject()->Get(names->Get(arrayIndex))->ToObject());
            fill_buffer_from_multi_array(arrayCheck, vl, fixedWidth, index, rank);
          }
          else{
          std::string s;
          String::Utf8Value buffer(array->Get(arrayIndex)->ToString());
          s.assign(*buffer);
          if (fixedWidth < s.length()) {
              throw Exception("failed fixed width was too small: "+std::to_string(fixedWidth));
          }
            std::strncpy(&vl.get()[fixedWidth * index], s.c_str(), s.length());
          index++;
              
          }
        }
    }
    
    static void fill_multi_array(Handle<Array>& array, std::unique_ptr<char[]>& tbuffer, std::unique_ptr<hsize_t[]>& dims, std::unique_ptr<hsize_t[]>& count, size_t fixedWidth, hsize_t& index, int depth, int rank, H5T_str_t paddingType){

        for (uint32_t arrayIndex = 0; arrayIndex < std::min(dims.get()[depth], count.get()[depth]); arrayIndex++) {
          if (depth<rank-1) {
            Handle<Array> arrayCheck=Array::New(v8::Isolate::GetCurrent(), std::min(dims.get()[depth], count.get()[depth]));
            int ldepth=depth+1;
            fill_multi_array(arrayCheck, tbuffer, dims, count, fixedWidth, index, ldepth, rank, paddingType);
            array->Set(arrayIndex, arrayCheck);
          }
          else{
            hsize_t realLength=0;
            char delimiter=(paddingType==H5T_STR_SPACEPAD) ? ' ' : 0;
            while(realLength<fixedWidth && ((char)tbuffer.get()[fixedWidth * index+realLength])!=delimiter){
              realLength++;
            }
            array->Set(arrayIndex,
                       String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                           &tbuffer.get()[fixedWidth * index],
                                           String::kNormalString,
                                           realLength));
              index++;
              
          }
        }
    }
    
    inline static unsigned int get_fixed_width(Handle<Object> options) {
      if (options.IsEmpty()) {
        return 0;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "fixed_width"));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return 0;
      }

      return options->Get(name)->Uint32Value();
    }

    inline static H5T_str_t get_padding_type(Handle<Object> options) {
      if (options.IsEmpty()) {
        return H5T_STR_NULLTERM;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "padding"));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return H5T_STR_NULLTERM;
      }

      return (H5T_str_t)options->Get(name)->Uint32Value();
    }

    static void get_type(Handle<Object> options, std::function<void(hid_t)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"));

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(toTypeMap[(H5T)options->Get(name)->Uint32Value()]);
      }
    }
    
    static void get_rank(Handle<Object> options, std::function<void(int)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(name)->Uint32Value());
      }
    }
    
    static void get_rows(Handle<Object> options, std::function<void(int)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"));

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(name)->Uint32Value());
      }
    }
    
    static void get_columns(Handle<Object> options, std::function<void(int)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"));

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(name)->Uint32Value());
      }
    }
    
    static void get_sections(Handle<Object> options, std::function<void(int)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"));

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(name)->Uint32Value());
      }
    }
    
    static void get_files(Handle<Object> options, std::function<void(int)> cb) {
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"));

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(name)->Uint32Value());
      }
    }
    
    static unsigned int get_compression(Handle<Object> options) {
      if (options.IsEmpty()) {
        return 0;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "compression"));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return 0;
      }

      return options->Get(name)->Uint32Value();
    }

    static int get_option_int(Handle<Object> options,const char * option_name, int default_value) {
      if (options.IsEmpty()) {
        return default_value;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), option_name));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return default_value;
      }

      return options->Get(name)->Int32Value();
    }

    static std::unique_ptr<hsize_t[]> get_chunk_size(Handle<Object> options, int rank) {
      std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);  
      for(int index=0;index<rank;index++){
        dims[index]=0;
      }
      if (options.IsEmpty()) {
        return dims;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "chunkSize"));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return dims;
      }
      if(options->Get(name)->IsArray()){
          v8::Handle<v8::Array> array = v8::Local<v8::Array>::Cast(options->Get(name));
        for (unsigned int arrayIndex = 0; arrayIndex < std::min((uint32_t)rank, array->Length()); arrayIndex++) {
           dims.get()[arrayIndex]=array->Get(arrayIndex)->Uint32Value();
        }
          
      }
      else {
        for(int index=0;index<rank;index++){
          dims.get()[index]=options->Get(name)->Uint32Value();
        }
      }

      return dims;
    }

    // TODO: permit other that square geometry
    static bool configure_chunked_layout(const hid_t& dcpl,  std::unique_ptr<hsize_t[]>& chunk_dims, const int& rank, const hsize_t* ds_dim) {
      herr_t err;
      if (chunk_dims[0]==0) {
        err = H5Pset_chunk(dcpl, rank, ds_dim);
        if (err) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
          return false;
        }

        return true;
      }

      err = H5Pset_chunk(dcpl, rank, chunk_dims.get());
      if (err) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
        return false;
      }

      return true;
    }

    static bool configure_compression(const hid_t& dcpl, const unsigned int& compression) {
      herr_t err = H5Pset_deflate(dcpl, compression);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
        return false;
      }

      return true;
    }

    static void make_dataset_from_buffer(const hid_t& group_id, const char* dset_name, Handle<Object> buffer, Handle<Object> options) {
      Local<Value>      encodingValue = buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding"));
      String::Utf8Value encoding(encodingValue->ToString());
      if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding")) && std::strcmp("binary", (*encoding))) {
        herr_t err = H5LTmake_dataset_string(group_id, dset_name, const_cast<char*>(node::Buffer::Data(buffer)));
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
          return;
        }
        return;
      }
      hid_t type_id;
      hsize_t propertyStartIndex=0;
      if (buffer->IsFloat64Array()) {
        type_id=H5T_NATIVE_DOUBLE;
        propertyStartIndex=Local<Float64Array>::Cast(buffer)->Length();
      } else if (buffer->IsFloat32Array()) {
        type_id=H5T_NATIVE_FLOAT;
        propertyStartIndex=Local<Float32Array>::Cast(buffer)->Length();
      } else if (buffer->IsInt32Array()) {
        type_id=H5T_NATIVE_INT;
        propertyStartIndex=Local<Int32Array>::Cast(buffer)->Length();
      } else if (buffer->IsUint32Array()) {
        type_id=H5T_NATIVE_UINT;
        propertyStartIndex=Local<Uint32Array>::Cast(buffer)->Length();
      } else if (buffer->IsInt16Array()) {
        type_id=H5T_NATIVE_SHORT;
        propertyStartIndex=Local<Int16Array>::Cast(buffer)->Length();
      } else if (buffer->IsUint16Array()) {
        type_id=H5T_NATIVE_USHORT;
        propertyStartIndex=Local<Uint16Array>::Cast(buffer)->Length();
      } else if (buffer->IsInt8Array()) {
        type_id=H5T_NATIVE_INT8;
        propertyStartIndex=Local<Int8Array>::Cast(buffer)->Length();
      }
      else{
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))) {
          type_id = toTypeMap[(H5T)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->Int32Value()];
        }
        else get_type(options, [&](hid_t _type_id){type_id=_type_id;});
      }
      int   rank    = 1;
      if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"))) {
        Local<Value> rankValue = buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank                   = rankValue->Int32Value();
      }
      else get_rank(options, [&](int _rank){rank=_rank;});
      std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> maxdims(new hsize_t[rank]);
      switch (rank) {
        case 1: 
          dims.get()[0] = {node::Buffer::Length(buffer) / H5Tget_size(type_id)}; 
          maxdims.get()[0] = get_option_int(options,"maxRows",dims.get()[0]);
          break;
        case 4: 
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"))) {
            dims.get()[0] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"))->Int32Value();
          }else{
            get_files(options, [&](int sections){dims.get()[0]=sections;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))) {
            dims.get()[1] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->Int32Value();
          }else{
            get_sections(options, [&](int sections){dims.get()[1]=sections;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))) {
            dims.get()[3] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
          }else{
            get_columns(options, [&](int columns){dims.get()[3]=columns;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))) {
            dims.get()[2] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
          }else{
            get_rows(options, [&](int rows){dims.get()[2]=rows;});
          }
          maxdims.get()[0] = get_option_int(options,"maxFiles",dims.get()[0]);
          maxdims.get()[1] = get_option_int(options,"maxSections",dims.get()[1]);
          maxdims.get()[3] = get_option_int(options,"maxColumns",dims.get()[3]);
          maxdims.get()[2] = get_option_int(options,"maxRows",dims.get()[2]);
          break;        
        case 3: 
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))) {
            dims.get()[0] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->Int32Value();
          }else{
            get_sections(options, [&](int sections){dims.get()[0]=sections;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))) {
            dims.get()[2] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
          }else{
            get_columns(options, [&](int columns){dims.get()[2]=columns;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))) {
            dims.get()[1] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
          }else{
            get_rows(options, [&](int rows){dims.get()[1]=rows;});
          }
          maxdims.get()[0] = get_option_int(options,"maxSections",dims.get()[0]);
          maxdims.get()[2] = get_option_int(options,"maxColumns",dims.get()[2]);
          maxdims.get()[1] = get_option_int(options,"maxRows",dims.get()[1]);
          break;
        case 2:
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))) {
            dims.get()[1] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
          }else{
            get_columns(options, [&](int columns){dims.get()[1]=columns;});
          }
          if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))) {
            dims.get()[0] = (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
          }else{
            get_rows(options, [&](int rows){dims.get()[0]=rows;});
          }
          maxdims.get()[1] = get_option_int(options,"maxColumns",dims.get()[1]);
          maxdims.get()[0] = get_option_int(options,"maxRows",dims.get()[0]);
          break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
          return;
          break;
      }
      unsigned int compression = get_compression(options);
      std::unique_ptr<hsize_t[]>&& chunk_dims  = get_chunk_size(options, rank);
      hid_t        dcpl        = H5Pcreate(H5P_DATASET_CREATE);
      if (compression > 0) {
        if (!configure_compression(dcpl, compression)) {
          return;
        }
      }

      if (!configure_chunked_layout(dcpl, chunk_dims, rank, dims.get())) {
        return;
      }

      herr_t err = H5LT_make_dataset_numerical(
          group_id, dset_name, rank, dims.get(), maxdims.get(), type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer));
      if (err < 0) {
        H5Pclose(dcpl);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
        return;
      }
      H5Pclose(dcpl);

      // Atributes
      v8::Local<v8::Array> propertyNames = buffer->GetPropertyNames();
      if(propertyStartIndex==0)propertyStartIndex=propertyNames->Length();
      for (hsize_t index = propertyStartIndex; index < propertyNames->Length(); index++) {
        v8::Local<v8::Value> name = propertyNames->Get(index);
        if (!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() &&
            strncmp("id", (*String::Utf8Value(name->ToString())), 2) != 0 &&
            strncmp("rank", (*String::Utf8Value(name->ToString())), 4) != 0 &&
            strncmp("rows", (*String::Utf8Value(name->ToString())), 4) != 0 &&
            strncmp("columns", (*String::Utf8Value(name->ToString())), 7) != 0 &&
            strncmp("buffer", (*String::Utf8Value(name->ToString())), 6) != 0) {

          if (buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal()) {
          } else if (buffer->Get(name)->IsUint32()) {
            uint32_t value = buffer->Get(name)->Uint32Value();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

          } else if (buffer->Get(name)->IsInt32()) {
            int32_t value = buffer->Get(name)->Int32Value();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

          } else if (buffer->Get(name)->IsString()) {
            std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
          } else if (buffer->Get(name)->IsNumber()) {
            double value = buffer->Get(name)->NumberValue();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
          }
        }
      }
    }

    static void make_dataset_from_typed_array(
        const hid_t& group_id, const char* dset_name, Handle<TypedArray> buffer, Handle<Object> options, hid_t type_id) {
      int rank = 1;
      get_rank(options, [&](int _rank){rank=_rank;});
      if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"))) {
        Local<Value> rankValue = buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank                   = rankValue->Int32Value();
      }

      if (rank == 1) {
        hsize_t      dims[1]     = {buffer->Length()};
        unsigned int compression = get_compression(options);
        std::unique_ptr<hsize_t[]>&& chunk_dims  = get_chunk_size(options, rank);

        hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);

        if (compression > 0) {
          if (!configure_compression(dcpl, compression)) {
            return;
          }
        }

        if (!configure_chunked_layout(dcpl, chunk_dims, rank, dims)) {
          return;
        }

        herr_t err = H5LT_make_dataset_numerical(
#if NODE_VERSION_AT_LEAST(8,0,0)
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
          return;
        }
        H5Pclose(dcpl);
      } else if (rank == 2) {
          
        hsize_t dims[2];
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))) {
          dims[0]= (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
        }else get_rows(options, [&](int rows){dims[0]=rows;});
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))) {
        dims[1]= (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
        }else get_columns(options, [&](int columns){dims[1]=columns;});
        unsigned int compression = get_compression(options);
        std::unique_ptr<hsize_t[]>&& chunk_dims = get_chunk_size(options, rank);

        hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);

        if (compression > 0) {
          if (!configure_compression(dcpl, compression)) {
            return;
          }
        }

        if (!configure_chunked_layout(dcpl, chunk_dims, rank, dims)) {
          return;
        }

        herr_t err = H5LT_make_dataset_numerical(
#if NODE_VERSION_AT_LEAST(8,0,0)
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
          return;
        }
        H5Pclose(dcpl);
      } else if (rank == 3) {
        hsize_t dims[3];
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))) {
          dims[0]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
        }else get_rows(options, [&](int rows){dims[0]=rows;});
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))) {
          dims[1]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
        }else get_columns(options, [&](int columns){dims[1]=columns;});
        if (buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))) {
          dims[2]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->Int32Value();
        }else get_sections(options, [&](int sections){dims[2]=sections;});
        unsigned int compression = get_compression(options);
        std::unique_ptr<hsize_t[]>&& chunk_dims  = get_chunk_size(options, rank);

        hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);

        if (compression > 0) {
          if (!configure_compression(dcpl, compression)) {
            return;
          }
        }

        if (!configure_chunked_layout(dcpl, chunk_dims, rank, dims)) {
          return;
        }

        herr_t err = H5LT_make_dataset_numerical(
#if NODE_VERSION_AT_LEAST(8,0,0)
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
          return;
        }
        H5Pclose(dcpl);
      } else {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
        return;
      }

      // Atributes
      v8::Local<v8::Array> propertyNames = buffer->GetPropertyNames();
      for (unsigned int index = buffer->Length(); index < propertyNames->Length(); index++) {
        v8::Local<v8::Value> name = propertyNames->Get(index);
        if (!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() &&
            strncmp("id", (*String::Utf8Value(name->ToString())), 2) != 0 &&
            strncmp("rank", (*String::Utf8Value(name->ToString())), 4) != 0 &&
            strncmp("rows", (*String::Utf8Value(name->ToString())), 4) != 0 &&
            strncmp("columns", (*String::Utf8Value(name->ToString())), 7) != 0 &&
            strncmp("buffer", (*String::Utf8Value(name->ToString())), 6) != 0) {
          if (buffer->Get(name)->IsObject()){
            std::string constructorName = "Reference";
            if (constructorName.compare(*String::Utf8Value(buffer->Get(name)->ToObject()->GetConstructorName())) == 0) {
                v8::Local<v8::Object> obj=buffer->Get(name)->ToObject();
            // unwrap ref
            Reference* ref =  ObjectWrap::Unwrap<Reference>(obj);
            hid_t attr_type  = H5Tcopy(H5T_STD_REF_OBJ);
            std::unique_ptr<hsize_t[]> currentDims(new hsize_t[1]);
            currentDims.get()[0] = 1;
            hid_t attr_space     = H5Screate_simple(1, currentDims.get(), NULL);
//            hid_t attr_space = H5Screate(H5S_SCALAR);
            hid_t  did = H5Dopen(group_id, dset_name, H5P_DEFAULT);
            hid_t attr_id = H5Acreate(did, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
            if (attr_id < 0) {
              H5Dclose(did);
              H5Sclose(attr_space);
              H5Tclose(attr_type);
              continue;
            }
            hid_t value=ref->getObjectId();
            H5Awrite(attr_id, attr_type, &value);
            H5Dclose(did);
            H5Sclose(attr_space);
            H5Tclose(attr_type);
            H5Aclose(attr_id);
                
            }
          }
          else if( buffer->Get(name)->IsExternal()) {

          } else if (buffer->Get(name)->IsUint32()) {
            uint32_t value = buffer->Get(name)->Uint32Value();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

          } else if (buffer->Get(name)->IsInt32()) {
            int32_t value = buffer->Get(name)->Int32Value();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

          } else if (buffer->Get(name)->IsString()) {
            std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }

            H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
          } else if (buffer->Get(name)->IsNumber()) {
            double value = buffer->Get(name)->NumberValue();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
            }
            H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
          }
        }
      }
    }

    static void make_dataset_from_array(const hid_t& group_id, const char* dset_name, Handle<Array> array, Handle<Object> options) {
      bool hasOptionType=false;
      get_type(options, [&](hid_t _type_id){hasOptionType=true;});
      hid_t        dcpl       = H5Pcreate(H5P_DATASET_CREATE);
      int          rank       = get_array_rank(array);
      std::unique_ptr<hsize_t[]> countSpace(new hsize_t[rank]);
      get_array_dimensions(array, countSpace, rank);
      unsigned int fixedWidth = get_fixed_width(options);
      H5T_str_t paddingType = get_padding_type(options);
      std::unique_ptr<hsize_t[]> maxsize(new hsize_t[rank]);
      unsigned int totalSize=1;
      for(int rankIndex=0;rankIndex<rank;rankIndex++){
        totalSize*=countSpace[rankIndex];
        maxsize[rankIndex]=H5S_UNLIMITED;
      }
      if (fixedWidth > 0) {
        std::unique_ptr<char[]> vl(new char[fixedWidth * totalSize]);
        
        if(paddingType==H5T_STR_NULLTERM || paddingType==H5T_STR_NULLPAD)std::memset(vl.get(), 0, fixedWidth * totalSize);
        else std::memset(vl.get(), ' ', fixedWidth * totalSize);
        unsigned int index=0;
        try{
          fill_buffer_from_multi_array(array, vl, fixedWidth, index, rank);
        } catch (Exception& ex) {
          v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
          return;
        }
        hid_t memspace_id   = H5Screate_simple(rank, countSpace.get(), NULL);
        hid_t type_id       = H5Tcopy(H5T_C_S1);
        H5Tset_size(type_id, fixedWidth);
        if(paddingType>0)H5Tset_strpad(type_id, paddingType);
        hid_t  did = H5Dcreate(group_id, dset_name, type_id, memspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);
        herr_t err = H5Dwrite(did, type_id, memspace_id, H5S_ALL, H5P_DEFAULT, vl.get());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset")));
          return;
        }

        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
      } else if(hasOptionType){
        std::unique_ptr<hvl_t[]> vl(new hvl_t[array->Length()]);
        for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
          char* buffer = (char*)node::Buffer::Data(array->Get(arrayIndex));
           hsize_t length=Local<v8::Int8Array>::Cast(array->Get(arrayIndex))->Length();
          //s.assign(*buffer);
          vl.get()[arrayIndex].p = new char[length];
          vl.get()[arrayIndex].len = length;
          std::memcpy(vl.get()[arrayIndex].p, buffer, length);
        }
        hid_t memspace_id = H5Screate_simple(rank, countSpace.get(), NULL);
        hid_t type_id;
          get_type(options, [&](hid_t _type_id){type_id=H5Tvlen_create(_type_id);});
        hid_t did = H5Dcreate(group_id, dset_name, type_id, memspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);

        herr_t err = H5Dwrite(did, type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, vl.get());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset")));
          return;
        }
//H5Dvlen_reclaim(type_id, did, dcpl, vl.get());
        //H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
        
      } else {
        std::unique_ptr<char*[]> vl(new char*[array->Length()]);
        for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
          String::Utf8Value buffer(array->Get(arrayIndex));
          //s.assign(*buffer);
          vl.get()[arrayIndex] = new char[buffer.length() + 1];
          vl.get()[arrayIndex][buffer.length()]=0;
          std::memcpy(vl.get()[arrayIndex], *buffer, buffer.length());
        }
        hid_t memspace_id = H5Screate_simple(rank, countSpace.get(), NULL);
        hid_t type_id     = H5Tcopy(H5T_C_S1);
        H5Tset_size(type_id, H5T_VARIABLE);
        hid_t did = H5Dcreate(group_id, dset_name, type_id, memspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);

        herr_t err = H5Dwrite(did, type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, vl.get());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset")));
          return;
        }

        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
      }
    }

    static void make_dataset_from_string(const hid_t& group_id, const char* dset_name, Handle<String> buffer, Handle<Object> /*options*/) {
      String::Utf8Value str_buffer(buffer);
      herr_t            err = H5LTmake_dataset_string(group_id, dset_name, const_cast<char*>(*str_buffer));
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
        return;
      }
    }

    static void make_dataset(const v8::FunctionCallbackInfo<Value>& args) {
      String::Utf8Value dset_name_ptr(args[1]->ToString());
      const char*       dset_name(*dset_name_ptr);
      Int64*            idWrap   = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      hid_t             group_id = idWrap->Value();
      Local<Value>      buffer   = args[2];
      Local<Object>     options;
      bool hasOptionType=false;
      if (args.Length() >= 4 && args[3]->IsObject()) {
        options = args[3]->ToObject();
        get_type(options, [&](hid_t _type_id){hasOptionType=true;});
      }
      bool bufferAsUnit8Array=true;
#if NODE_VERSION_AT_LEAST(8,0,0)
#else
      bufferAsUnit8Array=buffer->ToObject()->IsUint8Array();
#endif
      if ((hasOptionType && !buffer->IsArray()) || (bufferAsUnit8Array && node::Buffer::HasInstance(buffer) && buffer->ToObject()->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type")))) {
        make_dataset_from_buffer(group_id, dset_name, buffer->ToObject(), options);
      } else if (buffer->IsString()) {
        make_dataset_from_string(group_id, dset_name, buffer->ToString(), options);
      } else if (buffer->IsArray()) {
        make_dataset_from_array(group_id, dset_name, Local<v8::Array>::Cast(buffer), options);
      } else if (buffer->IsFloat64Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Float64Array>::Cast(buffer), options, H5T_NATIVE_DOUBLE);
      } else if (buffer->IsFloat32Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Float32Array>::Cast(buffer), options, H5T_NATIVE_FLOAT);
      } else if (buffer->IsInt32Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int32Array>::Cast(buffer), options, H5T_NATIVE_INT);
      } else if (buffer->IsUint32Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint32Array>::Cast(buffer), options, H5T_NATIVE_UINT);
      } else if (buffer->IsInt16Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int16Array>::Cast(buffer), options, H5T_NATIVE_SHORT);
      } else if (buffer->IsUint16Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint16Array>::Cast(buffer), options, H5T_NATIVE_USHORT);
      } else if (buffer->IsInt8Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int8Array>::Cast(buffer), options, H5T_NATIVE_INT8);
      } else if (buffer->IsUint8Array()) {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint8Array>::Cast(buffer), options, H5T_NATIVE_UINT8);
      } else {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
      }
    }

    static void write_dataset(const v8::FunctionCallbackInfo<Value>& args) {

      // fail out if arguments are not correct
      if (args.Length() < 3 || args.Length() > 4 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value dset_name(args[1]->ToString());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank   = 1;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
      err = H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      bool bindAttributes = false;
      bool                       subsetOn = false;
      std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
      if (args.Length() == 4) {
          bindAttributes = is_bind_attributes(args,3);
          subsetOn=get_dimensions(args, 3, start, stride, count, rank);
      }
      bool bufferAsUnit8Array=true;
#if NODE_VERSION_AT_LEAST(8,0,0)
#else
      bufferAsUnit8Array=args[2]->ToObject()->IsUint8Array();
#endif
      if (bufferAsUnit8Array && node::Buffer::HasInstance(args[2])) {
        Local<Value>      encodingValue = args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding"));
        String::Utf8Value encoding(encodingValue->ToString());
        if (args[2]->ToObject()->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding")) && std::strcmp("binary", (*encoding))) {
          Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
          hid_t  did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
          herr_t err    = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)node::Buffer::Data(args[2]));
          if (err < 0) {
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Dclose(did);
          args.GetReturnValue().SetUndefined();
          return;
        }

        Int64* idWrap  = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        hid_t  did     = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
        hid_t  dataspace_id = H5S_ALL;
        hid_t  memspace_id  = H5S_ALL;
        hid_t type_id = H5Dget_type(did );
        if (subsetOn) {
          memspace_id  = H5Screate_simple(rank, count.get(), NULL);
          dataspace_id = H5Dget_space(did);
          herr_t err   = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
          if (err < 0) {
            if (subsetOn) {
              H5Sclose(memspace_id);
              H5Sclose(dataspace_id);
            }
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
          std::unique_ptr<hsize_t[]> maxdims(new hsize_t[rank]);
          H5Sget_simple_extent_dims(dataspace_id, dims.get(), maxdims.get());

          int remainingRows = dims[0] - (*start.get() + *count.get());
          if (remainingRows < 0) {
            dims[0] -= remainingRows;
            H5Dset_extent(did, dims.get());
            H5Sset_extent_simple(dataspace_id, rank, dims.get(), maxdims.get());
          }
        }
        herr_t err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, node::Buffer::Data(args[2]));
        if (err < 0) {
          if (subsetOn) {
            H5Sclose(memspace_id);
            H5Sclose(dataspace_id);
          }
          H5Dclose(did);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite dataset")));
          args.GetReturnValue().SetUndefined();
          return;
        }
        if (subsetOn) {
          H5Sclose(memspace_id);
          H5Sclose(dataspace_id);
        }
        H5Dclose(did);

        args.GetReturnValue().SetUndefined();
        return;
      }
      if (args[2]->IsString()) {
        String::Utf8Value buffer(args[2]->ToString());
        Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
        herr_t            err    = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)(*buffer));
        if (err < 0) {
          H5Dclose(did);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset")));
          args.GetReturnValue().SetUndefined();
          return;
        }
        H5Dclose(did);
        return;
      }
      hid_t type_id;
      if (args[2]->IsArray()) {
        write_dataset_from_array(args, Local<Array>::Cast(args[2]), subsetOn, start, stride, count);
        return;
      }
      Local<TypedArray> buffer;
      if (args[2]->IsFloat64Array()) {
        type_id = H5T_NATIVE_DOUBLE;
        buffer  = Local<Float64Array>::Cast(args[2]);
      } else if (args[2]->IsFloat32Array()) {
        type_id = H5T_NATIVE_FLOAT;
        buffer  = Local<Float32Array>::Cast(args[2]);
      } else if (args[2]->IsInt32Array()) {
        type_id = H5T_NATIVE_INT;
        buffer  = Local<Int32Array>::Cast(args[2]);
      } else if (args[2]->IsUint32Array()) {
        type_id = H5T_NATIVE_UINT;
        buffer  = Local<Uint32Array>::Cast(args[2]);
      } else if (args[2]->IsInt16Array()) {
        type_id = H5T_NATIVE_SHORT;
        buffer  = Local<Int16Array>::Cast(args[2]);
      } else if (args[2]->IsUint16Array()) {
        type_id = H5T_NATIVE_USHORT;
        buffer  = Local<Uint16Array>::Cast(args[2]);
      } else if (args[2]->IsInt8Array()) {
        type_id = H5T_NATIVE_INT8;
        buffer  = Local<Int8Array>::Cast(args[2]);
      } else if (args[2]->IsUint8Array()) {
        type_id = H5T_NATIVE_UINT8;
        buffer  = Local<Uint8Array>::Cast(args[2]);
      } else {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      hid_t   did          = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t   dataspace_id = H5S_ALL;
      hid_t   memspace_id  = H5S_ALL;
      hsize_t dims;
      hsize_t maxdims;
      H5Sget_simple_extent_dims(dataspace_id, &dims, &maxdims);
      const int remainingRows = dims - (*start.get() + *count.get());
      if (remainingRows < 0) {
        dims -= remainingRows;
        H5Dset_extent(did, &dims);
        H5Sset_extent_simple(dataspace_id, rank, &dims, &maxdims);
      }
      if (subsetOn) {
        memspace_id  = H5Screate_simple(rank, count.get(), NULL);
        dataspace_id = H5Dget_space(did);
        herr_t err   = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
        if (err < 0) {
          if (subsetOn) {
            H5Sclose(memspace_id);
            H5Sclose(dataspace_id);
          }
          H5Dclose(did);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
          args.GetReturnValue().SetUndefined();
          return;
        }
      }
#if NODE_VERSION_AT_LEAST(8,0,0)
      err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject()));
#else
      err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
      if (err < 0) {
        if (subsetOn) {
          H5Sclose(memspace_id);
          H5Sclose(dataspace_id);
        }
        H5Dclose(did);
        // H5Pclose(dcpl);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (subsetOn) {
        H5Sclose(memspace_id);
        H5Sclose(dataspace_id);
      }
      if(bindAttributes){
          
      }
      H5Dclose(did);

      args.GetReturnValue().SetUndefined();
    }

    static void write_dataset_from_array(const v8::FunctionCallbackInfo<Value>& args,
                                         Local<v8::Array>                       array,
                                         bool                                   subsetOn,
                                         std::unique_ptr<hsize_t[]>&            start,
                                         std::unique_ptr<hsize_t[]>&            stride,
                                         std::unique_ptr<hsize_t[]>&            count) {
      int               rank = 1;
      String::Utf8Value dset_name(args[1]->ToString());
      Int64*            idWrap       = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      hid_t             did          = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t             dataspace_id = H5Dget_space(did);
      hid_t             memspace_id  = H5S_ALL;
      hsize_t           dims;
      hsize_t           maxdims;
      herr_t            err;
      H5Sget_simple_extent_dims(dataspace_id, &dims, &maxdims);
      const int remainingRows = dims - (*start.get() + *count.get());
      if (remainingRows < 0) {
        dims -= remainingRows;
        H5Dset_extent(did, &dims);
        H5Sset_extent_simple(dataspace_id, rank, &dims, &maxdims);
      }
      if (subsetOn) {
        memspace_id = H5Screate_simple(rank, count.get(), NULL);
        herr_t err  = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
        if (err < 0) {
          if (subsetOn) {
            H5Sclose(memspace_id);
            H5Sclose(dataspace_id);
          }
          H5Dclose(did);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
          args.GetReturnValue().SetUndefined();
          return;
        }
      }
      hid_t type_id = H5Tcopy(H5T_C_S1);
      H5Tset_size(type_id, H5T_VARIABLE);
      const unsigned int                              arraySize = array->Length();
      std::unique_ptr<char* []>                       vl(new char*[arraySize]);
      std::vector<std::unique_ptr<String::Utf8Value>> string_values;

      for (unsigned int arrayIndex = 0; arrayIndex < arraySize; arrayIndex++) {
        std::unique_ptr<String::Utf8Value> value(new String::Utf8Value(array->Get(arrayIndex)));
        vl.get()[arrayIndex] = **value;
        string_values.emplace_back(std::move(value));
      }
      err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, vl.get());
      if (err < 0) {
        if (subsetOn) {
          H5Sclose(memspace_id);
          H5Sclose(dataspace_id);
        }
        H5Dclose(did);
        // H5Pclose(dcpl);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (subsetOn) {
        H5Sclose(memspace_id);
        H5Sclose(dataspace_id);
      }
      H5Dclose(did);

      args.GetReturnValue().SetUndefined();
    }

    static void read_dataset_length(const v8::FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      const String::Utf8Value dataset_name(args[1]->ToString());

      Int64*      idWrap      = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      const hid_t location_id = idWrap->Value();

      const hid_t dataset   = H5Dopen(location_id, *dataset_name, H5P_DEFAULT);
      const hid_t dataspace = H5Dget_space(dataset);

      const int rank = H5Sget_simple_extent_ndims(dataspace);

      std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> maxdims(new hsize_t[rank]);
      H5Sget_simple_extent_dims(dataspace, dims.get(), maxdims.get());

      args.GetReturnValue().Set(Int32::New(v8::Isolate::GetCurrent(), dims[0]));

      H5Sclose(dataspace);
      H5Dclose(dataset);
    }

    static void read_dataset_datatype(const v8::FunctionCallbackInfo<Value>& args) {
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      const String::Utf8Value dataset_name(args[1]->ToString());

      Int64*      idWrap      = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      const hid_t location_id = idWrap->Value();

      const hid_t dataset = H5Dopen(location_id, *dataset_name, H5P_DEFAULT);
      hid_t       t       = H5Dget_type(dataset);

      args.GetReturnValue().Set(Int32::New(v8::Isolate::GetCurrent(), t));

      H5Dclose(dataset);
    }

    static void read_dataset(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() == 4 && (!args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsFunction())) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, [options], callback")));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 3 && (!args[0]->IsObject() || !args[1]->IsString() || (!args[2]->IsObject() && !args[2]->IsFunction()))) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, callback[options]")));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 2 && (!args[0]->IsObject() || !args[1]->IsString())) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(args[1]->ToString());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      bool bindAttributes = false;
      bool                       subsetOn = false;
      std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
      if (args.Length() >= 3 && args[2]->IsObject()) {
        bindAttributes = is_bind_attributes(args, 2);
        subsetOn=get_dimensions(args, 2, start, stride, count, rank);
      }
      std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
      err = H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if(!subsetOn){
          for(int index=0;index<rank;index++){
              count.get()[index]=values_dim.get()[index];
          }
      }
      hsize_t theSize = bufSize;
      switch (rank) {
        case 4: theSize = values_dim.get()[0] * values_dim.get()[1] * values_dim.get()[2] * values_dim.get()[3]; break;
        case 3: theSize = values_dim.get()[0] * values_dim.get()[1] * values_dim.get()[2]; break;
        case 2: theSize = values_dim.get()[0] * values_dim.get()[1]; break;
        case 1: theSize = values_dim.get()[0]; break;
        case 0: theSize = bufSize; break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
          args.GetReturnValue().SetUndefined();
          return;
          break;
      }
      if(subsetOn){
            theSize = 1;
            for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
              theSize *= count.get()[rankIndex];
            }
      }
      
      switch (class_id) {
        case H5T_ARRAY: {
          hid_t                      did          = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
          hid_t                      t            = H5Dget_type(did);
          hid_t                      type_id      = H5Tget_native_type(t, H5T_DIR_ASCEND);
          hid_t                      dataspace_id = H5S_ALL;
          hid_t                      memspace_id  = H5S_ALL;
          hid_t                      basetype_id  = H5Tget_super(type_id);
          int                        arrayRank    = H5Tget_array_ndims(type_id);
          std::unique_ptr<hsize_t[]> arrayDims(new hsize_t[arrayRank]);
          H5Tget_array_dims(type_id, arrayDims.get());
          std::unique_ptr<char* []> vl(new char*[arrayDims.get()[0]]);
          if (!(H5Tis_variable_str(basetype_id)>0)) {
            size_t typeSize = H5Tget_size(basetype_id);
            for (unsigned int arrayIndex = 0; arrayIndex < arrayDims.get()[0]; arrayIndex++) {
              vl.get()[arrayIndex] = new char[typeSize + 1];
              std::memset(vl.get()[arrayIndex], 0, typeSize + 1);
            }
          }
          err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, vl.get());
          if (err < 0) {
            H5Tclose(t);
            H5Tclose(type_id);
            H5Tclose(basetype_id);
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          if(*vl.get() == nullptr) {
            args.GetReturnValue().SetUndefined();
            return;
          }
          hsize_t arrayStart=0;
          hsize_t arrayMaximum=values_dim.get()[0];
          if(subsetOn){
            arrayStart=start.get()[0];
            arrayMaximum=std::min(values_dim.get()[0], arrayStart+count.get()[0]);
          }
          Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], count.get()[0]));
          for (unsigned int arrayIndex = arrayStart; arrayIndex < arrayMaximum; arrayIndex++) {
          //std::string s(vl.get()[arrayIndex]);
          array->Set(arrayIndex-arrayStart,
                      String::NewFromUtf8(
                          v8::Isolate::GetCurrent(), vl.get()[arrayIndex], String::kNormalString, std::strlen(vl.get()[arrayIndex])));
          }
          args.GetReturnValue().Set(array);
          H5Tclose(t);
          H5Tclose(type_id);
          H5Tclose(basetype_id);
          H5Dclose(did);
        } break;
        case H5T_STRING: {
          hid_t did     = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
          hid_t t       = H5Dget_type(did);
          hid_t type_id = H5Tget_native_type(t, H5T_DIR_ASCEND);
          if (H5Tis_variable_str(type_id)>0) {
            hid_t                     dataspace_id = H5S_ALL;
            hid_t                     memspace_id  = H5S_ALL;
            hid_t                     basetype_id  = H5Tget_super(type_id);
            if(subsetOn){
              std::unique_ptr<hsize_t[]> maxsize(new hsize_t[rank]);
              for(int rankIndex=0;rankIndex<rank;rankIndex++)
                  maxsize[rankIndex]=H5S_UNLIMITED;
              memspace_id           = H5Screate_simple(rank, count.get(), maxsize.get());
              dataspace_id          = H5Dget_space(did);
              herr_t err            = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
              if (err < 0) {
                if (subsetOn) {
                  H5Sclose(memspace_id);
                  H5Sclose(dataspace_id);
                }
                H5Dclose(did);
                v8::Isolate::GetCurrent()->ThrowException(
                    v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
                args.GetReturnValue().SetUndefined();
                return;
              }
              theSize = 1;
              for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
                theSize *= count.get()[rankIndex];
              }
            }
            std::unique_ptr<char* []> tbuffer(new char*[std::min(values_dim.get()[0], theSize)]);
            err=H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, tbuffer.get());
            Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], theSize));
            for (unsigned int arrayIndex = 0; arrayIndex < std::min(values_dim.get()[0], theSize); arrayIndex++) {
              //std::string s(tbuffer.get()[arrayIndex]);
              array->Set(
                  arrayIndex,
                  String::NewFromUtf8(
                      v8::Isolate::GetCurrent(), tbuffer.get()[arrayIndex], String::kNormalString, std::strlen(tbuffer.get()[arrayIndex])));
            }
            args.GetReturnValue().Set(array);
            H5Tclose(basetype_id);
          } else if (rank > 1 && values_dim.get()[0] > 0) {
            hid_t                   dataspace_id = H5S_ALL;
            hid_t                   memspace_id  = H5S_ALL;
            size_t                  typeSize     = H5Tget_size(t);
            std::unique_ptr<char[]> tbuffer(new char[typeSize * theSize]);
            size_t                  nalloc;
            H5Tencode(type_id, NULL, &nalloc);
            H5Tencode(type_id, tbuffer.get(), &nalloc);
            H5T_str_t paddingType=H5Tget_strpad(type_id);
            err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, tbuffer.get());
            if (err < 0) {
              H5Tclose(t);
              H5Tclose(type_id);
              H5Dclose(did);
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset")));
              args.GetReturnValue().SetUndefined();
              return;
            }
            hsize_t arrayStart=0;
            hsize_t arrayMaximum=values_dim.get()[0];
            hsize_t realLength=0;
            if(subsetOn){
              arrayStart=start.get()[0];
              arrayMaximum=std::min(values_dim.get()[0], arrayStart+count.get()[0]);
            }
            Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], count.get()[0]));
            fill_multi_array(array, tbuffer, values_dim, count, typeSize, arrayStart, 0, rank, paddingType);
            args.GetReturnValue().Set(array);
          } else {
            std::string buffer(bufSize*theSize + 1, 0);
            err = H5LTread_dataset_string(idWrap->Value(), *dset_name, (char*)buffer.c_str());
            if (err < 0) {
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
              args.GetReturnValue().SetUndefined();
              return;
            }

            args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
          }
          H5Tclose(t);
          H5Tclose(type_id);
          H5Dclose(did);
        } break;
        case H5T_VLEN:{
            hid_t                   dataspace_id = H5S_ALL;
            hid_t                   memspace_id  = H5S_ALL;
           hid_t did                  = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
           hid_t type_id                    = H5Dget_type(did);
            hsize_t arrayStart=0;
            hsize_t arrayMaximum=values_dim.get()[0];
            //hsize_t realLength=0;
            if(subsetOn){
              arrayStart=start.get()[0];
              arrayMaximum=std::min(values_dim.get()[0], arrayStart+count.get()[0]);
            }
            std::unique_ptr<hvl_t[]> vl(new hvl_t[arrayMaximum]);
          herr_t err=H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (void*)vl.get());
          if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to vlen read dataset")));
            args.GetReturnValue().SetUndefined();
          }
          else{
          hid_t super_type = H5Tget_super(type_id);
          hsize_t typeSize = H5Tget_size(super_type);
          Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], count.get()[0]));
          for (unsigned int arrayIndex = arrayStart; arrayIndex < arrayMaximum; arrayIndex++) {
            if(H5Tis_variable_str(super_type)){
              std::string s((char*)vl.get()[arrayIndex].p);
              array->Set(arrayIndex-arrayStart,
                          String::NewFromUtf8(
                              v8::Isolate::GetCurrent(), (char*)vl.get()[arrayIndex].p, String::kNormalString, vl.get()[arrayIndex].len));
            }
            else if(typeSize==1){
          Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), vl.get()[arrayIndex].p, typeSize * vl.get()[arrayIndex].len);
              array->Set(arrayIndex-arrayStart,
                          v8::Uint8Array::New(arrayBuffer, 0, vl.get()[arrayIndex].len));
              
            }
          }
          args.GetReturnValue().Set(array);
          }
            
            H5Tclose(type_id);
            H5Dclose(did);
          
        } break;
        default:

          hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);;
          hid_t type_id;
          Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), bufSize * theSize);
          Local<TypedArray>  buffer;
          if (class_id == H5T_FLOAT && bufSize == 8) {
            type_id = H5T_NATIVE_DOUBLE;
            buffer  = Float64Array::New(arrayBuffer, 0, theSize);
          } else if (class_id == H5T_FLOAT && bufSize == 4) {
            type_id = H5T_NATIVE_FLOAT;
            buffer  = Float32Array::New(arrayBuffer, 0, theSize);
          } else if (class_id == H5T_INTEGER && bufSize == 8) {

            type_id                    = H5Dget_type(did);
            Handle<Object> int64Buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
            H5LTread_dataset(idWrap->Value(), *dset_name, type_id, (char*)node::Buffer::Data(int64Buffer));

            hid_t native_type_id = H5Tget_native_type(type_id, H5T_DIR_ASCEND);
            if (H5Tequal(H5T_NATIVE_LLONG, native_type_id)) {
              int64Buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"),
                               Int32::New(v8::Isolate::GetCurrent(), toEnumMap[H5T_NATIVE_LLONG]));
            }
            args.GetReturnValue().Set(int64Buffer);
            H5Tclose(native_type_id);
            return;

          } else if (class_id == H5T_INTEGER && bufSize == 4) {
            hid_t t = H5Dget_type(did);
            if (H5Tget_sign(H5Dget_type(did)) == H5T_SGN_2) {
              type_id = H5T_NATIVE_INT;
              buffer  = Int32Array::New(arrayBuffer, 0, theSize);
            } else {
              type_id = H5T_NATIVE_UINT;
              buffer  = Uint32Array::New(arrayBuffer, 0, theSize);
            }
            H5Tclose(t);
          } else if ((class_id == H5T_INTEGER || class_id == H5T_ENUM) && bufSize == 2) {
            hid_t t = H5Dget_type(did);
            if (H5Tget_sign(H5Dget_type(did)) == H5T_SGN_2) {
              type_id = H5T_NATIVE_SHORT;
              buffer  = Int16Array::New(arrayBuffer, 0, theSize);
            } else {
              type_id = H5T_NATIVE_USHORT;
              buffer  = Uint16Array::New(arrayBuffer, 0, theSize);
            }
            H5Tclose(t);
          } else if (class_id == H5T_INTEGER && bufSize == 1) {
            hid_t t = H5Dget_type(did);
            if (H5Tget_sign(H5Dget_type(did)) == H5T_SGN_2) {
              type_id = H5T_NATIVE_INT8;
              buffer  = Int8Array::New(arrayBuffer, 0, theSize);
            } else {
              type_id = H5T_NATIVE_UINT8;
              buffer  = Uint8Array::New(arrayBuffer, 0, theSize);
            }
            H5Tclose(t);
          } else {
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type in reading")));
            args.GetReturnValue().SetUndefined();
            return;
          }

          hid_t dataspace_id = H5S_ALL;
          hid_t memspace_id  = H5S_ALL;
          if (subsetOn) {
              std::unique_ptr<hsize_t[]> maxsize(new hsize_t[rank]);
              for(int rankIndex=0;rankIndex<rank;rankIndex++)
                  maxsize[rankIndex]=H5S_UNLIMITED;
            memspace_id           = H5Screate_simple(rank, count.get(), maxsize.get());
            dataspace_id          = H5Dget_space(did);
            herr_t err            = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
            if (err < 0) {
              if (subsetOn) {
                H5Sclose(memspace_id);
                H5Sclose(dataspace_id);
              }
              H5Dclose(did);
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
              args.GetReturnValue().SetUndefined();
              return;
            }
          }
#if NODE_VERSION_AT_LEAST(8,0,0)
          //v8::Local<v8::Object> buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
          err                          = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)node::Buffer::Data(buffer->ToObject()));
#else
          //v8::Local<v8::Object> buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
          err                          = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)buffer->Buffer()->Externalize().Data());
#endif
          if (err < 0) {
            if (subsetOn) {
              H5Sclose(memspace_id);
              H5Sclose(dataspace_id);
            }
            H5Tclose( type_id);
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          if (subsetOn) {
            H5Sclose(memspace_id);
            H5Sclose(dataspace_id);
            for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
              values_dim.get()[rankIndex]= count.get()[rankIndex]/stride.get()[rankIndex];
            }
          }
          if ((args.Length() == 3 && args[2]->IsFunction()) || (args.Length() == 4 && args[3]->IsFunction())) {
            const unsigned               argc = 1;
            v8::Persistent<v8::Function> callback(v8::Isolate::GetCurrent(), args[args.Length()-1].As<Function>());
            v8::Local<v8::Object> options = v8::Object::New(v8::Isolate::GetCurrent());
            options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
            H5T_order_t order = H5Tget_order(type_id);
            options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "endian"), Number::New(v8::Isolate::GetCurrent(), order));
            switch (rank) {
              case 4:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
              case 3:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
              case 2:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                break;
              case 1:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
            }
            if(class_id == H5T_ENUM){
                v8::Local<v8::Object> enumeration = v8::Object::New(v8::Isolate::GetCurrent());

                hid_t h = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
                hid_t t = H5Dget_type(h);
                int n=H5Tget_nmembers( t );
                for(unsigned int i=0;i<(unsigned int)n;i++){
                    char * mname=H5Tget_member_name( t, i );
                    int idx=H5Tget_member_index(t, (const char *) mname );
                    unsigned int value;
                    H5Tget_member_value( t, idx, (void *)&value );
                    hsize_t dvalue=value;
                    enumeration->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), mname), Number::New(v8::Isolate::GetCurrent(), dvalue));                    
#if  H5_VERSION_GE(1,8,13)
                    H5free_memory((void *)mname);
#endif
                }
                H5Tclose(t);
                H5Dclose(h);
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "enumeration"), enumeration);
                
            }
            v8::Local<v8::Value> argv[1] = {options};
            v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
                ->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
          } else{
            buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
            switch (rank) {
            case 4:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 3:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 2:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              if (rank > 1)
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                            Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              break;
            case 1:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
          }
          }
          if(bindAttributes){

            v8::Local<v8::Object> focus=buffer->ToObject();
            refreshAttributes(focus, did);
          }
          H5Dclose(did);
          args.GetReturnValue().Set(buffer);
          break;
      }
    }
    static void readDatasetAsBuffer(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() == 4 && (!args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsFunction())) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, [options], callback")));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 3 && (!args[0]->IsObject() || !args[1]->IsString() || (!args[2]->IsObject() && !args[2]->IsFunction()))) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, callback[options]")));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 2 && (!args[0]->IsObject() || !args[1]->IsString())) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(args[1]->ToString());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
      err = H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      hsize_t                    theSize  = bufSize;
      bool bindAttributes = false;
      bool                       subsetOn = false;
      std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
      if (args.Length() >= 3 && args[2]->IsObject()) {
        bindAttributes = is_bind_attributes(args, 2);
        subsetOn=get_dimensions(args, 2, start, stride, count, rank);
      }
      if(!subsetOn){
          for(int index=0;index<rank;index++){
              count.get()[index]=values_dim.get()[index];
          }
      }
      switch (rank) {
        case 4: theSize = values_dim.get()[0] * values_dim.get()[1] * values_dim.get()[2] * values_dim.get()[3]; break;
        case 3: theSize = values_dim.get()[0] * values_dim.get()[1] * values_dim.get()[2]; break;
        case 2: theSize = values_dim.get()[0] * values_dim.get()[1]; break;
        case 1: theSize = values_dim.get()[0]; break;
        case 0: theSize = bufSize; break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
          args.GetReturnValue().SetUndefined();
          return;
          break;
      }
      switch (class_id) {
        case H5T_STRING: {
          std::string buffer(theSize + 1, 0);
          err = H5LTread_dataset_string(idWrap->Value(), *dset_name, (char*)buffer.c_str());
          if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          args.GetReturnValue().Set(node::Buffer::New(v8::Isolate::GetCurrent(), (char*)buffer.c_str(), theSize).ToLocalChecked());

        } break;
        case H5T_INTEGER:
        case H5T_FLOAT:
        case H5T_BITFIELD: {

          hid_t did          = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
          hid_t  type_id            = H5Dget_type(did);
          hid_t dataspace_id = H5S_ALL;
          hid_t memspace_id  = H5S_ALL;
          if (subsetOn) {
              std::unique_ptr<hsize_t[]> maxsize(new hsize_t[rank]);
              for(int rankIndex=0;rankIndex<rank;rankIndex++)
                  maxsize[rankIndex]=H5S_UNLIMITED;
            memspace_id           = H5Screate_simple(rank, count.get(), maxsize.get());
            dataspace_id          = H5Dget_space(did);
            herr_t err            = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
            if (err < 0) {
              if (subsetOn) {
                H5Sclose(memspace_id);
                H5Sclose(dataspace_id);
              }
              H5Dclose(did);
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
              args.GetReturnValue().SetUndefined();
              return;
            }
            theSize = 1;
            for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
              theSize *= count.get()[rankIndex];
            }
          }
          v8::Local<v8::Object> buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
          err                          = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)node::Buffer::Data(buffer));
          if (err < 0) {
            if (subsetOn) {
              H5Sclose(memspace_id);
              H5Sclose(dataspace_id);
            }
            H5Tclose( type_id);
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
            args.GetReturnValue().SetUndefined();
            return;
          }
          bool hit   = false;
          H5T  etype = NODE_H5T_UNKNOWN;
          for (std::map<H5T, hid_t>::iterator it = toTypeMap.begin(); !hit && it != toTypeMap.end(); it++) {

            if (H5Tequal(type_id, toTypeMap[(*it).first])) {
              etype = (*it).first;
              hit   = true;
            }
          }

          buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"), Int32::New(v8::Isolate::GetCurrent(), etype));
          if (subsetOn) {
            H5Sclose(memspace_id);
            H5Sclose(dataspace_id);
            for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
              values_dim.get()[rankIndex]= count.get()[rankIndex]/stride.get()[rankIndex];
            }
          }
          if ((args.Length() == 3 && args[2]->IsFunction()) || (args.Length() == 4 && args[3]->IsFunction())) {
            const unsigned               argc = 1;
            v8::Persistent<v8::Function> callback(v8::Isolate::GetCurrent(), args[args.Length()-1].As<Function>());
            v8::Local<v8::Object> options = v8::Object::New(v8::Isolate::GetCurrent());
            options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
            H5T_order_t order = H5Tget_order(type_id);
            options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "endian"), Number::New(v8::Isolate::GetCurrent(), order));
            switch (rank) {
              case 4:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
              case 3:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
              case 2:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                break;
              case 1:
                options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                break;
            }
            v8::Local<v8::Value> argv[1] = {options};
            v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
                ->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
          } else{
            buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
            switch (rank) {
            case 4:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 3:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 2:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              if (rank > 1)
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"),
                            Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              break;
            case 1:
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
          }
          }
          H5Tclose( type_id);
          if(bindAttributes){
            v8::Local<v8::Object> focus=buffer->ToObject();
            // Attributes
            refreshAttributes(focus, did);
          }
          H5Dclose(did);


          args.GetReturnValue().Set(buffer);
        } break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
          args.GetReturnValue().SetUndefined();
          break;
      }
    }
  };
}
