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
    static void Initialize(v8::Local<v8::Object> exports) {
      v8::Isolate* isolate = exports->GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // append this function to the target object
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeDataset", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::make_dataset)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeDataset", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::write_dataset)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetLength", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset_length)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetDatatype", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset_datatype)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDataset", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetAsBuffer", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::readDatasetAsBuffer)->GetFunction(context).ToLocalChecked()).Check();
    }

    inline static bool is_bind_attributes(const v8::FunctionCallbackInfo<Value>& args, unsigned int argIndex){
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
        bool bindAttributes=false;
        Local<Array> names = args[argIndex]->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
        for (uint32_t index = 0; index < names->Length(); index++) {
          String::Utf8Value _name(isolate, names->Get(context, index).ToLocalChecked());
          std::string       name(*_name);
          if (name.compare("bind_attributes") == 0) {
            bindAttributes = args[argIndex]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToBoolean(isolate)->Value();
          }
        }
        return bindAttributes;
    }

    inline static bool get_dimensions(const v8::FunctionCallbackInfo<Value>& args, unsigned int argIndex, std::unique_ptr<hsize_t[]>& start, std::unique_ptr<hsize_t[]>& stride, std::unique_ptr<hsize_t[]>& count, int rank){
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
        bool subsetOn=false;
        bool gotStart=false;
        bool gotStride=false;
        bool gotCount=false;
        unsigned int size=0;
        Local<Array> names = args[argIndex]->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
        for (uint32_t index = 0; index < names->Length(); index++) {
          String::Utf8Value _name(isolate, names->Get(context, index).ToLocalChecked());
          std::string       name(*_name);
          if (name.compare("start") == 0) {
            Local<Object> starts = args[argIndex]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < starts->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->Uint32Value(context).ToChecked();
                 arrayIndex++) {
              start.get()[arrayIndex] = starts->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
            }
            gotStart=true;
          } else if (name.compare("stride") == 0) {
            Local<Object> strides = args[argIndex]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < strides->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->Uint32Value(context).ToChecked();
                 arrayIndex++) {
              stride.get()[arrayIndex] = strides->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
            }
            gotStride=true;
          } else if (name.compare("count") == 0) {
            Local<Object> counts = args[argIndex]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
            for (unsigned int arrayIndex = 0;
                 arrayIndex < counts->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->Uint32Value(context).ToChecked();
                 arrayIndex++) {
              count.get()[arrayIndex] = counts->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
              size++;
            }
            gotCount=true;
            subsetOn = true;
          }
        }
        if(!gotStart && !gotStride && !gotCount)
          return false;
        if(!gotCount)v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Need to supply the subspace count dimensions. Start and stride are optional.", v8::NewStringType::kNormal).ToLocalChecked()));
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

    /*inline static int get_array_rank(v8::Local<v8::Array> array){
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
    }*/
    
    /*inline static void get_array_dimensions(v8::Local<v8::Array> array, std::unique_ptr<hsize_t[]>& dims, int rank){
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
    }*/

    /*static void fill_buffer_from_multi_array(v8::Local<v8::Array> array, std::unique_ptr<char[]>& vl, unsigned int fixedWidth, unsigned int& index, int rank){
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
    }*/
    
    /*static void fill_multi_array(v8::Local<v8::Array>& array, std::unique_ptr<char[]>& tbuffer, std::unique_ptr<hsize_t[]>& dims, std::unique_ptr<hsize_t[]>& count, size_t fixedWidth, hsize_t& index, int depth, int rank, H5T_str_t paddingType){

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
    }*/
    
    inline static int get_array_rank(v8::Local<v8::Array> array){
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
        int rank=1;
        v8::Local<v8::Array> arrayCheck=array;
        bool look=true;
        while(look){
        v8::Local<v8::Array> names = arrayCheck->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
        bool hit=false;
        for (uint32_t index = 0; !hit && index < names->Length(); index++) {
          if (arrayCheck->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->IsArray()) {
              arrayCheck=v8::Local<v8::Array>::Cast(arrayCheck->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
              hit=true;
              rank++;
          }
        }
        if(!hit)look=false;
        }
        return rank;
    }
    
    inline static void get_array_dimensions(v8::Local<v8::Array> array, std::unique_ptr<hsize_t[]>& dims, int rank){
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Array> arrayCheck=array;
        int count=0;
        dims[count++]=arrayCheck->Length();
        bool look=true;
        while(look && count<rank){
        v8::Local<v8::Array> names = arrayCheck->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
        bool hit=false;
        for (uint32_t index = 0; !hit && index < names->Length(); index++) {
          if (arrayCheck->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->IsArray()) {
            arrayCheck=v8::Local<v8::Array>::Cast(arrayCheck->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
            hit=true;
            dims[count++]=arrayCheck->Length();
          }
        }
        if(!hit)look=false;
        }
    }

    static void fill_buffer_from_multi_array(v8::Local<v8::Array> array, std::unique_ptr<char[]>& vl, unsigned int fixedWidth, unsigned int& index, int rank){
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Array> names = array->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
        for (uint32_t arrayIndex = 0; arrayIndex < names->Length(); arrayIndex++) {
          if (array->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, arrayIndex).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->IsArray()) {
            v8::Local<v8::Array> arrayCheck=Local<v8::Array>::Cast(array->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, arrayIndex).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
            fill_buffer_from_multi_array(arrayCheck, vl, fixedWidth, index, rank);
          }
          else{
          std::string s;
          v8::String::Utf8Value buffer(isolate, array->Get(context, arrayIndex).ToLocalChecked()->ToString(context).ToLocalChecked());
          s.assign(*buffer);
          if (fixedWidth < s.length()) {
              throw Exception("failed fixed width was too small: "+std::to_string(fixedWidth));
          }
          std::strncpy(&vl.get()[fixedWidth * index], s.c_str(), s.length());
          index++;
              
          }
        }
    }
    
    static void fill_multi_array(v8::Local<v8::Array>& array, std::unique_ptr<char[]>& tbuffer, std::unique_ptr<hsize_t[]>& dims, std::unique_ptr<hsize_t[]>& count, size_t fixedWidth, hsize_t& index, int depth, int rank, H5T_str_t paddingType){
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

        for (uint32_t arrayIndex = 0; arrayIndex < std::min(dims.get()[depth], count.get()[depth]); arrayIndex++) {
          if (depth<rank-1) {
            v8::Local<v8::Array> arrayCheck=v8::Array::New(v8::Isolate::GetCurrent(), std::min(dims.get()[depth], count.get()[depth]));
            int ldepth=depth+1;
            fill_multi_array(arrayCheck, tbuffer, dims, count, fixedWidth, index, ldepth, rank, paddingType);
            array->Set(context, arrayIndex, arrayCheck).Check();
          }
          else{
              hsize_t realLength=0;
            char delimiter=(paddingType==H5T_STR_SPACEPAD) ? ' ' : 0;
              while(realLength<fixedWidth && ((char)tbuffer.get()[fixedWidth * index+realLength])!=delimiter){
                realLength++;
              }
              array->Set(context, arrayIndex,
                         v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                             &tbuffer.get()[fixedWidth * index],
                                             v8::NewStringType::kNormal,
                                             realLength).ToLocalChecked()).Check();
              index++;
              
          }
        }
    }
    
    inline static unsigned int get_fixed_width(v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return 0;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "fixed_width", v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return 0;
      }

      return options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
    }

    inline static H5T_str_t get_padding_type(v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return H5T_STR_NULLTERM;
      }

      auto name(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "padding", v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return H5T_STR_NULLTERM;
      }

      return (H5T_str_t)options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
    }

    /*inline static H5T_str_t get_padding_type(v8::Local<v8::Object> options) {
      if (options.IsEmpty()) {
        return H5T_STR_NULLTERM;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "padding"));

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return H5T_STR_NULLTERM;
      }

      return (H5T_str_t)options->Get(name)->Uint32Value();
    }*/

    static void get_type(v8::Local<v8::Object> options, std::function<void(hid_t)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(toTypeMap[(H5T)options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked()]);
      }
    }
    
    static void get_rank(v8::Local<v8::Object> options, std::function<void(int)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked());
      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_rows(v8::Local<v8::Object> options, std::function<void(int)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_columns(v8::Local<v8::Object> options, std::function<void(int)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_sections(v8::Local<v8::Object> options, std::function<void(int)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_files(v8::Local<v8::Object> options, std::function<void(int)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb(options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static unsigned int get_compression(v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return 0;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "compression", v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return 0;
      }

      return options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
    }

    static int get_option_int(v8::Local<v8::Object> options,const char * option_name, int default_value) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return default_value;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), option_name, v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return default_value;
      }

      return options->Get(context, name).ToLocalChecked()->Int32Value(context).ToChecked();
    }

    static std::unique_ptr<hsize_t[]> get_chunk_size(v8::Local<v8::Object> options, int rank) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);  
      for(int index=0;index<rank;index++){
        dims[index]=0;
      }
      if (options.IsEmpty()) {
        return dims;
      }

      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "chunkSize", v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return dims;
      }
      if(options->Get(context, name).ToLocalChecked()->IsArray()){
          v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(options->Get(context, name).ToLocalChecked());
        for (unsigned int arrayIndex = 0; arrayIndex < std::min((uint32_t)rank, array->Length()); arrayIndex++) {
           dims.get()[arrayIndex]=array->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
        }
          
      }
      else {
        for(int index=0;index<rank;index++){
          dims.get()[index]=options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout", v8::NewStringType::kInternalized).ToLocalChecked()));
          return false;
        }

        return true;
      }

      err = H5Pset_chunk(dcpl, rank, chunk_dims.get());
      if (err) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout", v8::NewStringType::kInternalized).ToLocalChecked()));
        return false;
      }

      return true;
    }

    static bool configure_compression(const hid_t& dcpl, const unsigned int& compression) {
      herr_t err = H5Pset_deflate(dcpl, compression);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter", v8::NewStringType::kInternalized).ToLocalChecked()));
        return false;
      }

      return true;
    }

    static void make_dataset_from_buffer(const hid_t& group_id, const char* dset_name, v8::Local<v8::Object> buffer, v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      Local<Value>      encodingValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked();
      String::Utf8Value encoding(isolate, encodingValue->ToString(context).ToLocalChecked());
      if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked() && std::strcmp("binary", (*encoding))) {
        herr_t err = H5LTmake_dataset_string(group_id, dset_name, const_cast<char*>(node::Buffer::Data(buffer)));
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
        if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
          type_id = toTypeMap[(H5T)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked()];
        }
        else get_type(options, [&](hid_t _type_id){type_id=_type_id;});
      }
      int   rank    = 1;
      if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        Local<Value> rankValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked();
        rank                   = rankValue->Int32Value(context).ToChecked();
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
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[0] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_files(options, [&](int sections){dims.get()[0]=sections;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[1] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_sections(options, [&](int sections){dims.get()[1]=sections;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[3] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_columns(options, [&](int columns){dims.get()[3]=columns;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[2] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_rows(options, [&](int rows){dims.get()[2]=rows;});
          }
          maxdims.get()[0] = get_option_int(options,"maxFiles",dims.get()[0]);
          maxdims.get()[1] = get_option_int(options,"maxSections",dims.get()[1]);
          maxdims.get()[3] = get_option_int(options,"maxColumns",dims.get()[3]);
          maxdims.get()[2] = get_option_int(options,"maxRows",dims.get()[2]);
          break;        
        case 3: 
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[0] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_sections(options, [&](int sections){dims.get()[0]=sections;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[2] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_columns(options, [&](int columns){dims.get()[2]=columns;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[1] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_rows(options, [&](int rows){dims.get()[1]=rows;});
          }
          maxdims.get()[0] = get_option_int(options,"maxSections",dims.get()[0]);
          maxdims.get()[2] = get_option_int(options,"maxColumns",dims.get()[2]);
          maxdims.get()[1] = get_option_int(options,"maxRows",dims.get()[1]);
          break;
        case 2:
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[1] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_columns(options, [&](int columns){dims.get()[1]=columns;});
          }
          if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
            dims.get()[0] = (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
          }else{
            get_rows(options, [&](int rows){dims.get()[0]=rows;});
          }
          maxdims.get()[1] = get_option_int(options,"maxColumns",dims.get()[1]);
          maxdims.get()[0] = get_option_int(options,"maxRows",dims.get()[0]);
          break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank", v8::NewStringType::kInternalized).ToLocalChecked()));
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
        return;
      }
      H5Pclose(dcpl);

      // Atributes
      v8::Local<v8::Array> propertyNames = buffer->GetPropertyNames(context).ToLocalChecked();
      if(propertyStartIndex==0)propertyStartIndex=propertyNames->Length();
      for (hsize_t index = propertyStartIndex; index < propertyNames->Length(); index++) {
        v8::Local<v8::Value> name = propertyNames->Get(context, index).ToLocalChecked();
        if (!buffer->Get(context, name).ToLocalChecked()->IsFunction() && !buffer->Get(context, name).ToLocalChecked()->IsArray() &&
            strncmp("id", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 2) != 0 &&
            strncmp("rank", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 4) != 0 &&
            strncmp("rows", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 4) != 0 &&
            strncmp("columns", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 7) != 0 &&
            strncmp("buffer", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 6) != 0) {

          if (buffer->Get(context, name).ToLocalChecked()->IsObject() || buffer->Get(context, name).ToLocalChecked()->IsExternal()) {
          } else if (buffer->Get(context, name).ToLocalChecked()->IsUint32()) {
            uint32_t value = buffer->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (unsigned int*)&value, 1);

          } else if (buffer->Get(context, name).ToLocalChecked()->IsInt32()) {
            int32_t value = buffer->Get(context, name).ToLocalChecked()->Int32Value(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (int*)&value, 1);

          } else if (buffer->Get(context, name).ToLocalChecked()->IsString()) {
            std::string value((*String::Utf8Value(isolate, buffer->Get(context, name).ToLocalChecked()->ToString(context).ToLocalChecked())));
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (const char*)value.c_str());
          } else if (buffer->Get(context, name).ToLocalChecked()->IsNumber()) {
            double value = buffer->Get(context, name).ToLocalChecked()->NumberValue(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (double*)&value, 1);
          }
        }
      }
    }

    static void make_dataset_from_typed_array(
        const hid_t& group_id, const char* dset_name, v8::Local<v8::TypedArray> buffer, v8::Local<v8::Object> options, hid_t type_id) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      int rank = 1;
      get_rank(options, [&](int _rank){rank=_rank;});
      if (buffer->Has(context, String::NewFromUtf8(isolate, "rank", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        Local<Value> rankValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked();
        rank                   = rankValue->Int32Value(context).ToChecked();
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
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
          return;
        }
        H5Pclose(dcpl);
      } else if (rank == 2) {
          
        hsize_t dims[2];
        if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
          dims[0]= (hsize_t)buffer->Get(context, String::NewFromUtf8(isolate, "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
        }else get_rows(options, [&](int rows){dims[0]=rows;});
        if (buffer->Has(context, String::NewFromUtf8(isolate, "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        dims[1]= (hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
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
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
          return;
        }
        H5Pclose(dcpl);
      } else if (rank == 3) {
        hsize_t dims[3];
        if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
          dims[0]=(hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
        }else get_rows(options, [&](int rows){dims[0]=rows;});
        if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
          dims[1]=(hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
        }else get_columns(options, [&](int columns){dims[1]=columns;});
        if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
          dims[2]=(hsize_t)buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Int32Value(context).ToChecked();
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
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
#else
            group_id, dset_name, rank, dims, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
#endif
        if (err < 0) {
          H5Pclose(dcpl);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
          return;
        }
        H5Pclose(dcpl);
      } else {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank", v8::NewStringType::kInternalized).ToLocalChecked()));
        return;
      }

      // Atributes
      v8::Local<v8::Array> propertyNames = buffer->GetPropertyNames(context).ToLocalChecked();
      for (unsigned int index = buffer->Length(); index < propertyNames->Length(); index++) {
        v8::Local<v8::Value> name = propertyNames->Get(context, index).ToLocalChecked();
        if (!buffer->Get(context, name).ToLocalChecked()->IsFunction() && !buffer->Get(context, name).ToLocalChecked()->IsArray() &&
            strncmp("id", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 2) != 0 &&
            strncmp("rank", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 4) != 0 &&
            strncmp("rows", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 4) != 0 &&
            strncmp("columns", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 7) != 0 &&
            strncmp("buffer", (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), 6) != 0) {
          if (buffer->Get(context, name).ToLocalChecked()->IsObject()){
            std::string constructorName = "Reference";
            if (constructorName.compare(*String::Utf8Value(isolate, buffer->Get(context, name).ToLocalChecked()->ToObject(context).ToLocalChecked()->GetConstructorName())) == 0) {
                v8::Local<v8::Object> obj=buffer->Get(context, name).ToLocalChecked()->ToObject(context).ToLocalChecked();
            // unwrap ref
            Reference* ref =  ObjectWrap::Unwrap<Reference>(obj);
            hid_t attr_type  = H5Tcopy(H5T_STD_REF_OBJ);
            std::unique_ptr<hsize_t[]> currentDims(new hsize_t[1]);
            currentDims.get()[0] = 1;
            hid_t attr_space     = H5Screate_simple(1, currentDims.get(), NULL);
//            hid_t attr_space = H5Screate(H5S_SCALAR);
            hid_t  did = H5Dopen(group_id, dset_name, H5P_DEFAULT);
            hid_t attr_id = H5Acreate(did, *v8::String::Utf8Value(isolate, name->ToString(context).ToLocalChecked()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
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
          else if( buffer->Get(context, name).ToLocalChecked()->IsExternal()) {

          } else if (buffer->Get(context, name).ToLocalChecked()->IsUint32()) {
            uint32_t value = buffer->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (unsigned int*)&value, 1);

          } else if (buffer->Get(context, name).ToLocalChecked()->IsInt32()) {
            int32_t value = buffer->Get(context, name).ToLocalChecked()->Int32Value(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (int*)&value, 1);

          } else if (buffer->Get(context, name).ToLocalChecked()->IsString()) {
            std::string value((*String::Utf8Value(isolate, buffer->Get(context, name).ToLocalChecked()->ToString(context).ToLocalChecked())));
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }

            H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (const char*)value.c_str());
          } else if (buffer->Get(context, name).ToLocalChecked()->IsNumber()) {
            double value = buffer->Get(context, name).ToLocalChecked()->NumberValue(context).ToChecked();
            if (H5Aexists_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT) > 0) {
              H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), H5P_DEFAULT);
            }
            H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(isolate, name->ToString(context).ToLocalChecked())), (double*)&value, 1);
          }
        }
      }
    }

    static void make_dataset_from_array(const hid_t& group_id, const char* dset_name, v8::Local<v8::Array> array, v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
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
          v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what(), v8::NewStringType::kInternalized).ToLocalChecked()));
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
          return;
        }

        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
      } else if(hasOptionType){
        std::unique_ptr<hvl_t[]> vl(new hvl_t[array->Length()]);
        for (unsigned int arrayIndex = 0; arrayIndex < array->Length(); arrayIndex++) {
          char* buffer = (char*)node::Buffer::Data(array->Get(context, arrayIndex).ToLocalChecked());
           hsize_t length=Local<v8::Int8Array>::Cast(array->Get(context, arrayIndex).ToLocalChecked())->Length();
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
          String::Utf8Value buffer(isolate, array->Get(context, arrayIndex).ToLocalChecked());
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
          return;
        }

        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
      }
    }

    static void make_dataset_from_string(const hid_t& group_id, const char* dset_name, v8::Local<v8::String> buffer, v8::Local<v8::Object> /*options*/) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      String::Utf8Value str_buffer(isolate, buffer);
      herr_t            err = H5LTmake_dataset_string(group_id, dset_name, const_cast<char*>(*str_buffer));
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
        return;
      } 
    }

    static void make_dataset(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      String::Utf8Value dset_name_ptr(isolate, args[1]->ToString(context).ToLocalChecked());
      const char*       dset_name(*dset_name_ptr);
      Int64*            idWrap   = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      hid_t             group_id = idWrap->Value();
      Local<Value>      buffer   = args[2];
      Local<Object>     options;
      bool hasOptionType=false;
      if (args.Length() >= 4 && args[3]->IsObject()) {
        options = args[3]->ToObject(context).ToLocalChecked();
        get_type(options, [&](hid_t _type_id){hasOptionType=true;});
      }
      bool bufferAsUnit8Array=true;
#if NODE_VERSION_AT_LEAST(8,0,0)
#else
      bufferAsUnit8Array=buffer->ToObject(context).ToLocalChecked()->IsUint8Array();
#endif
      if ((hasOptionType && !buffer->IsArray()) || (bufferAsUnit8Array && node::Buffer::HasInstance(buffer) && buffer->ToObject(context).ToLocalChecked()->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked())) {
        make_dataset_from_buffer(group_id, dset_name, buffer->ToObject(context).ToLocalChecked(), options);
      } else if (buffer->IsString()) {
        make_dataset_from_string(group_id, dset_name, buffer->ToString(context).ToLocalChecked(), options);
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type", v8::NewStringType::kInternalized).ToLocalChecked()));
      }
    }

    static void write_dataset(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // fail out if arguments are not correct
      if (args.Length() < 3 || args.Length() > 4 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank   = 1;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
      err = H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info", v8::NewStringType::kInternalized).ToLocalChecked()));
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
        Local<Value>      encodingValue = args[2]->ToObject(context).ToLocalChecked()->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked();
        String::Utf8Value encoding(isolate, encodingValue->ToString(context).ToLocalChecked());
        if (args[2]->ToObject(context).ToLocalChecked()->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked() && std::strcmp("binary", (*encoding))) {
          Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
          hid_t  did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
          herr_t err    = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)node::Buffer::Data(args[2]));
          if (err < 0) {
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
            args.GetReturnValue().SetUndefined();
            return;
          }
          H5Dclose(did);
          args.GetReturnValue().SetUndefined();
          return;
        }

        Int64* idWrap  = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
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
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
        String::Utf8Value buffer(isolate, args[2]->ToString(context).ToLocalChecked());
        Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
        hid_t             did    = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
        herr_t            err    = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)(*buffer));
        if (err < 0) {
          H5Dclose(did);
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type", v8::NewStringType::kInternalized).ToLocalChecked()));
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
          args.GetReturnValue().SetUndefined();
          return;
        }
      }
