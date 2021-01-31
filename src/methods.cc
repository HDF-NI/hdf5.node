#include <sstream>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "hdf5V8.hpp"
#include "reference.hpp"
#include "attributes.hpp"
#include "methods.hpp"
#include "filters.hpp"

#include "H5LTpublic.h"
#include "H5PTpublic.h"
#include "H5Lpublic.h"

namespace NodeHDF5 {

    void Methods::QueryCallbackDelete(
        v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info) {
    v8::Isolate* isolate = info.GetIsolate();
      //info.GetReturnValue().Set(v8::PropertyAttribute::DontDelete);
     v8::String::Utf8Value attribute_name(isolate, property->ToString(v8::Isolate::GetCurrent()->GetCurrentContext()).ToLocalChecked());
        
        // unwrap group
        Methods* group       = ObjectWrap::Unwrap<Methods>(info.This());
        if(H5Aexists(group->id, (const char*)*attribute_name)){
            /*herr_t err =*/ H5Adelete(group->id, (const char*)*attribute_name);
        }
    }

  void Methods::GetNumAttrs(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // unwrap group
    Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

    args.GetReturnValue().Set(group->getNumAttrs());
  }

  void Methods::getAttributeNames(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // unwrap group
    Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

    Local<Array>             array = Array::New(v8::Isolate::GetCurrent(), group->getNumAttrs());
    hsize_t                  index = 0;
    std::vector<std::string> holder;
    H5Aiterate(group->id,
               H5_INDEX_NAME,
               H5_ITER_INC,
               &index,
               [](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void* operator_data) -> herr_t {
                 ((std::vector<std::string>*)operator_data)->push_back(attr_name);
                 return 0;
               },
               &holder);
    for (index = 0; index < (uint32_t)group->getNumAttrs(); index++) {
      v8::Maybe<bool> ret = array->Set(context, index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str(), v8::NewStringType::kInternalized).ToLocalChecked());
      if(ret.ToChecked()){};
    }

