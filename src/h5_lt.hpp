#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>

#include "hdf5.h"
#include "H5LTpublic.h"

namespace NodeHDF5 {

    class H5lt {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::make_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset)->GetFunction());
        
    }

static void make_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    hid_t type_id;
    Local<TypedArray> buffer;
    if(args[2]->IsFloat64Array())
    {
        type_id=H5T_NATIVE_DOUBLE;
         buffer = Local<Float64Array>::Cast(args[2]);
    }
    else if(args[2]->IsFloat32Array())
    {
        type_id=H5T_NATIVE_FLOAT;
        buffer = Local<Float32Array>::Cast(args[2]);
    }
//    else if(args[2]->IsInt64Array())
//    {
//        type_id=H5T_NATIVE_LLONG;
//        buffer = Local<int64Array>::Cast(args[2]);
//    }
    else if(args[2]->IsInt32Array())
    {
        type_id=H5T_NATIVE_INT;
        buffer = Local<Int32Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint32Array())
    {
        type_id=H5T_NATIVE_UINT;
        buffer = Local<Int32Array>::Cast(args[2]);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t dims[1]={buffer->Length()};
    herr_t err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, 1, dims, type_id, buffer->Buffer()->Externalize().Data() );
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    args.GetReturnValue().SetUndefined();
}
static void read_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    herr_t err=H5LTget_dataset_ndims(args[0]->ToInt32()->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t values_dim[rank];
    err=H5LTget_dataset_info(args[0]->ToInt32()->Value(), *dset_name, values_dim, &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
          if(rank==1)
          {
              hid_t type_id;
              Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), bufSize*(size_t)values_dim[0]);
                Local<TypedArray> buffer;
                if(class_id==H5T_FLOAT && bufSize==8)
                {
                    type_id=H5T_NATIVE_DOUBLE;
                    buffer = Float64Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
                }
                else if(class_id==H5T_FLOAT && bufSize==4)
                {
                    type_id=H5T_NATIVE_FLOAT;
                    buffer = Float32Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
                }
//                else if(class_id==H5T_INTEGER && bufSize==8)
//                {
//                    type_id=H5T_NATIVE_LLONG;
//                    buffer = int64Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
//                }
                else if(class_id==H5T_INTEGER && bufSize==4)
                {
                    hid_t h=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    hid_t t=H5Dget_type(h);
                    if(H5Tget_sign(H5Dget_type(h))==H5T_SGN_2)
                    {
                        type_id=H5T_NATIVE_INT;
                        buffer = Int32Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT;
                        buffer = Uint32Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else
                {
                          v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                          args.GetReturnValue().SetUndefined();
                          return;
                }
                hsize_t dims[1]={buffer->Length()};
//                buffer->;
                err=H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, type_id, buffer->Buffer()->Externalize().Data() );
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                args.GetReturnValue().Set(buffer);
          }
          else
          {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                    args.GetReturnValue().SetUndefined();
          }
}
    };
}