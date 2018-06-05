#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <cstring>
#include <vector>
#include <functional>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "H5PTpublic.h"

namespace NodeHDF5 {

  class PacketTable : public node::ObjectWrap {
    friend class H5pt;

  public:
    static void Init(v8::Handle<v8::Object> exports) {
      v8::Isolate* isolate = Isolate::GetCurrent();

      // Prepare constructor template
      v8::Local<v8::FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, "PacketTable"));
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Prototype
      NODE_SET_PROTOTYPE_METHOD(tpl, "next", next);
      NODE_SET_PROTOTYPE_METHOD(tpl, "append", append);
      NODE_SET_PROTOTYPE_METHOD(tpl, "close", close);

      Constructor.Reset(v8::Isolate::GetCurrent(), tpl->GetFunction());
      exports->Set(v8::String::NewFromUtf8(isolate, "PacketTable"), tpl->GetFunction());
    }

    static Local<Object> Instantiate(hid_t packetId, int nmembers) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      Local<Object> idInstance         = Int64::Instantiate(packetId);
      Int64*        idWrap             = ObjectWrap::Unwrap<Int64>(idInstance);
      idWrap->value                    = packetId;
      const unsigned        argc       = 2;
      v8::Handle<v8::Value> argv[argc] = {idInstance, Uint32::New(isolate, nmembers)};
      return v8::Local<v8::Function>::New(isolate, Constructor)->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked();
    }

  private:
    explicit PacketTable(hid_t packetTableID, unsigned int nmembers)
        : packetTableID(packetTableID)
        , nmembers(nmembers)
        , p_data(new char*[nmembers]){

          };

    ~PacketTable(){};

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      // Invoked as constructor: `new MyObject(...)`
      hid_t        value    = args[0]->IsUndefined() ? -1 : ObjectWrap::Unwrap<Int64>(args[0]->ToObject())->Value();
      int          nmembers = args[1]->IsUndefined() ? -1 : args[1]->Int32Value();
      PacketTable* obj      = new PacketTable(value, nmembers);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }

    static void next(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      PacketTable* obj = ObjectWrap::Unwrap<PacketTable>(args.This());

      herr_t err = H5PTget_next(obj->packetTableID, (hsize_t)1, obj->p_data.get());
      if (err < 0) {
        args.GetReturnValue().Set(false);
        return;
      }
      v8::Local<v8::Object> record = v8::Object::New(v8::Isolate::GetCurrent());
      for (unsigned int index = 0; index < obj->nmembers; index++) {
        record->Set(
            args.This()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "record"))->ToObject()->GetOwnPropertyNames()->Get(index),
            String::NewFromUtf8(v8::Isolate::GetCurrent(), obj->p_data[index]));
      }
      args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "record"), record);
      args.GetReturnValue().Set(true);
    }

    static void append(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      PacketTable* obj = ObjectWrap::Unwrap<PacketTable>(args.Holder());
      obj->p_data.reset(new char*[obj->nmembers]);
      v8::Local<v8::Object> record = args.This()->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "record"))->ToObject();
      for (unsigned int index = 0; index < obj->nmembers; index++) {
        String::Utf8Value record_value(record->Get(record->GetOwnPropertyNames()->Get(index))->ToString());
        obj->p_data[index] = new char[std::strlen(*record_value) + 1];
        std::strcpy(obj->p_data[index], *record_value);
      }
      H5PTappend(obj->packetTableID, (hsize_t)1, obj->p_data.get());
      for (unsigned int index = 0; index < obj->nmembers; index++) {
        delete obj->p_data[index];
      }

      args.GetReturnValue().SetUndefined();
    }

    static void close(const v8::FunctionCallbackInfo<v8::Value>& args) {
      v8::Isolate*    isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);

      PacketTable* obj = ObjectWrap::Unwrap<PacketTable>(args.This());
      H5PTclose(obj->packetTableID);

      args.GetReturnValue().SetUndefined();
    }
    static v8::Persistent<v8::Function> Constructor;

  protected:
    hid_t                     packetTableID;
    unsigned int              nmembers;
    std::unique_ptr<char* []> p_data;
  };

  class H5pt {
  public:
    static void Initialize(Handle<Object> target) {

      // append this function to the target object
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeTable"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5pt::make_table)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readTable"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5pt::read_table)->GetFunction());
    }

    static void make_table(const v8::FunctionCallbackInfo<Value>& args) {
      String::Utf8Value table_name(args[1]->ToString());

      PacketTable*          obj        = ObjectWrap::Unwrap<PacketTable>(args[2]->ToObject());
      hid_t                 compoundID = H5Tcreate(H5T_COMPOUND, obj->nmembers * sizeof(char*));
      v8::Local<v8::Object> record     = args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "record"))->ToObject();
      hid_t                 vlenID     = H5Tcopy(H5T_C_S1);
      H5Tset_size(vlenID, H5T_VARIABLE);
      for (unsigned int index = 0; index < obj->nmembers; index++) {

        String::Utf8Value record_name(record->GetOwnPropertyNames()->Get(index)->ToString());

        H5Tinsert(compoundID, *record_name, index * sizeof(char*), vlenID);
        String::Utf8Value record_value(record->Get(record->GetOwnPropertyNames()->Get(index))->ToString());

        obj->p_data[index] = new char[std::strlen(*record_value) + 1];
        std::strcpy(obj->p_data[index], *record_value);
      }
      H5Tpack(compoundID);
      Int64* idWrap      = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      obj->packetTableID = H5PTcreate_fl(idWrap->Value(), *table_name, compoundID, (hsize_t)100, -1);
      if (obj->packetTableID == H5I_BADID) {
        H5Tclose(compoundID);
        H5Tclose(vlenID);
        std::string errStr = "Failed creating table, " + std::string(*table_name) + " with return: " + std::to_string(obj->packetTableID) +
                             " " + std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Tclose(compoundID);
      H5Tclose(vlenID);
      H5PTcreate_index(obj->packetTableID);
      H5PTappend(obj->packetTableID, (hsize_t)1, obj->p_data.get());
      for (unsigned int index = 0; index < obj->nmembers; index++) {
        delete obj->p_data[index];
      }
    }

    static void read_table(const v8::FunctionCallbackInfo<Value>& args) {
      String::Utf8Value table_name(args[1]->ToString());
      Int64*            idWrap        = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      hid_t             packetTableID = H5PTopen(idWrap->Value(), (*table_name));
      int               nmembers      = 0;
      if (packetTableID == H5I_BADID) {
        std::string errStr = "Failed opening table, " + std::string(*table_name) + " with return: " + std::to_string(packetTableID) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      /* Initialize packet table's "current record" */
      H5PTcreate_index(packetTableID);

      hsize_t nrecords = 0;

      herr_t err = H5PTget_num_packets(packetTableID, &nrecords);
      if (err < 0) {
        std::string errStr = "Failed to get size of Events table, " + std::string(*table_name) + " with return: " + std::to_string(err) +
                             " " + std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      H5PTcreate_index(packetTableID);

      v8::Local<v8::Object> record = v8::Object::New(v8::Isolate::GetCurrent());
      hid_t                 ds     = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      if (ds >= 0) {
        hid_t type = H5Dget_type(ds);
        if (type >= 0) {
          nmembers = H5Tget_nmembers(type);
          for (int memberIndex = 0; memberIndex < nmembers; memberIndex++) {
            hid_t memberType = H5Tget_member_type(type, memberIndex);
            if (H5Tis_variable_str(memberType)>0) {
              record->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), H5Tget_member_name(type, memberIndex)),
                          String::NewFromUtf8(v8::Isolate::GetCurrent(), "huh"));
            } else {
              record->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), H5Tget_member_name(type, memberIndex)),
                          String::NewFromUtf8(v8::Isolate::GetCurrent(), "huh2"));
            }
            H5Tclose(memberType);
          }
          H5Tclose(type);
        }
        H5Dclose(ds);
      }

      v8::Local<v8::Object>&& pt = PacketTable::Instantiate(packetTableID, nmembers);
      pt->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "nrecords"), Uint32::New(v8::Isolate::GetCurrent(), nrecords));
      pt->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "record"), record);
      args.GetReturnValue().Set(pt);
    }
  };
}