#if NODE_VERSION_AT_LEAST(8,0,0)
      err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      int               rank = 1;
      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap       = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
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
        std::unique_ptr<String::Utf8Value> value(new v8::String::Utf8Value(isolate, array->Get(context, arrayIndex).ToLocalChecked()));
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      const String::Utf8Value dataset_name(isolate, args[1]->ToString(context).ToLocalChecked());

      Int64*      idWrap      = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
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
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      const String::Utf8Value dataset_name(isolate, args[1]->ToString(context).ToLocalChecked());

      Int64*      idWrap      = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      const hid_t location_id = idWrap->Value();

      const hid_t dataset = H5Dopen(location_id, *dataset_name, H5P_DEFAULT);
      hid_t       t       = H5Dget_type(dataset);

      args.GetReturnValue().Set(Int32::New(v8::Isolate::GetCurrent(), t));

      H5Dclose(dataset);
    }

    static void read_dataset(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() == 4 && (!args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsFunction())) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, [options], callback", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 3 && (!args[0]->IsObject() || !args[1]->IsString() || (!args[2]->IsObject() && !args[2]->IsFunction()))) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, callback[options]", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 2 && (!args[0]->IsObject() || !args[1]->IsString())) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank", v8::NewStringType::kInternalized).ToLocalChecked()));
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
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info", v8::NewStringType::kInternalized).ToLocalChecked()));
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank", v8::NewStringType::kInternalized).ToLocalChecked()));
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
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
            v8::Maybe<bool> ret = array->Set(context, arrayIndex-arrayStart,
                      String::NewFromUtf8(
                          v8::Isolate::GetCurrent(), vl.get()[arrayIndex], v8::NewStringType::kNormal, std::strlen(vl.get()[arrayIndex])).ToLocalChecked());
            if(ret.ToChecked()){
              
            }
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
                    v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
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
              array->Set(context,
                  arrayIndex,
                  String::NewFromUtf8(
                      v8::Isolate::GetCurrent(), tbuffer.get()[arrayIndex], v8::NewStringType::kNormal, std::strlen(tbuffer.get()[arrayIndex])).ToLocalChecked()).Check();
            }
            args.GetReturnValue().Set(array);
            H5Tclose(basetype_id);
          } else if (rank >= 1 && values_dim.get()[0] > 0) {
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
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
              args.GetReturnValue().SetUndefined();
              return;
            }
            hsize_t arrayStart=0;
            if(subsetOn){
              arrayStart=start.get()[0];
            }
            Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], count.get()[0]));
            fill_multi_array(array, tbuffer, values_dim, count, typeSize, arrayStart, 0, rank, paddingType);
            args.GetReturnValue().Set(array);
          } else {
            std::string buffer(bufSize*theSize + 1, 0);
            err = H5LTread_dataset_string(idWrap->Value(), *dset_name, (char*)buffer.c_str());
            if (err < 0) {
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string", v8::NewStringType::kInternalized).ToLocalChecked()));
              args.GetReturnValue().SetUndefined();
              return;
            }

            args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), v8::NewStringType::kNormal, theSize).ToLocalChecked());
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
            if(subsetOn){
              arrayStart=start.get()[0];
              arrayMaximum=std::min(values_dim.get()[0], arrayStart+count.get()[0]);
            }
            std::unique_ptr<hvl_t[]> vl(new hvl_t[arrayMaximum]);
          herr_t err=H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (void*)vl.get());
          if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to vlen read dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
            args.GetReturnValue().SetUndefined();
          }
          else{
          hid_t super_type = H5Tget_super(type_id);
          hsize_t typeSize = H5Tget_size(super_type);
          Local<Array> array = Array::New(v8::Isolate::GetCurrent(), std::min(values_dim.get()[0], count.get()[0]));
          for (unsigned int arrayIndex = arrayStart; arrayIndex < arrayMaximum; arrayIndex++) {
            if(H5Tis_variable_str(super_type)){
              std::string s((char*)vl.get()[arrayIndex].p);
              v8::Maybe<bool> ret = array->Set(context, arrayIndex-arrayStart,
                          String::NewFromUtf8(
                              v8::Isolate::GetCurrent(), (char*)vl.get()[arrayIndex].p, v8::NewStringType::kNormal, vl.get()[arrayIndex].len).ToLocalChecked());
              if(ret.ToChecked()){
                
              }
            }
            else if(typeSize==1){
#if NODE_VERSION_AT_LEAST(14,0,0)
        std::unique_ptr<v8::BackingStore> backing = v8::ArrayBuffer::NewBackingStore(
            vl.get()[arrayIndex].p, typeSize * vl.get()[arrayIndex].len, [](void*, size_t, void*){}, nullptr);
          Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), std::move(backing));
#else
          Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), vl.get()[arrayIndex].p, typeSize * vl.get()[arrayIndex].len);
