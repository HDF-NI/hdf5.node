#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_buffer.h>

#include <memory>
#include <functional>

#include "file.h"
#include "group.h"
#include "int64.hpp"
#include "H5IMpublic.h"

namespace NodeHDF5 {

  class H5im {
  public:
    static void Initialize(v8::Local<v8::Object> exports) {
      v8::Isolate* isolate = exports->GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      // append this function to the target object
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeImage", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_image)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readImage", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::read_image)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "readImageRegion", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::read_image_region)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "isImage", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::is_image)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "getImageInfo", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::get_image_info)->GetFunction(context).ToLocalChecked()).Check();
      exports->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "makePalette", v8::NewStringType::kNormal).ToLocalChecked(),
                  FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_palette)->GetFunction(context).ToLocalChecked()).Check();
    }

    static void get_height(v8::Local<v8::Object> options, std::function<void(hid_t)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb((hsize_t)options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_width(v8::Local<v8::Object> options, std::function<void(hsize_t)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb((hsize_t)options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void get_planes(v8::Local<v8::Object> options, std::function<void(hsize_t)> cb) {
      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      if (options.IsEmpty()) {
        return;
      }
      
      auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kNormal).ToLocalChecked());

      if (options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
        cb((hsize_t)options->Get(context, name).ToLocalChecked()->Uint32Value(context).ToChecked());
      }
    }
    
    static void make_image(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Local<v8::Object> buffer = args[2]->ToObject(context).ToLocalChecked();
      Local<Object>     options;
      if (args.Length() >= 4 && args[3]->IsObject()) {
        options = args[3]->ToObject(context).ToLocalChecked();
      }

      String::Utf8Value interlace(isolate, buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());
      herr_t            err;
      hsize_t           dims[3];
      if (buffer->Has(context, String::NewFromUtf8(isolate, "height", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        Local<Value> heightValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked();
        dims[0]                   = heightValue->Int32Value(context).ToChecked();
      }
      else get_height(options, [&](int _height){dims[0]=_height;});      
      if (buffer->Has(context, String::NewFromUtf8(isolate, "width", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        Local<Value> widthValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked();
        dims[1]                   = widthValue->Int32Value(context).ToChecked();
      }
      else get_width(options, [&](int _width){dims[1]=_width;});      
      if (buffer->Has(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kInternalized).ToLocalChecked()).ToChecked()) {
        Local<Value> planesValue = buffer->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked();
        dims[2]                   = planesValue->Int32Value(context).ToChecked();
      }
      else get_planes(options, [&](int _planes){dims[2]=_planes;});      
      Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
     err           = H5LTmake_dataset(idWrap->Value(), *dset_name, 3, dims, H5T_NATIVE_UCHAR, (const char*)node::Buffer::Data(args[2]));
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make image dataset", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      H5LTset_attribute_string(idWrap->Value(), *dset_name, "CLASS", "IMAGE");
      H5LTset_attribute_string(idWrap->Value(), *dset_name, "IMAGE_SUBCLASS", "IMAGE_BITMAP");
      H5LTset_attribute_string(idWrap->Value(), *dset_name, "IMAGE_SUBCLASS", "IMAGE_TRUECOLOR");
      H5LTset_attribute_string(idWrap->Value(), *dset_name, "IMAGE_VERSION", "1.2");
      H5LTset_attribute_string(idWrap->Value(), *dset_name, "INTERLACE_MODE", "INTERLACE_PIXEL");
      std::unique_ptr<unsigned char[]> rangeBuffer(new unsigned char[(size_t)(2)]);
      rangeBuffer.get()[0] = (unsigned char)0;
      rangeBuffer.get()[1] = (unsigned char)255;
      H5LTset_attribute_uchar(idWrap->Value(), *dset_name, "IMAGE_MINMAXRANGE", rangeBuffer.get(), 2);
      args.GetReturnValue().SetUndefined();
    }

    static void read_image(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      hsize_t           width;
      hsize_t           height;
      hsize_t           planes;
      char              interlace[255];
      hssize_t          npals;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5IMget_image_info(idWrap->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get image info", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      std::unique_ptr<unsigned char[]> contentBuffer(new unsigned char[(size_t)(planes * width * height)]);
      err = H5LTread_dataset(idWrap->Value(), *dset_name, H5T_NATIVE_UCHAR, contentBuffer.get());
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read image", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      v8::Local<v8::Object> buffer =
          node::Buffer::Copy(v8::Isolate::GetCurrent(), (char*)contentBuffer.get(), planes * width * height).ToLocalChecked();
        if ((args.Length() == 3 && args[2]->IsFunction()) || (args.Length() == 4 && args[3]->IsFunction())) {
          const unsigned               argc = 1;
          v8::Persistent<v8::Function> callback(v8::Isolate::GetCurrent(), args[args.Length()-1].As<Function>());
          v8::Local<v8::Object> options = v8::Object::New(v8::Isolate::GetCurrent());
          options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked(), v8::Number::New(v8::Isolate::GetCurrent(), width)).Check();
          options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked(), v8::Number::New(v8::Isolate::GetCurrent(), height)).Check();
          options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kNormal).ToLocalChecked(), v8::Number::New(v8::Isolate::GetCurrent(), planes)).Check();
          options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace, v8::NewStringType::kNormal).ToLocalChecked()).Check();
          options->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals", v8::NewStringType::kNormal).ToLocalChecked(), v8::Number::New(v8::Isolate::GetCurrent(), npals)).Check();
          v8::Local<v8::Value> argv[1] = {options};
          v8::MaybeLocal<v8::Value> ret=v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)
              ->Call(v8::Isolate::GetCurrent()->GetCurrentContext(), v8::Null(v8::Isolate::GetCurrent()), argc, argv);
          if(!ret.IsEmpty() && ret.ToLocalChecked()->IsNumber()){

          }
        } else{
          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), width)).Check();
          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), height)).Check();
          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), planes)).Check();
          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace, v8::NewStringType::kNormal).ToLocalChecked()).Check();
          buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), npals)).Check();
        }

      args.GetReturnValue().Set(buffer);
    }

    static void read_image_region(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      hsize_t           width;
      hsize_t           height;
      hsize_t           planes;
      char              interlace[255];
      hssize_t          npals;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5IMget_image_info(idWrap->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get image info", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      int                        rank = 3;
      std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
      std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
      if (args.Length() != 3) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "no region properties", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      Local<Array> names = args[2]->ToObject(context).ToLocalChecked()->GetOwnPropertyNames(context).ToLocalChecked();
      for (uint32_t index = 0; index < names->Length(); index++) {
        String::Utf8Value _name(isolate, names->Get(context, index).ToLocalChecked());
        std::string       name(*_name);
        if (name.compare("start") == 0) {
          Local<Object> starts = args[2]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
          for (unsigned int arrayIndex = 0;
               arrayIndex < starts->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Uint32Value(context).ToChecked();
               arrayIndex++) {
            start.get()[arrayIndex] = starts->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
          }
        } else if (name.compare("stride") == 0) {
          Local<Object> strides = args[2]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
          for (unsigned int arrayIndex = 0;
               arrayIndex < strides->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked()->Uint32Value(context).ToChecked();
               arrayIndex++) {
            stride.get()[arrayIndex] = strides->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
          }
        } else if (name.compare("count") == 0) {
          Local<Object> counts = args[2]->ToObject(context).ToLocalChecked()->Get(context, names->Get(context, index).ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
          for (unsigned int arrayIndex = 0;
               arrayIndex < counts->Get(context, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length", v8::NewStringType::kInternalized).ToLocalChecked()).ToLocalChecked()->Uint32Value(context).ToChecked();
               arrayIndex++) {
            count.get()[arrayIndex] = counts->Get(context, arrayIndex).ToLocalChecked()->Uint32Value(context).ToChecked();
          }
        }
      }

      hid_t did          = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
      hid_t t            = H5Dget_type(did);
      hid_t type_id      = H5Tget_native_type(t, H5T_DIR_ASCEND);
      hid_t dataspace_id = H5Dget_space(did);
      hid_t memspace_id  = H5Screate_simple(rank, count.get(), NULL);
      err                = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start.get(), stride.get(), count.get(), NULL);
      if (err < 0) {
        H5Sclose(memspace_id);
        H5Sclose(dataspace_id);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      hsize_t theSize = 1;
      for (int rankIndex = 0; rankIndex < rank; rankIndex++) {
        theSize *= count.get()[rankIndex];
      }

      std::unique_ptr<unsigned char[]> contentBuffer(new unsigned char[(size_t)(theSize)]);
      err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)contentBuffer.get());

      if (err < 0) {
        H5Sclose(memspace_id);
        H5Sclose(dataspace_id);
        H5Tclose(t);
        H5Dclose(did);
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read image region", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }

      H5Tclose(t);
      H5Sclose(memspace_id);
      H5Sclose(dataspace_id);
      H5Dclose(did);

      v8::Local<v8::Object> buffer =
          node::Buffer::Copy(v8::Isolate::GetCurrent(), (char*)contentBuffer.get(), planes * count.get()[0] * count.get()[1])
              .ToLocalChecked();
      buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), count.get()[0])).Check();
      buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), count.get()[1])).Check();
      buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), planes)).Check();
      buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace, v8::NewStringType::kNormal).ToLocalChecked()).Check();
      buffer->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), npals)).Check();

      args.GetReturnValue().Set(buffer);
    }

    static void is_image(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5IMis_image(idWrap->Value(), *dset_name);
      args.GetReturnValue().Set(err ? true : false);
    }

    static void get_image_info(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      hsize_t           width;
      hsize_t           height;
      hsize_t           planes;
      char              interlace[255];
      hssize_t          npals;
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      herr_t            err    = H5IMget_image_info(idWrap->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
      if (err < 0) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::Error(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get image info", v8::NewStringType::kNormal).ToLocalChecked()));
        args.GetReturnValue().SetUndefined();
        return;
      }
      v8::Local<v8::Object> attrs = v8::Object::New(v8::Isolate::GetCurrent());
      attrs->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "width", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), width)).Check();
      attrs->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "height", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), height)).Check();
      attrs->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), planes)).Check();
      attrs->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace, v8::NewStringType::kNormal).ToLocalChecked()).Check();
      attrs->Set(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(v8::Isolate::GetCurrent(), npals)).Check();

      args.GetReturnValue().Set(attrs);
    }

    static void make_palette(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      Local<Uint8Array> buffer = Local<Uint8Array>::Cast(args[2]);
      String::Utf8Value dset_name(isolate, args[1]->ToString(context).ToLocalChecked());
      Local<Value>      rankValue = args[2]->ToObject(context).ToLocalChecked()->Get(context, String::NewFromUtf8(v8::Isolate::GetCurrent(), "size", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked();
      hsize_t           pal_dims[1]{static_cast<hsize_t>(rankValue->Int32Value(context).ToChecked())};
      Int64*            idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject(context).ToLocalChecked());
      H5IMmake_palette(idWrap->Value(), *dset_name, pal_dims, (const unsigned char*)node::Buffer::Data(buffer->ToObject(context).ToLocalChecked()));
      args.GetReturnValue().SetUndefined();
    }
  };
}