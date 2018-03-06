

#include <node.h>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>

#include "file.h"
#include "group.h"
#include "int64.hpp"

#include "H5Lpublic.h"

namespace NodeHDF5 {

  using namespace v8;

  File::File(const char* path) {

    bool exists = std::ifstream(path).good();
    if (exists) {
      id = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
    }

    if (!exists && id < 0) {
      plist_id = H5Pcreate(H5P_FILE_ACCESS);
      id       = H5Fcreate(path, H5F_ACC_RDONLY, H5P_DEFAULT, plist_id);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to create file, with return: " << id << ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
        error = true;
        return;
      }
    }

    gcpl       = H5Pcreate(H5P_GROUP_CREATE);
    herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    if (err < 0) {
      std::stringstream ss;
      ss << "Failed to set link creation order, with return: " << err << ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
      error = true;
    }
  }

  File::File(const char* path, unsigned int flags) {
    if (flags & (H5F_ACC_EXCL | H5F_ACC_TRUNC | H5F_ACC_DEBUG)) {
      plist_id = H5Pcreate(H5P_FILE_ACCESS);
      id       = H5Fcreate(path, flags, H5P_DEFAULT, plist_id);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to create file, with return: " << id << ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
        error = true;
        return;
      }
    } else {
      id = H5Fopen(path, flags, H5P_DEFAULT);
      if (id < 0) {
        std::stringstream ss;
        ss << "Failed to open file, " << path << " and flags " << flags << " with return: " << id << ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
        error = true;
        return;
      }
    }

    gcpl       = H5Pcreate(H5P_GROUP_CREATE);
    herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
    if (err < 0) {
      std::stringstream ss;
      ss << "Failed to set link creation order, with return: " << err << ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
    }
  }

  File::~File() {

    if (id > 0) {
      H5Fclose(id);
    }
  }

  Persistent<FunctionTemplate> File::Constructor;

  void File::Initialize(Handle<Object> target) {

    HandleScope scope(v8::Isolate::GetCurrent());

    // instantiate constructor function template
    Local<FunctionTemplate> t = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
    t->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"));
    t->InstanceTemplate()->SetInternalFieldCount(1);
    Constructor.Reset(v8::Isolate::GetCurrent(), t);
    // member method prototypes
    NODE_SET_PROTOTYPE_METHOD(t, "enableSingleWriteMumltiRead", EnableSingleWriteMumltiRead);
    NODE_SET_PROTOTYPE_METHOD(t, "createGroup", CreateGroup);
    NODE_SET_PROTOTYPE_METHOD(t, "openGroup", OpenGroup);
    NODE_SET_PROTOTYPE_METHOD(t, "getNumAttrs", GetNumAttrs);
    NODE_SET_PROTOTYPE_METHOD(t, "refresh", Refresh);
    NODE_SET_PROTOTYPE_METHOD(t, "move", Move);
    NODE_SET_PROTOTYPE_METHOD(t, "delete", Delete);
    NODE_SET_PROTOTYPE_METHOD(t, "flush", Flush);
    NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(t, "getNumObjs", GetNumObjs);
    NODE_SET_PROTOTYPE_METHOD(t, "getMemberNames", GetMemberNames);
    NODE_SET_PROTOTYPE_METHOD(t, "getMemberNamesByCreationOrder", GetMemberNamesByCreationOrder);
    NODE_SET_PROTOTYPE_METHOD(t, "getChildType", GetChildType);
    NODE_SET_PROTOTYPE_METHOD(t, "getDatasetType", getDatasetType);
    NODE_SET_PROTOTYPE_METHOD(t, "getDatasetDimensions", getDatasetDimensions);
    NODE_SET_PROTOTYPE_METHOD(t, "getDataType", getDataType);
    NODE_SET_PROTOTYPE_METHOD(t, "getDatasetAttributes", getDatasetAttributes);
    NODE_SET_PROTOTYPE_METHOD(t, "getByteOrder", getByteOrder);
    NODE_SET_PROTOTYPE_METHOD(t, "getFilters", getFilters);
    NODE_SET_PROTOTYPE_METHOD(t, "iterate", iterate);

    // append this function to the target object
    target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"), t->GetFunction());
  }

  void File::New(const v8::FunctionCallbackInfo<Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() < 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected file path")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    String::Utf8Value path(args[0]->ToString());

    // fail out if file is not valid hdf5
    if (args.Length() < 2 && !H5Fis_hdf5(*path)) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "file is not hdf5 format")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // create hdf file object
    File* f;
    if (args.Length() < 2) {
      f = new File(*path);
    } else {
      f = new File(*path, args[1]->Uint32Value());
    }

    if (f->error) {
      return;
    }

    // extend target object with file
    f->Wrap(args.This());

    // attach various properties
    args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "path"),
                     String::NewFromUtf8(v8::Isolate::GetCurrent(), f->name.c_str()));
    hsize_t file_size;
    herr_t  ret_value = H5Fget_filesize(f->id, &file_size);
    if (ret_value < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "H5Fget_filesize failed")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "size"), Number::New(v8::Isolate::GetCurrent(), file_size));
    args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "freeSpace"),
                     Number::New(v8::Isolate::GetCurrent(), H5Fget_freespace(f->id)));
    args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "objectCount"),
                     Number::New(v8::Isolate::GetCurrent(), H5Fget_obj_count(f->id, H5F_OBJ_ALL)));
    Local<Object> idInstance = Int64::Instantiate(args.This(), f->id);
    Int64*        idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
    idWrap->value            = f->id;

    args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), idInstance);
  }

  void File::EnableSingleWriteMumltiRead(const v8::FunctionCallbackInfo<Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected no arguments")));
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

    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, callback")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    String::Utf8Value group_name(args[0]->ToString());

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
                     //                //std::cout<<"n="<<n<<" "<<err_desc[0].desc<<std::endl;
                     if (((std::string*)client_data)->empty())
                       ((std::string*)client_data)->assign(err_desc[0].desc, strlen(err_desc[0].desc));
                     return 0;
                   },
                   (void*)&desc);
        }
        desc = "Group create failed: " + desc;
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), desc.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      if (index < trail.size() - 1)
        hidPath.push_back(hid);
      if (index == trail.size() - 1) {
        Group* group = new Group(hid);
        group->name.assign(trail[index].c_str());
        group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->Uint32Value());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
          args.GetReturnValue().SetUndefined();
          return;
        }
        for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
          group->hidPath.push_back(*it);

        Local<Object> idInstance = Int64::Instantiate(args.This(), group->id);
        Int64*        idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
        idWrap->value            = group->id;
        instance->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), idInstance);
        group->Wrap(instance);

        created = true;
      }
      previous_hid = hid;
    }
    if (!created) {
      Group* group = new Group(previous_hid);
      group->name.assign(trail[trail.size() - 1].c_str());
      group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
      herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->Uint32Value());
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
        group->hidPath.push_back(*it);
      v8::Local<v8::Object> idInstance = Int64::Instantiate(args.This(), group->id);
      Int64*                idWrap     = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = group->id;
      instance->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), idInstance);
      group->Wrap(instance);

      created = true;
    }

    args.GetReturnValue().Set(instance);
  }

  void File::OpenGroup(const v8::FunctionCallbackInfo<Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() < 1 || args.Length() > 2 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    String::Utf8Value group_name(args[0]->ToString());

    Local<Object> instance = Group::Instantiate(*group_name, args.This(), args[1]->Uint32Value());
    args.GetReturnValue().Set(instance);
    return;
  }

  void File::Move(const v8::FunctionCallbackInfo<Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 3) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected src name, dest id, dest name")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    File*             file = ObjectWrap::Unwrap<File>(args.This());
    String::Utf8Value group_name(args[0]->ToString());
    String::Utf8Value dest_name(args[2]->ToString());
    herr_t            err = H5Lmove(file->id, *group_name, args[1]->Uint32Value(), *dest_name, H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) {
      std::string str(*dest_name);
      std::string errStr = "Failed move link to , " + str + " with return: " + std::to_string(err) + ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
      args.GetReturnValue().SetUndefined();
      return;
    }
  }

  void File::Delete(const v8::FunctionCallbackInfo<Value>& args) {
    // fail out if arguments are not correct
    if (args.Length() != 1 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group name")));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    File* file = ObjectWrap::Unwrap<File>(args.This());
    // delete specified group name
    String::Utf8Value group_name(args[0]->ToString());
    H5Ldelete(file->id, (*group_name), H5P_DEFAULT);
  }

  void File::Close(const v8::FunctionCallbackInfo<Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() > 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
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
    if (H5Fget_obj_count(file->id, H5F_OBJ_FILE) == 1) {
      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_GROUP)) > 0) {
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_GROUP, size, groupList.get());
        std::stringstream ss;
        ss << "H5 has not closed all groups: " << H5Fget_obj_count(file->id, H5F_OBJ_GROUP) << " H5F_OBJ_GROUPs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_ATTR)) > 0) {
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_ATTR, size, groupList.get());
        std::stringstream ss;
        ss << "H5 has not closed all attributes: " << H5Fget_obj_count(file->id, H5F_OBJ_ATTR) << " H5F_OBJ_ATTRs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATASET)) > 0) {
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_DATASET, size, groupList.get());
        std::stringstream ss;
        ss << "H5 has not closed all datasets" << H5Fget_obj_count(file->id, H5F_OBJ_DATASET) << " H5F_OBJ_DATASETs OPEN" << std::endl;
        for (int i = 0; i < (int)size; i++) {
          std::unique_ptr<char[]> buffer(new char[1024]);
          H5Iget_name(groupList[i], buffer.get(), 1024);
          ss << groupList[i] << " " << buffer.get() << std::endl;
        }
      }

      if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATATYPE)) > 0) {
        std::unique_ptr<hid_t[]> groupList(new hid_t[size]);
        H5Fget_obj_ids(file->id, H5F_OBJ_DATATYPE, size, groupList.get());
        std::stringstream ss;
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
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to close h5")));
      args.GetReturnValue().SetUndefined();
    }

    file->id = 0;
    return;
  }
};
