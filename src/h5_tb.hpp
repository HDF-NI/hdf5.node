#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <cstring>
#include <vector>
#include <functional>
#include <exception>
#include <stdexcept>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "H5TBpublic.h"
#include "H5Dpublic.h"
#include "H5Tpublic.h"

#include "exceptions.hpp"

namespace NodeHDF5 {

  class H5tb {
  public:
    static void Initialize(Handle<Object> target) {

      // append this function to the target object
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeTable"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::make_table)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readTable"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_table)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "appendRecords"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::append_records)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeRecords"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_records)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readRecords"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_records)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "deleteRecord"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::delete_record)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "insertRecord"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::insert_record)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeFieldsName"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_fields_name)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeFieldsIndex"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::write_fields_index)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readFieldsName"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_fields_name)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readFieldsIndex"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_fields_index)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getTableInfo"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::get_table_info)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "getFieldInfo"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::get_field_info)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "deleteField"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::delete_field)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "insertField"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::insert_field)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "addRecordsFrom"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::add_records_from)->GetFunction());
      target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "combineTables"),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::combine_tables)->GetFunction());
    }

    static std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>
        prepareData(Local<v8::Array>& table) {
      hsize_t                   nrecords  = 0;
      size_t                    type_size = 0;
      std::unique_ptr<size_t[]> field_offsets(new size_t[table->Length()]);
      std::unique_ptr<size_t[]> field_sizes(new size_t[table->Length()]);
      std::unique_ptr<hid_t[]>  field_types(new hid_t[table->Length()]);
      for (uint32_t i = 0; i < table->Length(); i++) {
        if (table->Get(i)->IsFloat64Array()) {
          Local<v8::Float64Array> field = Local<v8::Float64Array>::Cast(table->Get(i));
          nrecords                      = field->Length();
          field_offsets[i]              = type_size;
          field_sizes[i]                = 8;
          type_size += 8;
          field_types[i] = H5T_NATIVE_DOUBLE;
        } else if (table->Get(i)->IsFloat32Array()) {
          Local<v8::Float32Array> field = Local<v8::Float32Array>::Cast(table->Get(i));
          nrecords                     = field->Length();
          field_offsets[i]             = type_size;
          field_sizes[i]               = 4;
          type_size += 4;
          field_types[i] = H5T_NATIVE_FLOAT;
        } else if (table->Get(i)->IsUint32Array()) {
          Local<v8::Uint32Array> field = Local<v8::Uint32Array>::Cast(table->Get(i));
          nrecords                     = field->Length();
          field_offsets[i]             = type_size;
          field_sizes[i]               = 4;
          type_size += 4;
          field_types[i] = H5T_NATIVE_UINT;
        } else if (table->Get(i)->IsInt32Array()) {
          Local<v8::Int32Array> field = Local<v8::Int32Array>::Cast(table->Get(i));
          nrecords                    = field->Length();
          field_offsets[i]            = type_size;
          field_sizes[i]              = 4;
          type_size += 4;
          field_types[i] = H5T_NATIVE_INT;
        } else if (table->Get(i)->IsUint16Array()) {
          Local<v8::Uint16Array> field = Local<v8::Uint16Array>::Cast(table->Get(i));
          nrecords                     = field->Length();
          field_offsets[i]             = type_size;
          field_sizes[i]               = 2;
          type_size += 2;
          field_types[i] = H5T_NATIVE_USHORT;
        } else if (table->Get(i)->IsInt16Array()) {
          Local<v8::Int16Array> field = Local<v8::Int16Array>::Cast(table->Get(i));
          nrecords                    = field->Length();
          field_offsets[i]            = type_size;
          field_sizes[i]              = 2;
          type_size += 2;
          field_types[i] = H5T_NATIVE_SHORT;
        } else if (table->Get(i)->IsUint8Array()) {
          Local<v8::Uint8Array> field = Local<v8::Uint8Array>::Cast(table->Get(i));
          nrecords                    = field->Length();
          field_offsets[i]            = type_size;
          field_sizes[i]              = 1;
          type_size += 1;
          field_types[i] = H5T_NATIVE_UINT8;
        } else if (table->Get(i)->IsInt8Array()) {
          Local<v8::Int8Array> field = Local<v8::Int8Array>::Cast(table->Get(i));
          nrecords                   = field->Length();
          field_offsets[i]           = type_size;
          field_sizes[i]             = 1;
          type_size += 1;
          field_types[i] = H5T_NATIVE_INT8;
        } else if (table->Get(i)->IsArray()) {
          Local<v8::Array> field = Local<v8::Array>::Cast(table->Get(i));
          nrecords               = field->Length();
          field_offsets[i]       = type_size;

          if (field->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))) { // explicit type
#ifdef LONGLONG53BITS
            hid_t type_id = toTypeMap[(H5T)field->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->Int32Value()];
            if (type_id==H5T_NATIVE_LLONG || type_id==H5T_NATIVE_ULLONG) {
              field_sizes[i] = 8;
              type_size += 8;
              field_types[i] = type_id;
            } else {
              throw Exception("unsupported data type");
            }
#else
            throw Exception("unsupported data type");
#endif

          } else { // string array
            size_t max = 0;
            for (uint32_t j = 0; j < nrecords; j++) {             
              size_t len = field->Get(j)->ToString()->Length();
              if (max < len)
                max = len;
            }
            field_sizes[i] = max;
            type_size += max;
            hid_t string_type = H5Tcopy(H5T_C_S1);
            H5Tset_size(string_type, max);
            field_types[i] = string_type;
          }
        } else {
          throw Exception("unsupported data type");
        }
      }

      std::unique_ptr<char[]> data(new char[type_size * nrecords]);
      for (uint32_t i = 0; i < table->Length(); i++) {
        if (table->Get(i)->IsFloat64Array()) {
          Local<v8::Float64Array> field = Local<v8::Float64Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            double value = field->Get(j)->NumberValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 8);
          }

        } else if (table->Get(i)->IsFloat32Array()) {
          Local<v8::Float32Array> field = Local<v8::Float32Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            float value = field->Get(j)->NumberValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 4);
          }

        } else if (table->Get(i)->IsUint32Array()) {
          Local<v8::Uint32Array> field = Local<v8::Uint32Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned int value = field->Get(j)->Uint32Value();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 4);
          }

        } else if (table->Get(i)->IsInt32Array()) {
          Local<v8::Int32Array> field = Local<v8::Int32Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned int value = field->Get(j)->Int32Value();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 4);
          }

        } else if (table->Get(i)->IsUint16Array()) {
          Local<v8::Uint16Array> field = Local<v8::Uint16Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned short value = field->Get(j)->IntegerValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 2);
          }

        } else if (table->Get(i)->IsInt16Array()) {
          Local<v8::Int16Array> field = Local<v8::Int16Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned short value = field->Get(j)->IntegerValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 2);
          }

        } else if (table->Get(i)->IsUint8Array()) {
          Local<v8::Uint8Array> field = Local<v8::Uint8Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned char value = field->Get(j)->IntegerValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 1);
          }

        } else if (table->Get(i)->IsInt8Array()) {
          Local<v8::Int8Array> field = Local<v8::Int8Array>::Cast(table->Get(i));
          for (uint32_t j = 0; j < nrecords; j++) {
            unsigned char value = field->Get(j)->IntegerValue();
            std::memcpy(&data[j * type_size + field_offsets[i]], &value, 1);
          }

        } else if (table->Get(i)->IsArray()) {
          Local<v8::Array> field = Local<v8::Array>::Cast(table->Get(i));

          if (field->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))) { // explicit type
#ifdef LONGLONG53BITS
            hid_t type_id = toTypeMap[(H5T)field->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->Int32Value()];
            if (type_id==H5T_NATIVE_LLONG) {
              for (uint32_t j = 0; j < nrecords; j++) {
                long long value = field->Get(j)->NumberValue();
                std::memcpy(&data[j * type_size + field_offsets[i]], &value, 8);
              }
            } else if(type_id==H5T_NATIVE_ULLONG) {
              for (uint32_t j = 0; j < nrecords; j++) {
                unsigned long long value = field->Get(j)->NumberValue();
                std::memcpy(&data[j * type_size + field_offsets[i]], &value, 8);
              }
            }
#else
            throw Exception("unsupported data type");
#endif

          } else { // string array
            for (uint32_t j = 0; j < nrecords; j++) {
              String::Utf8Value value(field->Get(j)->ToString());
              std::memcpy(&data[j * type_size + field_offsets[i]], (*value), H5Tget_size(field_types[i]));
            }
          }
        }
      }

      return std::
          tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>(
              nrecords, type_size, std::move(field_offsets), std::move(field_sizes), std::move(field_types), std::move(data));
    }

    static void prepareTable(hsize_t                   nrecords,
                             hsize_t                   nfields,
                             std::unique_ptr<int[]>    field_indices,
                             size_t                    type_size,
                             hid_t&                    dataset,
                             hid_t&                    dataset_type,
                             char**                    field_names,
                             std::unique_ptr<size_t[]> field_offsets,
                             std::unique_ptr<char[]>   data,
                             Local<v8::Array>&         table) {
      for (uint32_t i = 0; i < nfields; i++) {
        hid_t type = H5Tget_member_type(dataset_type, field_indices[i]);

        switch (H5Tget_class(type)) {
          case H5T_FLOAT:
            if (H5Tget_precision(type) == 64) {
              Local<ArrayBuffer>  arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 8 * nrecords);
              Local<Float64Array> buffer      = Float64Array::New(arrayBuffer, 0, nrecords);
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                          String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));

              for (uint32_t j = 0; j < nrecords; j++) {
                double value;
                std::memcpy(&value, &data[j * type_size + field_offsets[i]], 8);
                buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
              }

              table->Set(i, buffer);
            } else if (H5Tget_precision(type) == 32) {
              Local<ArrayBuffer>  arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * nrecords);
              Local<Float32Array> buffer      = Float32Array::New(arrayBuffer, 0, nrecords);
              buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                          String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));

              for (uint32_t j = 0; j < nrecords; j++) {
                float value;
                std::memcpy(&value, &data[j * type_size + field_offsets[i]], 4);
                buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
              }

              table->Set(i, buffer);
            }
            break;
          case H5T_INTEGER:
            switch (H5Tget_precision(type)) {
#ifdef LONGLONG53BITS
              case 64:
                if (H5Tget_sign(type) == H5T_SGN_NONE) {
                  Local<Array> buffer = Array::New(v8::Isolate::GetCurrent(), nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"),
                              Int32::New(v8::Isolate::GetCurrent(), toEnumMap[H5T_NATIVE_ULLONG]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    unsigned long long value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 8);
                    buffer->Set(j, Number::New(v8::Isolate::GetCurrent(), value));
                  }
                  table->Set(i, buffer);
                } else {
                  Local<Array> buffer = Array::New(v8::Isolate::GetCurrent(), nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"),
                              Int32::New(v8::Isolate::GetCurrent(), toEnumMap[H5T_NATIVE_LLONG]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    long long value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 8);
                    buffer->Set(j, Number::New(v8::Isolate::GetCurrent(), value));
                  }
                  table->Set(i, buffer);
                }
                break;
#endif
              case 32:
                if (H5Tget_sign(type) == H5T_SGN_NONE) {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * nrecords);
                  Local<Uint32Array> buffer      = Uint32Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    unsigned int value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 4);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                } else {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 4 * nrecords);
                  Local<Int32Array>  buffer      = Int32Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    int value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 4);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                }
                break;
              case 16:
                if (H5Tget_sign(type) == H5T_SGN_NONE) {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 2 * nrecords);
                  Local<Uint16Array> buffer      = Uint16Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    unsigned short value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 2);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                } else {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), 2 * nrecords);
                  Local<Int16Array>  buffer      = Int16Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    short value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 2);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                }
                break;
              case 8:
                if (H5Tget_sign(type) == H5T_SGN_NONE) {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), nrecords);
                  Local<Uint8Array>  buffer      = Uint8Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    unsigned char value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 1);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                } else {
                  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(v8::Isolate::GetCurrent(), nrecords);
                  Local<Int8Array>   buffer      = Int8Array::New(arrayBuffer, 0, nrecords);
                  buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                              String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                  for (uint32_t j = 0; j < nrecords; j++) {
                    char value;
                    std::memcpy(&value, &data[j * type_size + field_offsets[i]], 1);
                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), value));
                  }

                  table->Set(i, buffer);
                }
                break;
            }
            break;
          case H5T_STRING: {
            Local<Array> buffer = Array::New(v8::Isolate::GetCurrent(), nrecords);
            buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"),
                        String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
            for (uint32_t j = 0; j < nrecords; j++) {
              std::string cell(&data[j * type_size + field_offsets[i]], H5Tget_size(type));
              buffer->Set(j, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), cell.c_str()));
            }

            table->Set(i, buffer);
          } break;
          default: throw Exception("unsupported data type"); break;
        }
        H5Tclose(type);
      }
    }

    static void make_table(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value         table_name(args[1]->ToString());
      Local<v8::Array>          table = Local<v8::Array>::Cast(args[2]);
      std::unique_ptr<char* []> field_names(new char*[table->Length()]);
      for (unsigned int i = 0; i < table->Length(); i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(field_names.get()[i], 0, 255);
        String::Utf8Value field_name(table->Get(i)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"))->ToString());
        std::string       fieldName((*field_name));
        std::memcpy(field_names.get()[i], fieldName.c_str(), fieldName.length());
      }
      try {
        std::tuple<hsize_t,
                   size_t,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<hid_t[]>,
                   std::unique_ptr<char[]>>&& data = prepareData(table);

        hsize_t chunk_size = 10;
        int*    fill_data  = NULL;
        Int64*  idWrap     = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());

        herr_t err = H5TBmake_table((*table_name),
                                    idWrap->Value(),
                                    (*table_name),
                                    table->Length(),
                                    std::get<0>(data),
                                    std::get<1>(data),
                                    (const char**)field_names.get(),
                                    std::get<2>(data).get(),
                                    std::get<4>(data).get(),
                                    chunk_size,
                                    fill_data,
                                    0,
                                    (const void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr =
              "Failed making table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void read_table(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
      }
      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;

      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting field info , " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      size_t                 dst_size = 0;
      std::unique_ptr<int[]> field_indices(new int[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        dst_size += field_size[i];
        field_indices[i] = i;
      }

      std::unique_ptr<char[]> data(new char[type_size * nrecords]);
      err = H5TBread_table(idWrap->Value(), (*table_name), type_size, field_offsets.get(), field_size.get(), (void*)data.get());
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr =
            "Failed reading table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      hid_t                dataset      = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      hid_t                dataset_type = H5Dget_type(dataset);
      v8::Local<v8::Array> table        = v8::Array::New(v8::Isolate::GetCurrent(), nfields);
      try {
        prepareTable(nrecords,
                     nfields,
                     std::move(field_indices),
                     type_size,
                     dataset,
                     dataset_type,
                     field_names.get(),
                     std::move(field_offsets),
                     std::move(data),
                     table);
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);
      args.GetReturnValue().Set(table);
    }

    static void append_records(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      Local<v8::Array>  table = Local<v8::Array>::Cast(args[2]);
      std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&&
          data = prepareData(table);
      try {
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        herr_t err    = H5TBappend_records(idWrap->Value(),
                                        (*table_name),
                                        std::get<0>(data),
                                        std::get<1>(data),
                                        std::get<2>(data).get(),
                                        std::get<3>(data).get(),
                                        (const void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed appending to table , " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void write_records(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      Local<v8::Array>  table = Local<v8::Array>::Cast(args[3]);
      std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&&
          data = prepareData(table);
      try {
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        herr_t err    = H5TBwrite_records(idWrap->Value(),
                                       (*table_name),
                                       args[2]->Int32Value(),
                                       std::get<0>(data),
                                       std::get<1>(data),
                                       std::get<2>(data).get(),
                                       std::get<3>(data).get(),
                                       (const void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed writing to table, " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void read_records(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
      }
      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;

      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      std::unique_ptr<int[]> field_indices(new int[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_indices[i] = i;
      }

      std::unique_ptr<char[]> data(new char[type_size * args[3]->Int32Value()]);
      err = H5TBread_records(idWrap->Value(),
                             (*table_name),
                             args[2]->Int32Value(),
                             args[3]->Int32Value(),
                             type_size,
                             field_offsets.get(),
                             field_size.get(),
                             (void*)data.get());
      hid_t                dataset      = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      hid_t                dataset_type = H5Dget_type(dataset);
      v8::Local<v8::Array> table        = v8::Array::New(v8::Isolate::GetCurrent(), nfields);
      try {
        prepareTable(args[3]->Int32Value(),
                     nfields,
                     std::move(field_indices),
                     type_size,
                     dataset,
                     dataset_type,
                     field_names.get(),
                     std::move(field_offsets),
                     std::move(data),
                     table);
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);
      args.GetReturnValue().Set(table);
    }

    static void delete_record(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBdelete_record(idWrap->Value(), (*table_name), args[2]->Int32Value(), args[3]->Int32Value());
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed deleting records from table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void insert_record(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      Local<v8::Array>  table = Local<v8::Array>::Cast(args[3]);
      std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&&
          data = prepareData(table);
      try {
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        herr_t err    = H5TBinsert_record(idWrap->Value(),
                                       (*table_name),
                                       args[2]->Int32Value(),
                                       std::get<0>(data),
                                       std::get<1>(data),
                                       std::get<2>(data).get(),
                                       std::get<3>(data).get(),
                                       (void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed inserting to table, " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void write_fields_name(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value         table_name(args[1]->ToString());
      Local<v8::Array>          table = Local<v8::Array>::Cast(args[3]);
      std::unique_ptr<char* []> model_field_names(new char*[table->Length()]);
      std::string               fieldNames = "";
      for (unsigned int i = 0; i < table->Length(); i++) {
        model_field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(model_field_names.get()[i], 0, 255);
        String::Utf8Value field_name(table->Get(i)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"))->ToString());
        std::string       fieldName((*field_name));
        fieldNames += fieldName;
        std::memcpy(model_field_names.get()[i], fieldName.c_str(), fieldName.length());
        if (i < table->Length() - 1)
          fieldNames += ",";
      }
      hsize_t nfields;
      hsize_t nrecords;
      Int64*  idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t  err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(field_names.get()[i], 0, 255);
      }
      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;
      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      try {
        std::tuple<hsize_t,
                   size_t,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<hid_t[]>,
                   std::unique_ptr<char[]>>&& data = prepareData(table);

        herr_t err = H5TBwrite_fields_name(idWrap->Value(),
                                           (*table_name),
                                           (const char*)fieldNames.c_str(),
                                           args[2]->Int32Value(),
                                           std::get<0>(data),
                                           std::get<1>(data),
                                           std::get<2>(data).get(),
                                           std::get<3>(data).get(),
                                           (const void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed overwriting fields in table, " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      args.GetReturnValue().SetUndefined();
      return;
    }

    static void write_fields_index(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 5 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray() ||
          !args[4]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model, indices")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value      table_name(args[1]->ToString());
      Local<v8::Array>       table   = Local<v8::Array>::Cast(args[3]);
      Local<v8::Array>       indices = Local<v8::Array>::Cast(args[4]);
      std::unique_ptr<int[]> field_indices(new int[indices->Length()]);
      for (unsigned int i = 0; i < indices->Length(); i++) {
        field_indices[i] = indices->Get(i)->Uint32Value();
      }
      hsize_t nfields;
      hsize_t nrecords;
      Int64*  idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t  err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(field_names.get()[i], 0, 255);
      }
      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;
      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      try {
        std::tuple<hsize_t,
                   size_t,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<size_t[]>,
                   std::unique_ptr<hid_t[]>,
                   std::unique_ptr<char[]>>&& data = prepareData(table);

        herr_t err = H5TBwrite_fields_index(idWrap->Value(),
                                            (*table_name),
                                            table->Length(),
                                            field_indices.get(),
                                            args[2]->Int32Value(),
                                            std::get<0>(data),
                                            std::get<1>(data),
                                            std::get<2>(data).get(),
                                            std::get<3>(data).get(),
                                            (const void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed overwriting fields in table, " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      args.GetReturnValue().SetUndefined();
      return;
    }

    static void read_fields_name(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 5 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() ||
          !args[4]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(
            String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords, model, field names")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      Local<v8::Array>          indices    = Local<v8::Array>::Cast(args[4]);
      std::string               fieldNames = "";
      std::unique_ptr<char* []> model_field_names(new char*[indices->Length()]);
      for (unsigned int i = 0; i < indices->Length(); i++) {
        String::Utf8Value field_name(indices->Get(i)->ToString());
        std::string       fieldName((*field_name));
        model_field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(model_field_names.get()[i], 0, 255);
        std::strcpy(model_field_names.get()[i], fieldName.c_str());
        fieldNames.append(fieldName);
        if (i < indices->Length() - 1)
          fieldNames.append(",");
      }

      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(field_names.get()[i], 0, 255);
      }

      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;
      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      std::unique_ptr<size_t[]> model_field_size(new size_t[indices->Length()]);
      std::unique_ptr<size_t[]> model_field_offsets(new size_t[indices->Length()]);
      size_t                    model_type_size = 0;
      size_t                    model_size      = 0;
      hid_t                     dataset         = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      hid_t                     dataset_type    = H5Dget_type(dataset);
      std::unique_ptr<int[]>    field_indices(new int[indices->Length()]);
      for (unsigned int j = 0; j < indices->Length(); j++) {
        size_t            model_type_offset = 0;
        bool              hit               = false;
        String::Utf8Value field_name(indices->Get(j)->ToString());
        std::string       fieldName((*field_name));
        for (unsigned int i = 0; !hit && i < nfields; i++) {
          hid_t type = H5Tget_member_type(dataset_type, i);
          if (fieldName.compare(field_names.get()[i]) == 0) {
            model_field_offsets[j] = model_type_size;
            model_field_size[j]    = H5Tget_size(type);
            model_size += field_size[i];
            model_type_size += H5Tget_size(type);
            model_type_offset += H5Tget_size(type);
            field_indices[j] = i;
            hit              = true;
          }
          H5Tclose(type);
        }
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);

      std::unique_ptr<char[]> data(new char[model_type_size * args[3]->Int32Value()]);
      err = H5TBread_fields_name(idWrap->Value(),
                                 (*table_name),
                                 fieldNames.c_str(),
                                 args[2]->Int32Value(),
                                 args[3]->Int32Value(),
                                 model_type_size,
                                 model_field_offsets.get(),
                                 model_field_size.get(),
                                 (void*)data.get());
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed reading fields in table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      v8::Local<v8::Array> table = v8::Array::New(v8::Isolate::GetCurrent(), indices->Length());
      dataset                    = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      dataset_type               = H5Dget_type(dataset);
      try {
        prepareTable(args[3]->Int32Value(),
                     indices->Length(),
                     std::move(field_indices),
                     model_type_size,
                     dataset,
                     dataset_type,
                     model_field_names.get(),
                     std::move(model_field_offsets),
                     std::move(data),
                     table);
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);
      args.GetReturnValue().Set(table);
    }

    static void read_fields_index(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 5 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() ||
          !args[4]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(
            String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, nrecords, model, field indices")));
        args.GetReturnValue().SetUndefined();
        return;
      }

      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }

      Local<v8::Array>          indices    = Local<v8::Array>::Cast(args[4]);
      std::string               fieldNames = "";
      std::unique_ptr<char* []> model_field_names(new char*[indices->Length()]);
      std::unique_ptr<int[]>    field_indices(new int[indices->Length()]);
      for (unsigned int i = 0; i < indices->Length(); i++) {
        field_indices[i]           = indices->Get(i)->Uint32Value();
        model_field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(model_field_names.get()[i], 0, 255);
      }

      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
        std::memset(field_names.get()[i], 0, 255);
      }

      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;
      err = H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      std::unique_ptr<size_t[]> model_field_size(new size_t[indices->Length()]);
      std::unique_ptr<size_t[]> model_field_offsets(new size_t[indices->Length()]);
      size_t                    model_type_size = 0;
      size_t                    model_size      = 0;
      hid_t                     dataset         = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      hid_t                     dataset_type    = H5Dget_type(dataset);
      for (unsigned int j = 0; j < indices->Length(); j++) {
        size_t model_type_offset = 0;
        bool   hit               = false;
        for (unsigned int i = 0; !hit && i < nfields; i++) {
          hid_t type = H5Tget_member_type(dataset_type, i);
          if (field_indices[j] == static_cast<int>(i)) {
            std::string fieldName(field_names.get()[i]);
            fieldName += fieldName;

            if (j < indices->Length() - 1) {
              fieldName.append(",");
            }

            std::strcpy(model_field_names.get()[j], field_names.get()[i]);
            model_field_offsets[j] = model_type_size;
            model_field_size[j]    = H5Tget_size(type);
            model_size += field_size[i];
            model_type_size += H5Tget_size(type);
            model_type_offset += H5Tget_size(type);
            hit = true;
          }
          H5Tclose(type);
        }
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);

      std::unique_ptr<char[]> data(new char[model_type_size * args[3]->Int32Value()]);
      err = H5TBread_fields_index(idWrap->Value(),
                                  (*table_name),
                                  indices->Length(),
                                  field_indices.get(),
                                  args[2]->Int32Value(),
                                  args[3]->Int32Value(),
                                  model_type_size,
                                  model_field_offsets.get(),
                                  model_field_size.get(),
                                  (void*)data.get());
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed reading fields in table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      v8::Local<v8::Array> table = v8::Array::New(v8::Isolate::GetCurrent(), indices->Length());
      dataset                    = H5Dopen(idWrap->Value(), (*table_name), H5P_DEFAULT);
      dataset_type               = H5Dget_type(dataset);
      try {
        prepareTable(args[3]->Int32Value(),
                     indices->Length(),
                     std::move(field_indices),
                     model_type_size,
                     dataset,
                     dataset_type,
                     model_field_names.get(),
                     std::move(model_field_offsets),
                     std::move(data),
                     table);
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5Tclose(dataset_type);
      H5Dclose(dataset);
      args.GetReturnValue().Set(table);
    }

    static void delete_field(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, field name")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      String::Utf8Value field_name(args[2]->ToString());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBdelete_field(idWrap->Value(), (*table_name), (*field_name));
      if (err < 0) {
        std::string tableName(*table_name);
        std::string errStr = "Failed deleting field from table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void add_records_from(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 6 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsUint32() ||
          !args[4]->IsString() || !args[5]->IsUint32()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(
            String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name1, start1, nrecords, name2, start2")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name1(args[1]->ToString());
      String::Utf8Value table_name2(args[4]->ToString());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      herr_t            err    = H5TBadd_records_from(
          idWrap->Value(), (*table_name1), args[2]->Int32Value(), args[3]->Int32Value(), (*table_name2), args[5]->Int32Value());
      if (err < 0) {
        std::string tableName(*table_name1);
        std::string errStr = "Failed add records from table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void combine_tables(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 5 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsString() ||
          !args[4]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id1, name1, id2, name2, name3")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name1(args[1]->ToString());
      String::Utf8Value table_name2(args[3]->ToString());
      String::Utf8Value table_name3(args[4]->ToString());
      Int64*            idWrap  = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      Int64*            id2Wrap = ObjectWrap::Unwrap<Int64>(args[2]->ToObject());
      herr_t            err     = H5TBcombine_tables(idWrap->Value(), (*table_name1), id2Wrap->Value(), (*table_name2), (*table_name3));
      if (err < 0) {
        std::string tableName(*table_name1);
        std::string errStr = "Failed combining from table, " + tableName + " with return: " + std::to_string(err) + " " +
                             std::to_string(idWrap->Value()) + ".\n";
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void insert_field(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 4 || !args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsUint32() || !args[3]->IsArray()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, start, model")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      Local<v8::Array>  table = Local<v8::Array>::Cast(args[3]);
      String::Utf8Value field_name(table->Get(0)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"))->ToString());
      std::tuple<hsize_t, size_t, std::unique_ptr<size_t[]>, std::unique_ptr<size_t[]>, std::unique_ptr<hid_t[]>, std::unique_ptr<char[]>>&&
          data = prepareData(table);
      try {
        int*   fill_data = NULL;
        Int64* idWrap    = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        herr_t err       = H5TBinsert_field(idWrap->Value(),
                                      (*table_name),
                                      (*field_name),
                                      std::get<4>(data)[0],
                                      args[2]->Int32Value(),
                                      fill_data,
                                      (void*)std::get<5>(data).get());
        if (err < 0) {
          std::string tableName(*table_name);
          std::string errStr = "Failed inserting to table, " + tableName + " with return: " + std::to_string(err) + " " +
                               std::to_string(idWrap->Value()) + ".\n";
          v8::Isolate::GetCurrent()->ThrowException(
              v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
          args.GetReturnValue().SetUndefined();
          return;
        }
      } catch (Exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      } catch (std::exception& ex) {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.what())));
        args.GetReturnValue().SetUndefined();
        return;
      }
    }

    static void get_table_info(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      v8::Local<v8::Object> obj = v8::Object::New(v8::Isolate::GetCurrent());
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "nfields"), Uint32::New(v8::Isolate::GetCurrent(), nfields));
      obj->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "nrecords"), Uint32::New(v8::Isolate::GetCurrent(), nrecords));
      args.GetReturnValue().Set(obj);
    }

    static void get_field_info(const v8::FunctionCallbackInfo<Value>& args) {
      // fail out if arguments are not correct
      if (args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
      }
      String::Utf8Value table_name(args[1]->ToString());
      hsize_t           nfields;
      hsize_t           nrecords;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
      H5TBget_table_info(idWrap->Value(), (*table_name), &nfields, &nrecords);
      std::unique_ptr<char* []> field_names(new char*[nfields]);
      for (unsigned int i = 0; i < nfields; i++) {
        field_names.get()[i] = (char*)malloc(sizeof(char) * 255);
      }
      std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
      std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
      size_t                    type_size;
      H5TBget_field_info(idWrap->Value(), (*table_name), field_names.get(), field_size.get(), field_offsets.get(), &type_size);
      v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), nfields);

      for (unsigned int i = 0; i < nfields; i++) {
        array->Set(i, String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names.get()[i]));
      }
      args.GetReturnValue().Set(array);
    }
  };
}