#include <node.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <memory>
#include <functional>
#include <exception>

#include "hdf5V8.hpp"
#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "filters.hpp"
#include "H5LTpublic.h"
#include "H5PTpublic.h"
#include "H5Lpublic.h"

namespace NodeHDF5 {

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

  Persistent<Function> Filters::Constructor;

  Group::Group(hid_t id)
      : Methods(id) {
    is_open = true;
  }

  v8::Persistent<v8::Function> Group::Constructor;

  void Group::Initialize() {

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // instantiate constructor template
    Local<FunctionTemplate> t = FunctionTemplate::New(isolate, New);

    // set properties
    t->SetClassName(String::NewFromUtf8(isolate, "Group", v8::NewStringType::kInternalized).ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(
      nullptr, nullptr, nullptr, QueryCallbackDelete));

    // member method prototypes
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "create", v8::NewStringType::kInternalized).ToLocalChecked(), Create);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "open", v8::NewStringType::kInternalized).ToLocalChecked(), Open);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "openGroup", v8::NewStringType::kInternalized).ToLocalChecked(), OpenGroup);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "refresh", v8::NewStringType::kInternalized).ToLocalChecked(), Refresh);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "flush", v8::NewStringType::kInternalized).ToLocalChecked(), Flush);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "copy", v8::NewStringType::kInternalized).ToLocalChecked(), Copy);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "move", v8::NewStringType::kInternalized).ToLocalChecked(), Move);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "link", v8::NewStringType::kInternalized).ToLocalChecked(), Link);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "delete", v8::NewStringType::kInternalized).ToLocalChecked(), Delete);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "close", v8::NewStringType::kInternalized).ToLocalChecked(), Close);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getNumAttrs", v8::NewStringType::kInternalized).ToLocalChecked(), GetNumAttrs);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "getAttributeNames", v8::NewStringType::kInternalized).ToLocalChecked(), getAttributeNames);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "readAttribute", v8::NewStringType::kInternalized).ToLocalChecked(), readAttribute);
    setPrototypeMethod(isolate, t, v8::String::NewFromUtf8(isolate, "deleteAttribute", v8::NewStringType::kInternalized).ToLocalChecked(), deleteAttribute);
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

    // initialize constructor reference
    Constructor.Reset(v8::Isolate::GetCurrent(), t->GetFunction(context).ToLocalChecked());
  }

  void Group::New(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    try {
      if (args.Length() == 3 && args[0]->IsString() && args[1]->IsObject()) {
        // store specified group name
        String::Utf8Value        group_name(isolate, args[0]->ToString(context).ToLocalChecked());
        std::vector<std::string> trail;
        std::vector<hid_t>       hidPath;
        std::istringstream       buf(*group_name);
        for (std::string token; getline(buf, token, '/');)
          if (!token.empty())
            trail.push_back(token);

        hid_t previous_hid;
        // unwrap parent object
        std::string constructorName = "File";
        if (constructorName.compare(*String::Utf8Value(isolate, args[1]->ToObject(context).ToLocalChecked()->GetConstructorName())) == 0) {

          File* parent = ObjectWrap::Unwrap<File>(args[1]->ToObject(context).ToLocalChecked());
          previous_hid = parent->getId();
        } else {
          Group* parent = ObjectWrap::Unwrap<Group>(args[1]->ToObject(context).ToLocalChecked());
          previous_hid  = parent->id;
        }
        for (unsigned int index = 0; index < trail.size() - 1; index++) {
          // check existence of stem
          if (H5Lexists(previous_hid, trail[index].c_str(), H5P_DEFAULT)) {
            hid_t hid = H5Gopen(previous_hid, trail[index].c_str(), H5P_DEFAULT);
            if (hid >= 0) {
              if (index < trail.size() - 1)
                hidPath.push_back(hid);
              previous_hid = hid;
              continue;
            } else {
              std::string msg = "Group " + trail[index] + " doesn't exist";
              v8::Isolate::GetCurrent()->ThrowException(
                  v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), msg.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
              args.GetReturnValue().SetUndefined();
              return;
            }
          } else {
            std::string msg = "Group " + trail[index] + " doesn't exist";
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), msg.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
            args.GetReturnValue().SetUndefined();
            return;
          }
        }

        if (H5Lexists(previous_hid, trail[trail.size() - 1].c_str(), H5P_DEFAULT)) {
          // create group H5Gopen
          Group* group   = new Group(H5Gopen(previous_hid, trail[trail.size() - 1].c_str(), H5P_DEFAULT));
          group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
          herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->IntegerValue(context).ToChecked());
          if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(
                v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
            args.GetReturnValue().SetUndefined();
            return;
          }
         //        group->m_group.
          group->name.assign(trail[trail.size() - 1]);
          for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
            group->hidPath.push_back(*it);
          group->Wrap(args.This());

          // attach various properties
          Local<Object> instance = Int64::Instantiate(args.This(), group->id);
          Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
          idWrap->value          = group->id;
          v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
          if(ret.ToChecked()){
            
          }
        }
        else{
          Group* group   = new Group(-1);
          group->name.assign(trail[trail.size() - 1]);
          group->Wrap(args.This());
          // attach various properties
          Local<Object> instance = Int64::Instantiate(args.This(), group->id);
          Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
          idWrap->value          = group->id;
          v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
          if(ret.ToChecked()){
            
          }
        }

      } else {
        Local<Object> instance = Int64::Instantiate(args.This(), -1);
        Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
        idWrap->value          = -1;
        v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
        if(ret.ToChecked()){
          
        }
      }
    } catch (std::exception& ex) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Group open failed", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    } catch (...) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Group open failed", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    return;
  }

  void Group::Create(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified group name
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());

    // unwrap parent object
    File*                    parent = ObjectWrap::Unwrap<File>(args[1]->ToObject(context).ToLocalChecked());
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

      if (index < trail.size() - 1) {
        hidPath.push_back(hid);
      }

      if (index == trail.size() - 1) {
        Group* group = new Group(hid);
        group->name.assign(trail[index].c_str());
        group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->IntegerValue(context).ToChecked());
        if (err < 0) {
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
          args.GetReturnValue().SetUndefined();
          return;
        }
        for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
          group->hidPath.push_back(*it);
        group->Wrap(args.This());
        Local<Object> instance = Int64::Instantiate(args.This(), group->id);
        Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
        idWrap->value          = group->id;

        // attach various properties
        v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
        if(ret.ToChecked()){
          
        }
        created = true;
      }
      previous_hid = hid;
    }
    if (!created) {
      Group* group = new Group(previous_hid);
      group->name.assign(trail[trail.size() - 1].c_str());
      group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
      herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->IntegerValue(context).ToChecked());
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
        group->hidPath.push_back(*it);
      group->Wrap(args.This());
      Local<Object> instance = Int64::Instantiate(args.This(), group->id);
      Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
      idWrap->value          = group->id;

      // attach various properties
      v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
      if(ret.ToChecked()){
        
      }
      created = true;
    }
    return;
  }

  void Group::Open(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // store specified group name
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());

    std::vector<std::string> trail;
    std::vector<hid_t>       hidPath;
    std::istringstream       buf(*group_name);
    for (std::string token; getline(buf, token, '/');) {
      if (!token.empty()) {
        trail.push_back(token);
      }
    }

    hid_t previous_hid;

    // unwrap parent object
    std::string constructorName = "File";
    if (constructorName.compare(*String::Utf8Value(isolate, args[1]->ToObject(context).ToLocalChecked()->GetConstructorName())) == 0) {

      File* parent = ObjectWrap::Unwrap<File>(args[1]->ToObject(context).ToLocalChecked());
      previous_hid = parent->getId();
    } else {
      Group* parent = ObjectWrap::Unwrap<Group>(args[1]->ToObject(context).ToLocalChecked());
      previous_hid  = parent->id;
    }
    for (unsigned int index = 0; index < trail.size() - 1; index++) {
      // check existence of stem
      if (H5Lexists(previous_hid, trail[index].c_str(), H5P_DEFAULT)) {
        hid_t hid = H5Gopen(previous_hid, trail[index].c_str(), H5P_DEFAULT);
        if (hid >= 0) {
          if (index < trail.size() - 1)
            hidPath.push_back(hid);
          previous_hid = hid;
          continue;
        } else {
          std::string msg = "Group" + trail[index] + " doesn't exist";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), msg.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } else {
        std::string msg = "Group" + trail[index] + " doesn't exist";
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), msg.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }
    Group* group   = new Group(H5Gopen(previous_hid, trail[trail.size() - 1].c_str(), H5P_DEFAULT));
    group->gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
    herr_t err     = H5Pset_link_creation_order(group->gcpl_id, args[2]->IntegerValue(context).ToChecked());
    if (err < 0) {
      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
    group->name.assign(trail[trail.size() - 1]);
    for (std::vector<hid_t>::iterator it = hidPath.begin(); it != hidPath.end(); ++it)
      group->hidPath.push_back(*it);
    group->Wrap(args.This());
    Local<Object> instance = Int64::Instantiate(args.This(), group->id);
    Int64*        idWrap   = ObjectWrap::Unwrap<Int64>(instance);
    idWrap->value          = group->id;

    // attach various properties
    v8::Maybe<bool> ret = args.This()->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "id", v8::NewStringType::kInternalized).ToLocalChecked(), instance);
    if(ret.ToChecked()){
      
    }
  }

  void Group::OpenGroup(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // fail out if arguments are not correct
    if (args.Length() < 1 || args.Length() > 2 || !args[0]->IsString()) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name and optional options", v8::NewStringType::kInternalized).ToLocalChecked()));
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

  void Group::Copy(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 3) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected src name, dest group, dest name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Group*            group = ObjectWrap::Unwrap<Group>(args.This());
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value dest_name(isolate, args[2]->ToString(context).ToLocalChecked());

    herr_t err = H5Ocopy(group->id, *group_name, args[1]->IntegerValue(context).ToChecked(), *dest_name, H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) {
      std::string str(*dest_name);
      std::string errStr = "Failed move link to , " + str + " with return: " + std::to_string(err) + ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
  }

  void Group::Move(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 3) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected src name, dest group, dest name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Group*            group = ObjectWrap::Unwrap<Group>(args.This());
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value dest_name(isolate,args[2]->ToString(context).ToLocalChecked());
    Int64*            idWrap   = ObjectWrap::Unwrap<Int64>(args[1]->ToObject(context).ToLocalChecked());
    hid_t             file_id = idWrap->Value();

    herr_t err = H5Lmove(group->id, *group_name, file_id, *dest_name, H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) {
      std::string str(*dest_name);
      std::string errStr = "Failed move link to , " + str + " with return: " + std::to_string(err) + ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
  }

  void Group::Link(const v8::FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // fail out if arguments are not correct
    if (args.Length() != 3) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected src name, dest group, dest name", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Group*            group = ObjectWrap::Unwrap<Group>(args.This());
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value dest_name(isolate, args[2]->ToString(context).ToLocalChecked());
    herr_t err = H5Lcopy(group->id, *group_name, args[1]->IntegerValue(context).ToChecked(), *dest_name, H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) {
      std::string str(*dest_name);
      std::string errStr = "Failed move link to , " + str + " with return: " + std::to_string(err) + ".\n";
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str(), v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }
  }

  void Group::Delete(const v8::FunctionCallbackInfo<Value>& args) {
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
    Group* group = ObjectWrap::Unwrap<Group>(args.This());
    // delete specified group name
    String::Utf8Value group_name(isolate, args[0]->ToString(context).ToLocalChecked());
    H5Ldelete(group->id, (*group_name), H5P_DEFAULT);
  }

  void Group::Close(const v8::FunctionCallbackInfo<Value>& args) {

    // fail out if arguments are not correct
    if (args.Length() > 0) {

      v8::Isolate::GetCurrent()->ThrowException(
          v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments", v8::NewStringType::kInternalized).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
      return;
    }

    // unwrap group
    Group* group = ObjectWrap::Unwrap<Group>(args.This());
    if (!group->is_open) {
      return;
    }

    for (std::vector<hid_t>::iterator it = group->hidPath.begin(); it != group->hidPath.end(); ++it) {
      H5Gclose(*it);
    }

    H5Pclose(group->gcpl_id);
    herr_t err = H5Gclose(group->id);
    if (err < 0) {
      return;
    }

    group->is_open = false;
  }

  Local<Object> Group::Instantiate(Local<Object> file) {
    auto isolate = v8::Isolate::GetCurrent();
    // group name and file reference
    Local<Value> argv[1] = {file};

    // return new group instance
    return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 1, argv).ToLocalChecked();
  }

  Local<Object> Group::Instantiate(const char* name, Local<Object> parent, unsigned long creationOrder) {
    auto isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    // group name and parent reference
    Local<Value> argv[3] = {Local<Value>::New(isolate, String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked()), parent, Uint32::New(isolate, creationOrder)};

    Local<Object> tmp;
    Local<Value> value;
    auto instance = v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), 3, argv);
    bool toCheck = instance.ToLocal(&value);
    // unwrap group
    Group* group = ObjectWrap::Unwrap<Group>(value->ToObject(context).ToLocalChecked());
    if (toCheck && group->id>=0) {
      // return new group instance
      return instance.ToLocalChecked();
    } else {
      // return empty
      std::stringstream ss;
      ss << "Failed to read group. Group "<<(name)<< " doesn't exist.";
      throw  Exception(ss.str());
      return tmp;
    }
  }
};
