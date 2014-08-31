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
    if(args[2]->IsString())
    {
        String::Utf8Value buffer (args[2]->ToString());
        herr_t err=H5LTmake_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)(*buffer));
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        return;
    }
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
        buffer = Local<Uint32Array>::Cast(args[2]);
    }
    else if(args[2]->IsInt8Array())
    {
        type_id=H5T_NATIVE_INT8;
        buffer = Local<Int8Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint8Array())
    {
        type_id=H5T_NATIVE_UINT8;
        buffer = Local<Uint8Array>::Cast(args[2]);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    int rank=1;
    if(buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank")))
    {
        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank=rankValue->ToInt32()->Value();
//        std::cout<<"has rank "<<rank<<std::endl;
    }
    if(rank==1)
    {
        hsize_t dims[1]={buffer->Length()};
        herr_t err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
    }
    else if(rank==2)
    {
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value();
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value();
        hsize_t dims[2]={(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value()};
        herr_t err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
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
    hsize_t theSize=bufSize;
          switch(rank)
          {
              case 2:
              theSize=values_dim[0]*values_dim[1];
              break;
              case 1:
                  theSize=values_dim[0];
                  break;
              case 0:
                  theSize=bufSize;
                  break;
              default:
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
                args.GetReturnValue().SetUndefined();
                return;
                break;
          }
        //std::cout<<"rank "<<rank<<" "<<bufSize<<" "<<values_dim[0]<<" "<<class_id<<std::endl;
        switch(class_id)
        { 
            case H5T_STRING:
            {
                std::string buffer(theSize+1, 0);
                err=H5LTread_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)buffer.c_str());
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
//                std::cout<<"c side\n"<<buffer<<std::endl;
                args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
            }
                break;
            default:
        
          hid_t type_id;
              Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), bufSize*theSize);
                Local<TypedArray> buffer;
                if(class_id==H5T_FLOAT && bufSize==8)
                {
                    type_id=H5T_NATIVE_DOUBLE;
                    buffer = Float64Array::New(arrayBuffer, 0, theSize);
                }
                else if(class_id==H5T_FLOAT && bufSize==4)
                {
                    type_id=H5T_NATIVE_FLOAT;
                    buffer = Float32Array::New(arrayBuffer, 0, theSize);
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
                        buffer = Int32Array::New(arrayBuffer, 0, theSize);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT;
                        buffer = Uint32Array::New(arrayBuffer, 0, theSize);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else if(class_id==H5T_INTEGER && bufSize==1)
                {
                    hid_t h=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    hid_t t=H5Dget_type(h);
                    if(H5Tget_sign(H5Dget_type(h))==H5T_SGN_2)
                    {
                        type_id=H5T_NATIVE_INT8;
                        buffer = Int8Array::New(arrayBuffer, 0, theSize);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT8;
                        buffer = Uint8Array::New(arrayBuffer, 0, theSize);
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
//                hsize_t dims[1]={buffer->Length()};
//                buffer->;
                err=H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, type_id, buffer->Buffer()->Externalize().Data() );
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                if(rank>1)buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim[1]));
                args.GetReturnValue().Set(buffer);
                break;
        }
}
    };
}