    args.GetReturnValue().Set(array);
  }

  void Methods::readAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected attribute name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified attribute name
    v8::String::Utf8Value attribute_name(isolate, args[0]->ToString(context).ToLocalChecked());
    // unwrap group
    Methods* group       = ObjectWrap::Unwrap<Methods>(args.This());
    if(!H5Aexists(group->id, (const char*)*attribute_name)){
        std::stringstream ss;
      ss<<"Attribute '"<<(*attribute_name)<<"' does not exist.";
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str()).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    v8::Local<v8::Value>&& value = readAttributeByName(args.This(), group->id, (*attribute_name));
    args.GetReturnValue().Set(value);
    return;
  }

  void Methods::deleteAttribute(const v8::FunctionCallbackInfo<Value>& args){
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected attribute name", v8::NewStringType::kNormal).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified attribute name
    v8::String::Utf8Value attribute_name(isolate, args[0]->ToString(context).ToLocalChecked());

    // unwrap group
    Methods* group       = ObjectWrap::Unwrap<Methods>(args.This());
    if(H5Aexists(group->id, (const char*)*attribute_name)){
        /*herr_t err =*/ H5Adelete(group->id, (const char*)*attribute_name);
    }
    
  }
  
  void Methods::GetNumObjs(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // unwrap group
    Methods*   group = ObjectWrap::Unwrap<Methods>(args.This());
    H5G_info_t ginfo; /* Group information */

    H5Gget_info(group->id, &ginfo);
    args.GetReturnValue().Set((uint32_t)ginfo.nlinks);
  }

  void Methods::GetMemberNames(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // unwrap group
    Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

    v8::Local<v8::Array>     array = v8::Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
    uint32_t                 index = 0;
    std::vector<std::string> holder;
    H5G_info_t               ginfo; /* Group information */

    H5Gget_info(group->id, &ginfo);
    args.GetReturnValue().Set((uint32_t)ginfo.nlinks);

    for (index = 0; index < (uint32_t)ginfo.nlinks; index++) {
      std::unique_ptr<char[]>&& name_C=group->getObjnameByIdx(index);
      v8::Maybe<bool> ret = array->Set(context, index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (const char*)name_C.get(), v8::NewStringType::kInternalized).ToLocalChecked());
      if(ret.ToChecked()){};
    }
    args.GetReturnValue().Set(array);
    return;
  }

  void Methods::GetMemberNamesByCreationOrder(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // unwrap group
    Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

    Local<Array>             array = Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
    uint32_t                 index = 0;
    std::vector<std::string> holder;

    herr_t     err;
    H5G_info_t group_info;
    if ((err = H5Gget_info(group->id, &group_info)) < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), " has no info", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    for (index = 0; index < (uint32_t)group_info.nlinks; index++) {
      std::string datasetTitle;
      H5L_info_t  link_buff;
      herr_t      err = H5Lget_info_by_idx(group->id, ".", H5_INDEX_NAME, H5_ITER_INC, index, &link_buff, H5P_DEFAULT);
      if (err >= 0) {
        H5_index_t index_field = (link_buff.corder_valid) ? H5_INDEX_CRT_ORDER : H5_INDEX_NAME;
        /*
         * Get size of name, add 1 for null terminator.
         */
        ssize_t size = 1 + H5Lget_name_by_idx(group->id, ".", index_field, H5_ITER_INC, index, NULL, 0, H5P_DEFAULT);

        /*
         * Allocate storage for name.
         */
        datasetTitle.resize(size);

        /*
         * Retrieve name, print it, and free the previously allocated
         * space.
         */
        size = H5Lget_name_by_idx(group->id, ".", index_field, H5_ITER_INC, index, (char*)datasetTitle.c_str(), (size_t)size, H5P_DEFAULT);
      }
      v8::Maybe<bool> ret = array->Set(context, index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), datasetTitle.c_str(), v8::NewStringType::kInternalized).ToLocalChecked());
      if(ret.ToChecked()){};
    }
    args.GetReturnValue().Set(array);
    return;
  }

  void Methods::GetChildType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
    // store specified child name
    v8::String::Utf8Value child_name(isolate, args[0]->ToString(context).ToLocalChecked());
    args.GetReturnValue().Set((uint32_t)group->childObjType(*child_name));
  }

  void Methods::getDatasetType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified child name
    v8::String::Utf8Value child_name(isolate, args[0]->ToString(context).ToLocalChecked());
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
    hid_t  id     = idWrap->Value();
    HLType hlType = HLType::HL_TYPE_LITE;
    if (H5IMis_image(id, (*child_name))) {
      hlType = HLType::HL_TYPE_IMAGE;
    } else {
      hid_t ds = H5Dopen(id, (*child_name), H5P_DEFAULT);
      if (ds >= 0) {

        hid_t type = H5Dget_type(ds);
        switch (H5Tget_class(type)) {
          case H5T_COMPOUND: {

            int  nmembers     = H5Tget_nmembers(type);
            bool variableType = true;
            for (int memberIndex = 0; memberIndex < nmembers; memberIndex++) {
              hid_t memberType = H5Tget_member_type(type, memberIndex);
              if (!(H5Tis_variable_str(memberType)>0)) {
                variableType = false;
              }

              H5Tclose(memberType);
            }
            if (variableType) {
              hlType = HLType::HL_TYPE_PACKET_TABLE;
            } else {
              hlType = HLType::HL_TYPE_TABLE;
            }
          } break;
          case H5T_STRING: hlType = HLType::HL_TYPE_TEXT; break;
          default: break;
        }
        if (type >= 0) {
          H5Tclose(type);
        }
        H5Dclose(ds);
      }
    }
    args.GetReturnValue().Set((uint32_t)hlType);
    return;
  }

  void Methods::getDatasetDimensions(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    if (args.Length() != 1 || !args[0]->IsString()) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    const String::Utf8Value dataset_name(isolate, args[0]->ToString(context).ToLocalChecked());

    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
    const hid_t location_id = idWrap->Value();

    const hid_t dataset   = H5Dopen(location_id, *dataset_name, H5P_DEFAULT);
    if(dataset<0){
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "can't open dataset", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    const hid_t dataspace = H5Dget_space(dataset);
    if(dataspace<0){
      H5Dclose(dataset);
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "can't get dataset space", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    const int rank = H5Sget_simple_extent_ndims(dataspace);

    std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
    std::unique_ptr<hsize_t[]> maxdims(new hsize_t[rank]);
    H5Sget_simple_extent_dims(dataspace, dims.get(), maxdims.get());

    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), rank);
    for (int elementIndex = 0; elementIndex < rank; elementIndex++) {
      v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), dims.get()[elementIndex]));
      if(ret.ToChecked()){};
    }
    args.GetReturnValue().Set(array);

    H5Sclose(dataspace);
    H5Dclose(dataset);

    return;
  }

  void Methods::getDataType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified child name
    v8::String::Utf8Value child_name(isolate, args[0]->ToString(context).ToLocalChecked());
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
    hid_t  did    = H5Dopen(idWrap->Value(), *child_name, H5P_DEFAULT);
    hid_t  t      = H5Dget_type(did);
    bool   hit    = false;
    H5T    etype  = JS_H5T_UNKNOWN;
    for (std::map<H5T, hid_t>::iterator it = toTypeMap.begin(); !hit && it != toTypeMap.end(); it++) {

      if (H5Tequal(t, toTypeMap[(*it).first])) {
        etype = (*it).first;
        hit   = true;
      }
    }
    args.GetReturnValue().Set((uint32_t)etype);
    H5Tclose(t);
    H5Dclose(did);
    return;
  }

  void Methods::getDatasetAttributes(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected dataset name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    v8::String::Utf8Value dset_name(isolate, args[0]->ToString(context).ToLocalChecked());
    // unwrap group
    Methods*    group = ObjectWrap::Unwrap<Methods>(args.This());
    std::string name(*dset_name);
    H5O_info_t  object_info;
    herr_t      err = H5Oget_info_by_name(group->id, *dset_name, &object_info, H5P_DEFAULT);
    if (err < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get attr info", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    v8::Local<v8::Object> attrs = v8::Object::New(v8::Isolate::GetCurrent());
    for (unsigned int index = 0; index < object_info.num_attrs; index++) {
      hid_t attr_id = H5Aopen_by_idx(group->id, *dset_name, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, index, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to open attr", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5A_info_t ainfo;
      H5Aget_info(attr_id, &ainfo);
      size_t      nameSize = H5Aget_name(attr_id, 0, NULL);
      std::string attrName(nameSize + 1, '\0');
      H5Aget_name(attr_id, nameSize + 1, (char*)attrName.c_str());
      hid_t    attr_type    = H5Aget_type(attr_id);
      hid_t    space_id     = H5Aget_space(attr_id);
      hssize_t num_elements = H5Sget_simple_extent_npoints(space_id);

      switch (H5Tget_class(attr_type)) {
        case H5T_BITFIELD:
        case H5T_OPAQUE:
            break;
        case H5T_REFERENCE:{
          std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
          H5Aread(attr_id, attr_type, buf.get());
          hid_t objectId=((hid_t*)buf.get())[0];
          v8::Local<v8::Object>&& ref = Reference::Instantiate(objectId, 1);
          v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), ref);
                  if(ret.ToChecked()){};
        }
            break;
        case H5T_ARRAY:
        case H5T_ENUM: break;
        case H5T_COMPOUND: {
          v8::Local<v8::Array>    array = v8::Array::New(v8::Isolate::GetCurrent(), H5Tget_nmembers(attr_type));
          std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
          H5Aread(attr_id, attr_type, buf.get());
          for (int mIndex = 0; mIndex < H5Tget_nmembers(attr_type); mIndex++) {
            hid_t mType = H5Tget_member_type(attr_type, mIndex);

            switch (H5Tget_member_class(attr_type, mIndex)) {
              case H5T_BITFIELD:
              case H5T_OPAQUE: break;
              case H5T_REFERENCE: {
                H5O_type_t obj_type;
                H5Rget_obj_type(attr_id, H5R_OBJECT, buf.get(), &obj_type);
                ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, buf.get(), NULL, 0);
                std::string refName(size, '\0');
                size            = H5Rget_name(attr_id, H5R_OBJECT, buf.get(), (char*)refName.c_str(), size + 1);
                std::string ref = "->" + refName;
                v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
                  if(ret.ToChecked()){};
              } break;
              case H5T_ARRAY:
              case H5T_ENUM:
              case H5T_COMPOUND: break;
              case H5T_INTEGER: {
                size_t size = H5Tget_size(attr_type);
                if(size == 8){
                  v8::Maybe<bool> ret = array->Set(context,
                      mIndex,
                      v8::Integer::New(v8::Isolate::GetCurrent(), ((long long*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else if(size == 4){
                  v8::Maybe<bool> ret = array->Set(context,
                      mIndex,
                      v8::Int32::New(v8::Isolate::GetCurrent(), ((long*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else if(size == 2){
                  v8::Maybe<bool> ret = array->Set(context,
                      mIndex,
                      v8::Int32::New(v8::Isolate::GetCurrent(), ((short*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else{
                  v8::Maybe<bool> ret = array->Set(context,
                      mIndex,
                      v8::Int32::New(v8::Isolate::GetCurrent(), ((char*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
              } break;
              case H5T_FLOAT: {
                size_t size = H5Tget_size(attr_type);
                if(size == 8){
                  v8::Maybe<bool> ret = array->Set(context, mIndex,
                             v8::Int32::New(v8::Isolate::GetCurrent(), ((double*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else{
                  v8::Maybe<bool> ret = array->Set(context, mIndex,
                             v8::Int32::New(v8::Isolate::GetCurrent(), ((float*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  
                  if(ret.ToChecked()){};
                }
              } break;
              case H5T_VLEN: break;
              case H5T_STRING: {
                v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)), v8::NewStringType::kInternalized).ToLocalChecked());
                if(ret.ToChecked()){};
               } break;
              case H5T_NO_CLASS:
              default: break;
            }

            H5Tclose(mType);
          }
          v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
          if(ret.ToChecked()){};
        } break;
        case H5T_INTEGER: {
          size_t size = H5Tget_size(attr_type);
          if(size == 8){
            std::unique_ptr<long long[]> buf(new long long[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Integer::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};
            } else{
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::Integer::New(v8::Isolate::GetCurrent(), buf.get()[0]));
              if(ret.ToChecked()){};
            }
          }
          else if(size == 4){
            std::unique_ptr<long[]> buf(new long[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};
            } else{
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
              if(ret.ToChecked()){};
            }
          }
          else if(size == 2){
            std::unique_ptr<short[]> buf(new short[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};
            } else{
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
              if(ret.ToChecked()){};
            }
          }
          else {
            std::unique_ptr<char[]> buf(new char[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};
            } else{
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
              if(ret.ToChecked()){};
            }
          }
        } break;
        case H5T_FLOAT: {
          size_t size = H5Tget_size(attr_type);
          if(size == 8){
            double value;
            H5Aread(attr_id, attr_type, &value);
            v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                       v8::Number::New(v8::Isolate::GetCurrent(), value));
            if(ret.ToChecked()){};
          }
          else{
            float value;
            H5Aread(attr_id, attr_type, &value);
            v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                       v8::Number::New(v8::Isolate::GetCurrent(), value));
            if(ret.ToChecked()){};
          }
        } break;
        case H5T_VLEN: {
          hid_t super_type = H5Tget_super(attr_type);

          if (H5Tget_class(super_type) == H5T_REFERENCE) {
            H5O_type_t             obj_type;
            std::unique_ptr<hvl_t[]> vl(new hvl_t[num_elements]);
            H5Aread(attr_id, attr_type, (void*)vl.get());
            if (num_elements > 0) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                /*herr_t err=*/H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, &obj_type);
                ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, NULL, 0);
                std::string refName(size, '\0');
                size            = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, (char*)refName.c_str(), size + 1);
                std::string ref = "->" + refName;
                v8::Maybe<bool> ret = array->Set(context,
                    elementIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
                if(ret.ToChecked()){};
            } else {
              H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[0].p, &obj_type);

              ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, NULL, 0);
              std::string refName(size, '\0');
              size            = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, (char*)refName.c_str(), size + 1);
              std::string ref = "->" + refName;
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
              if(ret.ToChecked()){};
            }
          } else {
            std::unique_ptr<hvl_t[]> vl(new hvl_t[num_elements]);
            H5Aread(attr_id, attr_type, (void*)vl.get());
            v8::Maybe<bool> ret = attrs->Set(context,
                v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(vl.get()[0].p), v8::NewStringType::kNormal, vl.get()[0].len).ToLocalChecked());
            if(ret.ToChecked()){};
          }
          if (super_type >= 0)
            H5Tclose(super_type);
        } break;
        case H5T_STRING: {
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
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};

            } else {
            std::unique_ptr<char*[]>data(new char*[1]);
            //std::memset(data.get(), 0, H5Aget_storage_size(attr_id) + 1); // clear buffer
              H5Aread(attr_id, attr_type, data.get());
              std::string attrValue = "";
              if (data.get()[0] != NULL)
                  attrValue = std::string(data.get()[0]);
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                         v8::String::NewFromUtf8(
                             v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::NewStringType::kNormal, attrValue.length()).ToLocalChecked());
              if(ret.ToChecked()){};
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
                if(ret.ToChecked()){};
              }
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(), array);
              if(ret.ToChecked()){};
            }
            else{
              v8::Maybe<bool> ret = attrs->Set(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str(), v8::NewStringType::kInternalized).ToLocalChecked(),
                       v8::String::NewFromUtf8(
                           v8::Isolate::GetCurrent(), (char*)data.get(), v8::NewStringType::kNormal, storeSize).ToLocalChecked());
              if(ret.ToChecked()){};
            }
          }
        } break;
        case H5T_NO_CLASS:
        default: break;
      }
      H5Sclose(space_id);
      H5Tclose(attr_type);
      H5Aclose(attr_id);
    }
    args.GetReturnValue().Set(attrs);
  }

  void Methods::getDatasetAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 2 || !args[0]->IsString()|| !args[1]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected dataset and attribute name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    v8::String::Utf8Value dset_name(isolate, args[0]->ToString(context).ToLocalChecked());
    v8::String::Utf8Value attr_name(isolate, args[1]->ToString(context).ToLocalChecked());
    // unwrap group
    Methods*    group = ObjectWrap::Unwrap<Methods>(args.This());
    std::string name(*dset_name);
    H5O_info_t  object_info;
    herr_t      err = H5Oget_info_by_name(group->id, *dset_name, &object_info, H5P_DEFAULT);
    if (err < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get attr info", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
        
      hid_t attr_id = H5Aopen_by_name(group->id, *dset_name, *attr_name, H5P_DEFAULT, H5P_DEFAULT);
      if (attr_id < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to open attr", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5A_info_t ainfo;
      H5Aget_info(attr_id, &ainfo);
      size_t      nameSize = H5Aget_name(attr_id, 0, NULL);
      std::string attrName(nameSize + 1, '\0');
      H5Aget_name(attr_id, nameSize + 1, (char*)attrName.c_str());
      hid_t    attr_type    = H5Aget_type(attr_id);
      hid_t    space_id     = H5Aget_space(attr_id);
      hssize_t num_elements = H5Sget_simple_extent_npoints(space_id);

      switch (H5Tget_class(attr_type)) {
        case H5T_BITFIELD:
        case H5T_OPAQUE:
            break;
        case H5T_REFERENCE:{
          std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
          H5Aread(attr_id, attr_type, buf.get());
          hid_t objectId=((hid_t*)buf.get())[0];
          v8::Local<v8::Object>&& ref = Reference::Instantiate(objectId, 1);
          args.GetReturnValue().Set(ref);
        }
            break;
        case H5T_ARRAY:
        case H5T_ENUM: break;
        case H5T_COMPOUND: {
          v8::Local<v8::Array>    array = v8::Array::New(v8::Isolate::GetCurrent(), H5Tget_nmembers(attr_type));
          std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
          H5Aread(attr_id, attr_type, buf.get());
          for (int mIndex = 0; mIndex < H5Tget_nmembers(attr_type); mIndex++) {
            hid_t mType = H5Tget_member_type(attr_type, mIndex);

            switch (H5Tget_member_class(attr_type, mIndex)) {
              case H5T_BITFIELD:
              case H5T_OPAQUE: break;
              case H5T_REFERENCE: {
                H5O_type_t obj_type;
                H5Rget_obj_type(attr_id, H5R_OBJECT, buf.get(), &obj_type);
                ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, buf.get(), NULL, 0);
                std::string refName(size, '\0');
                size            = H5Rget_name(attr_id, H5R_OBJECT, buf.get(), (char*)refName.c_str(), size + 1);
                std::string ref = "->" + refName;
                v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
                    if(ret.ToChecked()){};
              } break;
              case H5T_ARRAY:
              case H5T_ENUM:
              case H5T_COMPOUND: break;
              case H5T_INTEGER: {
                size_t size = H5Tget_size(attr_type);
                if(size == 8){
                  v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::Integer::New(v8::Isolate::GetCurrent(), ((long long*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else if(size == 4){
                  v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::Int32::New(v8::Isolate::GetCurrent(), ((long*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else if(size == 2){
                  v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::Int32::New(v8::Isolate::GetCurrent(), ((short*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else{
                  v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::Int32::New(v8::Isolate::GetCurrent(), ((char*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
              } break;
              case H5T_FLOAT: {
                size_t size = H5Tget_size(attr_type);
                if(size == 8){
                  v8::Maybe<bool> ret = array->Set(context, mIndex,
                             v8::Number::New(v8::Isolate::GetCurrent(), ((double*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                }
                else{
                  v8::Maybe<bool> ret = array->Set(context, mIndex,
                             v8::Number::New(v8::Isolate::GetCurrent(), ((float*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)))[0]));
                  if(ret.ToChecked()){};
                  
                }
              } break;
              case H5T_VLEN: break;
              case H5T_STRING: {
                v8::Maybe<bool> ret = array->Set(context,
                    mIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(buf.get() + H5Tget_member_offset(attr_type, mIndex)), v8::NewStringType::kInternalized).ToLocalChecked());
                if(ret.ToChecked()){};
              } break;
              case H5T_NO_CLASS:
              default: break;
            }

            H5Tclose(mType);
          }
          args.GetReturnValue().Set(array);
        } break;
        case H5T_INTEGER: {
          size_t size = H5Tget_size(attr_type);
          if(size == 8){
            std::unique_ptr<long long[]> buf(new long long[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Integer::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            } else
              args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), buf.get()[0]));
          }
          else if(size == 4){
            std::unique_ptr<long[]> buf(new long[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            } else
              args.GetReturnValue().Set(v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
          }
          else if(size == 2){
            std::unique_ptr<short[]> buf(new short[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            } else{
              args.GetReturnValue().Set(v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
            }
          }
          else {
            std::unique_ptr<char[]> buf(new char[num_elements]);
            H5Aread(attr_id, attr_type, buf.get());
            if (num_elements > 1) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                v8::Maybe<bool> ret = array->Set(context, elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            } else
              args.GetReturnValue().Set(v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
          }
        } break;
        case H5T_FLOAT: {
          size_t size = H5Tget_size(attr_type);
          if(size == 8){
            double value;
            H5Aread(attr_id, attr_type, &value);
            args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), value));
          }
          else{
            float value;
            H5Aread(attr_id, attr_type, &value);
            args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), value));
            
          }
        } break;
        case H5T_VLEN: {
          hid_t super_type = H5Tget_super(attr_type);

          if (H5Tget_class(super_type) == H5T_REFERENCE) {
            H5O_type_t             obj_type;
            std::unique_ptr<hvl_t[]> vl(new hvl_t[num_elements]);
            H5Aread(attr_id, attr_type, (void*)vl.get());
            if (num_elements > 0) {
              v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
              for (unsigned int elementIndex = 0; elementIndex < num_elements; elementIndex++) {
                /*herr_t err=*/H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, &obj_type);
                ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, NULL, 0);
                std::string refName(size, '\0');
                size            = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, (char*)refName.c_str(), size + 1);
                std::string ref = "->" + refName;
                v8::Maybe<bool> ret = array->Set(context,
                    elementIndex,
                    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            } else {
              H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[0].p, &obj_type);

              ssize_t     size = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, NULL, 0);
              std::string refName(size, '\0');
              size            = H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, (char*)refName.c_str(), size + 1);
              std::string ref = "->" + refName;
              args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::NewStringType::kNormal, ref.length()).ToLocalChecked());
            }
          } else {
            std::unique_ptr<hvl_t[]> vl(new hvl_t[num_elements]);
            H5Aread(attr_id, attr_type, (void*)vl.get());
            args.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(vl.get()[0].p), v8::NewStringType::kNormal, vl.get()[0].len).ToLocalChecked());
          }
          if (super_type >= 0)
            H5Tclose(super_type);
        } break;
        case H5T_STRING: {
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
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);

            } else {
            std::unique_ptr<char*[]>data(new char*[1]);
            //std::memset(data.get(), 0, H5Aget_storage_size(attr_id) + 1); // clear buffer
              H5Aread(attr_id, attr_type, data.get());
              std::string attrValue = "";
              if (data.get()[0] != NULL)
                  attrValue = std::string(data.get()[0]);
              args.GetReturnValue().Set(v8::String::NewFromUtf8(
                             v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::NewStringType::kNormal, attrValue.length()).ToLocalChecked());
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
                if(ret.ToChecked()){};
              }
              args.GetReturnValue().Set(array);
            }
            else{
              args.GetReturnValue().Set(v8::String::NewFromUtf8(
                           v8::Isolate::GetCurrent(), (char*)data.get(), v8::NewStringType::kNormal, storeSize).ToLocalChecked());
            }
          }
        } break;
        case H5T_NO_CLASS:
        default: break;
      }
      H5Sclose(space_id);
      H5Tclose(attr_type);
      H5Aclose(attr_id);
  }

  void Methods::getByteOrder(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified child name
    v8::String::Utf8Value child_name(isolate, args[0]->ToString(context).ToLocalChecked());
    Int64*      idWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked());
    hid_t       did    = H5Dopen(idWrap->Value(), *child_name, H5P_DEFAULT);
    hid_t       t      = H5Dget_type(did);
    H5T_order_t order  = H5Tget_order(t);
    args.GetReturnValue().Set((uint32_t)order);
    H5Tclose(t);
    H5Dclose(did);
  }

  void Methods::getFilters(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected dataset name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    v8::String::Utf8Value dset_name(isolate, args[0]->ToString(context).ToLocalChecked());
    // unwrap group
    Methods*                group = ObjectWrap::Unwrap<Methods>(args.This());
    std::string             name(*dset_name);
    v8::Local<v8::Object>&& filters = Filters::Instantiate(group->id, name);
    args.GetReturnValue().Set(filters);
  }

  // protected methods

  int Methods::getNumAttrs() {
    H5O_info_t oinfo; /* Object info */

    H5Oget_info(id, &oinfo);
    return ((int)oinfo.num_attrs);
  }

  hsize_t Methods::getNumObjs() {
    H5G_info_t ginfo; /* Group information */

    H5Gget_info(id, &ginfo);
    return ginfo.nlinks;
  }

  H5O_type_t Methods::childObjType(const char* objname) {
    H5O_info_t objinfo;

    // Use C API to get information of the object
    herr_t ret_value = H5Oget_info_by_name(id, objname, &objinfo, H5P_DEFAULT);

    if (ret_value < 0) {
      // Return a valid type or throw an exception for unknown type
      return H5O_TYPE_UNKNOWN;
    }

    switch (objinfo.type) {
      case H5O_TYPE_GROUP:
      case H5O_TYPE_DATASET:
      case H5O_TYPE_NAMED_DATATYPE: return objinfo.type; break;
      default: break;
    }
    return H5O_TYPE_UNKNOWN;
  }

  std::unique_ptr<char[]> Methods::getObjnameByIdx(hsize_t idx) {
    // call H5Lget_name_by_idx with name as NULL to get its length
    ssize_t name_len = H5Lget_name_by_idx(id, ".", H5_INDEX_NAME, H5_ITER_INC, idx, NULL, 0, H5P_DEFAULT);

    // now, allocate C buffer to get the name
    std::unique_ptr<char[]> name_C(new char[name_len + 1]);
    std::memset(name_C.get(), 0, name_len + 1); // clear buffer

    name_len = H5Lget_name_by_idx(id, ".", H5_INDEX_NAME, H5_ITER_INC, idx, name_C.get(), name_len+1, H5P_DEFAULT);
    return name_C;
  }

    void Methods::iterate(const v8::FunctionCallbackInfo<Value>& args) {

      Methods*                group = ObjectWrap::Unwrap<Methods>(args.This());
//      v8::Isolate* isolate = args.GetIsolate();
//      v8::Local<v8::Context> context = isolate->GetCurrentContext();
//      hsize_t                          idx = args[0]->Int32Value();
      v8::Persistent<v8::Function> callback;
//      const unsigned               argc = 2;
      callback.Reset(v8::Isolate::GetCurrent(), args[1].As<Function>());
      v8::Local<v8::Function> func = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback);
      herr_t                  err  = H5Literate(group->id,
                                      H5_INDEX_NAME,H5_ITER_NATIVE,
                                      nullptr,
                                      [](hid_t group, const char *name, const H5L_info_t *info, void *op_data) -> herr_t {
                                        v8::Local<v8::Value> argv[2] = {v8::Int32::New(v8::Isolate::GetCurrent(), toEnumMap[info->type]),
                                                                        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name, v8::NewStringType::kInternalized).ToLocalChecked()};
                                        v8::MaybeLocal<v8::Value> ret = ((v8::Local<v8::Function>*)op_data)[0]->Call(
                                            v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(v8::Isolate::GetCurrent()), 3, argv);
                                        if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){
                                            
                                        }
                                        return (herr_t)0;
                                      },
                                      &func);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed iterating through children", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      args.GetReturnValue().SetUndefined();
    }

    void Methods::visit(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Methods*                group = ObjectWrap::Unwrap<Methods>(args.This());
//      hsize_t                          idx = args[0]->Int32Value();
      v8::Persistent<v8::Function> callback;
//      const unsigned               argc = 2;
      callback.Reset(v8::Isolate::GetCurrent(), args[1].As<Function>());
//      std::function<herr_t(hid_t did, unsigned int dim, hid_t dsid, void* visitor_data)> f =
//          [&](hid_t group, const char *name, const H5L_info_t *info, void *op_data) {
//            v8::Local<v8::Value> argv[2] = {v8::Int32::New(v8::Isolate::GetCurrent(), dim),
//                                            v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "success")};
//            v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
//                ->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
//            return (herr_t)0;
//          };
      v8::Local<v8::Function> func = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback);
      herr_t                  err  = H5Lvisit(group->id,
                                      H5_INDEX_NAME,H5_ITER_NATIVE,
                                      [](hid_t group, const char *name, const H5L_info_t *info, void *op_data) -> herr_t {
                                        v8::Local<v8::Value> argv[2] = {v8::Int32::New(v8::Isolate::GetCurrent(), toEnumMap[info->type]),
                                                                        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name, v8::NewStringType::kInternalized).ToLocalChecked()};
                                        v8::MaybeLocal<v8::Value> ret = ((v8::Local<v8::Function>*)op_data)[0]->Call(
                                            v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(v8::Isolate::GetCurrent()), 2, argv);
                                        if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){
                                            
                                        }
                                        return (herr_t)0;
                                      },
                                      &func);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed iterating through children", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      args.GetReturnValue().SetUndefined();
    }

}
