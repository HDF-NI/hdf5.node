

#include <node.h>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <exception>

#include "hdf5V8.hpp"
#include "file.h"
#include "group.h"
#include "int64.hpp"

#include "H5Lpublic.h"

namespace NodeHDF5 {

  using namespace v8;

    static bool get_separate_attrs(v8::Local<v8::Object> options) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return false;
      }

      auto name(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "separate_attributes", v8::NewStringType::kInternalized).ToLocalChecked());

      if (!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        return false;
      }

      return options->Get(context, name).ToLocalChecked()->BooleanValue(isolate);
    };

  File::File(const char* path) {

    bool exists = std::ifstream(path).good();
    if (exists) {
      id = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to read file, with return: " << id << ".\n";
        error = true;
        throw  Exception(ss.str());
        return;
      }
    }

    if (!exists && id < 0) {
      plist_id = H5Pcreate(H5P_FILE_ACCESS);
      id       = H5Fcreate(path, H5F_ACC_RDONLY, H5P_DEFAULT, plist_id);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to create file, with return: " << id << ".\n";
        error = true;
        throw  Exception(ss.str());
        return;
      }
    }

    gcpl       = H5Pcreate(H5P_GROUP_CREATE);
    herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    if (err < 0) {
      std::stringstream ss;
      ss << "Failed to set link creation order, with return: " << err << ".\n";
      error = true;
      throw  Exception(ss.str());
      
    }
  }

  File::File(const char* path, unsigned int flags) {
    if (flags & (H5F_ACC_EXCL | H5F_ACC_TRUNC | H5F_ACC_DEBUG)) {
      plist_id = H5Pcreate(H5P_FILE_ACCESS);
      id       = H5Fcreate(path, flags, H5P_DEFAULT, plist_id);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to create file, with return: " << id << ".\n";
        error = true;
        throw  Exception(ss.str());
        return;
      }
    } else {
      bool exists = std::ifstream(path).good();
      if (!exists) {
        std::stringstream ss;
        ss << "File " << path << " doesn't exist.";
        error = true;
        throw  Exception(ss.str());
        return;
      }
      id = H5Fopen(path, flags, H5P_DEFAULT);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to open file, " << path << " and flags " << flags << " with return: " << id << ".";
        error = true;
        throw  Exception(ss.str());
        return;
      }
    }

    gcpl       = H5Pcreate(H5P_GROUP_CREATE);
    herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    if (err < 0) {
      std::stringstream ss;
      ss << "Failed to set link creation order, with return: " << err << ".\n";
      error =true;
      throw  Exception(ss.str());
    }
  }

  File::~File() {

    if (id > 0) {
      H5Fclose(id);
    }
  }

  Persistent<FunctionTemplate> File::Constructor;

  void File::Initialize(v8::Local<v8::Object> target) {

    v8::Isolate* isolate = target->GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    // instantiate constructor function template
    Local<FunctionTemplate> t = FunctionTemplate::New(isolate, New);
    t->SetClassName(String::NewFromUtf8(isolate, "File", v8::NewStringType::kInternalized).ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(
      nullptr, nullptr, nullptr, QueryCallbackDelete));
    Constructor.Reset(v8::Isolate::GetCurrent(), t);
    // member method prototypes
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "enableSingleWriteMultiRead", v8::NewStringType::kInternalized).ToLocalChecked(), EnableSingleWriteMultiRead);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "createGroup", v8::NewStringType::kInternalized).ToLocalChecked(), CreateGroup);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "openGroup", v8::NewStringType::kInternalized).ToLocalChecked(), OpenGroup);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getNumAttrs", v8::NewStringType::kInternalized).ToLocalChecked(), GetNumAttrs);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getAttributeNames", v8::NewStringType::kInternalized).ToLocalChecked(), getAttributeNames);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "readAttribute", v8::NewStringType::kInternalized).ToLocalChecked(), readAttribute);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "deleteAttribute", v8::NewStringType::kInternalized).ToLocalChecked(), deleteAttribute);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "refresh", v8::NewStringType::kInternalized).ToLocalChecked(), Refresh);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "move", v8::NewStringType::kInternalized).ToLocalChecked(), Move);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "delete", v8::NewStringType::kInternalized).ToLocalChecked(), Delete);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "flush", v8::NewStringType::kInternalized).ToLocalChecked(), Flush);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "close", v8::NewStringType::kInternalized).ToLocalChecked(), Close);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getNumObjs", v8::NewStringType::kInternalized).ToLocalChecked(), GetNumObjs);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getMemberNames", v8::NewStringType::kInternalized).ToLocalChecked(), GetMemberNames);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getMemberNamesByCreationOrder", v8::NewStringType::kInternalized).ToLocalChecked(), GetMemberNamesByCreationOrder);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getChildType", v8::NewStringType::kInternalized).ToLocalChecked(), GetChildType);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getDatasetType", v8::NewStringType::kInternalized).ToLocalChecked(), getDatasetType);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getDatasetDimensions", v8::NewStringType::kInternalized).ToLocalChecked(), getDatasetDimensions);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getDataType", v8::NewStringType::kInternalized).ToLocalChecked(), getDataType);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getDatasetAttributes", v8::NewStringType::kInternalized).ToLocalChecked(), getDatasetAttributes);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getDatasetAttribute", v8::NewStringType::kInternalized).ToLocalChecked(), getDatasetAttribute);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getByteOrder", v8::NewStringType::kInternalized).ToLocalChecked(), getByteOrder);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getFilters", v8::NewStringType::kInternalized).ToLocalChecked(), getFilters);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "iterate", v8::NewStringType::kInternalized).ToLocalChecked(), iterate);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "visit", v8::NewStringType::kInternalized).ToLocalChecked(), visit);

    // append this function to the target object
    v8::Maybe<bool> ret = target->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "File", v8::NewStringType::kInternalized).ToLocalChecked(), t->GetFunction(context).ToLocalChecked());
    if(ret.ToChecked()){
      
    }
  }

  void File::New(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() < 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected file path", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    String::Utf8Value path(isolate, args[0]->ToString(context).ToLocalChecked());

    // fail out if file is not valid hdf5
    if (args.Length() < 2 && !H5Fis_hdf5(*path)) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "file is not hdf5 format", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    try{
    // create hdf file object
    File* f;
    if (args.Length() < 2) {
      f = new File(*path);
    } else {
      f = new File(*path, args[1]->Uint32Value(context).ToChecked());
    }

    if (f->error) {
      return;
    }

    // extend target object with file
    f->Wrap(args.This());

    // attach various properties
    v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "path", v8::NewStringType::kInternalized).ToLocalChecked(),
                     String::NewFromUtf8(v8::Isolate::GetCurrent(), f->name.c_str(), v8::NewStringType::kInternalized).ToLocalChecked());
    if(ret.ToChecked()){
      
    }
    hsize_t file_size;
    herr_t  ret_value = H5Fget_filesize(f->id, &file_size);
    if (ret_value < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "H5Fget_filesize failed", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "size", v8::NewStringType::kInternalized).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), file_size));
    ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "freeSpace", v8::NewStringType::kInternalized).ToLocalChecked(),
                     Number::New(v8::Isolate::GetCurrent(), H5Fget_freespace(f->id)));
    ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "objectCount", v8::NewStringType::kInternalized).ToLocalChecked(),
                     Number::New(v8::Isolate::GetCurrent(), H5Fget_obj_count(f->id, H5F_OBJ_ALL)));
    Local<Object> idInstance = Int64::Instantiate(args.This(), f->id);
    Int64*        idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
    idWrap->value            = f->id;

    ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), idInstance);
    if(ret.ToChecked()){
      
    }
    } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what(), v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
    } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what(), v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
    }
  }

  void File::EnableSingleWriteMultiRead(const v8::FunctionCallbackInfo<Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected no arguments", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
#if  H5_VERSION_GE(1,10,0)
    File* file = ObjectWrap::Unwrap<File>(args.This());
    if(file->id>=0)H5Fstart_swmr_write(file->id);
#endif
  }

  void File::CreateGroup(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, callback", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());

    Local<Object> instance = Group::Instantiate(args.This());

    // unwrap parent object
    File*                    parent = ObjectWrap::Unwrap<File>(args.This());
    std::vector<std::string> trail;
    std::vector<hid_t>       hidPath;
    std::istringstream       buf(*group_name);
    for (std::string token; getline(buf, token, '/');)
      if (!token.empty())
        trail.push_back(token);
    hid_t previous_hid = parent->getId();
    bool  created      = false;
    for (unsigned int index = 0; index < trail.size(); index++) {
      // check existence of stem
      if (H5Lexists(previous_hid, trail[index].c_str(), H5P_DEFAULT)) {
        hid_t hid = H5Gopen(previous_hid, trail[index].c_str(), H5P_DEFAULT);
        if (hid >= 0) {
          if (index < trail.size() - 1)
            hidPath.push_back(hid);
          previous_hid = hid;
          continue;
        }
      }

      // create group
      hid_t hid = H5Gcreate(previous_hid, trail[index].c_str(), H5P_DEFAULT, parent->getGcpl(), H5P_DEFAULT);
      if (hid < 0) {
        std::string desc;
        {
          H5Ewalk2(H5Eget_current_stack(),
                   H5E_WALK_UPWARD,
                   [](unsigned int n, const H5E_error2_t* err_desc, void* client_data) -> herr_t {
                     if (((std::string*)client_data)->empty())
                       ((std::string*)client_data)->assign(err_desc[0].desc, strlen(err_desc[0].desc));
                     return 0;
                   },
                   (void*)&desc);
        }
        desc = "Group create failed: " + desc;
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), desc.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (index < trail.size() - 1)
        hidPath.push_back(hid);
      if (index == trail.size() - 1) {
        Group* group = new Group(hid);
        group->name.assign(trail[index].c_str());
        group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->Uint32Value(context).ToChecked());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
          args.GetReturnValue().SetUndefined();
          return;
        }
        for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
          group->hidPath.push_back(*it);

        Local<Object> idInstance = Int64::Instantiate(args.This(), group->id);
        Int64*        idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
        idWrap->value            = group->id;
        v8::Maybe<bool> ret = instance->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), idInstance);
        if(ret.ToChecked()){
          
        }
        group->Wrap(instance);

        created = true;
      }
      previous_hid = hid;
    }
    if (!created) {
      Group* group = new Group(previous_hid);
      group->name.assign(trail[trail.size() - 1].c_str());
      group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
      herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->Uint32Value(context).ToChecked());
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
        group->hidPath.push_back(*it);
      v8::Local<v8::Object> idInstance = Int64::Instantiate(args.This(), group->id);
      Int64*                idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = group->id;
      v8::Maybe<bool>  ret = instance->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), idInstance);
      if(ret.ToChecked()){
        
      }
      group->Wrap(instance);

      created = true;
    }

    args.GetReturnValue().Set(instance);
  }

  void File::OpenGroup(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() < 1 || args.Length() > 2 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    
    try {
      Local<Object>     options;
      bool separateAttrs=false;
      if (args.Length() >= 2 && args[1]->IsObject()) {
        options = args[1]->ToObject(context).ToLocalChecked();
        separateAttrs = get_separate_attrs(options);
      }
      String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
      Local<Object> instance = Group::Instantiate(*group_name, args.This(), args[1]->Uint32Value(context).ToChecked());
      if(separateAttrs){
         v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), 2);
          v8::Local<v8::Object> attrs = v8::Object::New(v8::Isolate::GetCurrent());
        Group* group = ObjectWrap::Unwrap<Group>(instance);
        refreshAttributes(attrs, group->id);
        v8::Maybe<bool> ret = array->Set(context, 0, instance);
        ret = array->Set(context, 1, attrs);
        if(ret.ToChecked()){
          
        }
        args.GetReturnValue().Set(array);
      }
      else
        args.GetReturnValue().Set(instance);
      return;
    } catch (Exception& ex) {
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    } catch (std::exception& ex) {
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    
  }

  void File::Move(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 3) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected src name, dest id, dest name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    File*             file = ObjectWrap::Unwrap<File>(args.This());
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value dest_name(isolate, args[2]->ToString(context).ToLocalChecked());
    Int64*            idWrap   = ObjectWrap::Unwrap<Int64>(args[1]->ToObject(context).ToLocalChecked());
    hid_t             group_id = idWrap->Value();

    herr_t            err = H5Lmove(file->id, *group_name, group_id, *dest_name, H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) {
      std::string str(*dest_name);
      std::string errStr = "Failed move link to , " + str + " with return: " + std::to_string(err) + ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
  }

  void File::Delete(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    File* file = ObjectWrap::Unwrap<File>(args.This());
    // delete specified group name
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    H5Ldelete(file->id, (*group_name), H5P_DEFAULT);
  }

  void File::Close(const v8::FunctionCallbackInfo<Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() > 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap file object
    File* file = ObjectWrap::Unwrap<File>(args.This());
    if (!file->id) {
      return;
    }

    H5Pclose(file->gcpl);
    ssize_t size = 0;
    std::stringstream ss;
    bool allClosed=true;
    if (H5Fget_obj_count(file->id, H5F_OBJ_FILE) == 1) {
      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_GROUP)) > 0) {
        allClosed=false;
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_GROUP, size, groupList.get());
        ss << "H5 has not closed all groups: " << H5Fget_obj_count(file->id, H5F_OBJ_GROUP) << " H5F_OBJ_GROUPs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_ATTR)) > 0) {
        allClosed=false;
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_ATTR, size, groupList.get());
        ss << "H5 has not closed all attributes: " << H5Fget_obj_count(file->id, H5F_OBJ_ATTR) << " H5F_OBJ_ATTRs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATASET)) > 0) {
        allClosed=false;
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_DATASET, size, groupList.get());
        ss << "H5 has not closed all datasets" << H5Fget_obj_count(file->id, H5F_OBJ_DATASET) << " H5F_OBJ_DATASETs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATATYPE)) > 0) {
        allClosed=false;
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_DATATYPE, size, groupList.get());
        ss << "H5 has not closed all datatypes" << H5Fget_obj_count(file->id, H5F_OBJ_DATATYPE) << " H5F_OBJ_DATATYPEs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }
    }

    herr_t err = H5Fclose(file->id);
    if (err < 0) {
      ss<<"Failed to close h5"<<std::endl;
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
    }
    else if(!allClosed){
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
    }

    file->id = 0;
    return;
  }
};