#endif
              array->Set(context, arrayIndex-arrayStart,
                          v8::Uint8Array::New(arrayBuffer, 0, vl.get()[arrayIndex].len)).Check();
              
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
            v8::Local<v8::Object> int64Buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
            H5LTread_dataset(idWrap->Value(), *dset_name, type_id, (char*)node::Buffer::Data(int64Buffer));

            hid_t native_type_id = H5Tget_native_type(type_id, H5T_DIR_ASCEND);
            if (H5Tequal(H5T_NATIVE_LLONG, native_type_id)) {
              int64Buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked(),
                               Int32::New(v8::Isolate::GetCurrent(), toEnumMap[H5T_NATIVE_LLONG])).Check();
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
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type in reading", v8::NewStringType::kInternalized).ToLocalChecked()));
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
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
              args.GetReturnValue().SetUndefined();
              return;
            }
          }
          //v8::Local<v8::Object> buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
          err                          = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
          if (err < 0) {
            if (subsetOn) {
              H5Sclose(memspace_id);
              H5Sclose(dataspace_id);
            }
            H5Tclose( type_id);
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
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
            options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), rank)).Check();
            H5T_order_t order = H5Tget_order(type_id);
            options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "endian", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), order)).Check();
            switch (rank) {
              case 4:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                break;
              case 3:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                break;
              case 2:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                break;
              case 1:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
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
                    enumeration->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), mname, v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), dvalue)).Check();                    
#if  H5_VERSION_GE(1,8,13)
                    H5free_memory((void *)mname);
#endif
                }
                H5Tclose(t);
                H5Dclose(h);
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "enumeration", v8::NewStringType::kInternalized).ToLocalChecked(), enumeration).Check();
                
            }
            v8::Local<v8::Value> argv[1] = {options};
            v8::MaybeLocal<v8::Value> ret = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
                ->Call(v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(isolate), argc, argv);
            if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){
                
            }
          } else{
            v8::Maybe<bool> ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), rank));
            if(ret.ToChecked()){
              
            }
            switch (rank) {
            case 4:
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3]));
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 3:
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
            case 2:
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              if (rank > 1)
                ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                            Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
              break;
            case 1:
              ret = buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
              break;
          }
          }
          if(bindAttributes){

            v8::Local<v8::Object> focus=buffer->ToObject(context).ToLocalChecked();
            refreshAttributes(focus, did);
          }
          H5Dclose(did);
          args.GetReturnValue().Set(buffer);
          break;
      }
    }
    static void readDatasetAsBuffer(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      // fail out if arguments are not correct
      if (args.Length() == 4 && (!args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsFunction())) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, [options], callback", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 3 && (!args[0]->IsObject() || !args[1]->IsString() || (!args[2]->IsObject() && !args[2]->IsFunction()))) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, callback[options]", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;

      } else if (args.Length() == 2 && (!args[0]->IsObject() || !args[1]->IsString())) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      size_t            bufSize = 0;
      H5T_class_t       class_id;
      int               rank;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
      err = H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info", v8::NewStringType::kInternalized).ToLocalChecked()));
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
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank", v8::NewStringType::kInternalized).ToLocalChecked()));
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
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string", v8::NewStringType::kInternalized).ToLocalChecked()));
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
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kInternalized).ToLocalChecked()));
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
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
            args.GetReturnValue().SetUndefined();
            return;
          }
          bool hit   = false;
          H5T  etype = JS_H5T_UNKNOWN;
          for (std::map<H5T, hid_t>::iterator it = toTypeMap.begin(); !hit && it != toTypeMap.end(); it++) {

            if (H5Tequal(type_id, toTypeMap[(*it).first])) {
              etype = (*it).first;
              hit   = true;
            }
          }

          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "type", v8::NewStringType::kInternalized).ToLocalChecked(), Int32::New(v8::Isolate::GetCurrent(), etype)).Check();
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
            options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), rank)).Check();
            H5T_order_t order = H5Tget_order(type_id);
            options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "endian", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), order)).Check();
            switch (rank) {
              case 4:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                break;
              case 3:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                break;
              case 2:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
                break;
              case 1:
                options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
                break;
            }
            v8::Local<v8::Value> argv[1] = {options};
            v8::MaybeLocal<v8::Value> ret = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
                ->Call(v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(isolate), argc, argv);
            if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){
                
            }
          } else{
            buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), rank)).Check();
            switch (rank) {
            case 4:
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[3])).Check();
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "files", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
              break;
            case 3:
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2])).Check();
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
              break;
            case 2:
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
              if (rank > 1)
                buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns", v8::NewStringType::kInternalized).ToLocalChecked(),
                            Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1])).Check();
              break;
            case 1:
              buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows", v8::NewStringType::kInternalized).ToLocalChecked(),
                          Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0])).Check();
              break;
          }
          }
          H5Tclose( type_id);
          if(bindAttributes){
            v8::Local<v8::Object> focus=buffer->ToObject(context).ToLocalChecked();
            // Attributes
            refreshAttributes(focus, did);
          }
          H5Dclose(did);


          args.GetReturnValue().Set(buffer);
        } break;
        default:
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type", v8::NewStringType::kInternalized).ToLocalChecked()));
          args.GetReturnValue().SetUndefined();
          break;
      }
    }
  };
};
