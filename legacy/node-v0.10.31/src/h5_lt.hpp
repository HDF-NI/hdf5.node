#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>

#include "hdf5.h"
#include "H5LTpublic.h"
#include "Float64Array.hpp"
#include "Float32Array.hpp"
#include "Int32Array.hpp"
#include "Uint32Array.hpp"
#include "Uint8Array.hpp"
#include "Int8Array.hpp"

namespace NodeHDF5 {

    class H5lt {
    private:
            static v8::Persistent<v8::FunctionTemplate> Constructor;
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::New("makeDataset"), FunctionTemplate::New(H5lt::make_dataset)->GetFunction());
        target->Set(String::New("readDataset"), FunctionTemplate::New(H5lt::read_dataset)->GetFunction());

        
    }

static v8::Handle<v8::Value> make_dataset (const v8::Arguments& args)
{
    v8::HandleScope scope;

    String::Utf8Value dset_name (args[1]->ToString());
    if(args[2]->IsString())
    {
        String::Utf8Value buffer (args[2]->ToString());
        herr_t err=H5LTmake_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)(*buffer));
        if(err<0)
        {
            ThrowException(v8::Exception::SyntaxError(String::New("failed to make char dataset")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
        }
//        v8::Local<v8::String> bufferWrap=String::New(buffer);
        return scope.Close(args.This());
    }
    hid_t type_id;
    v8::Local<v8::Object> buffer;
    if(args[2]->IsObject())
    {
        String::Utf8Value constructor_name (v8::Handle<v8::Object>::Cast(args[2])->GetConstructorName()->ToString());
//       std::cout<<(*dset_name)<<" make_dataset "<<(*constructor_name)<<" "<<v8::Handle<v8::Object>::Cast(args[2])->GetIndexedPropertiesExternalArrayDataLength ()<<std::endl;
        Local< Array > 	propertyNames=v8::Handle<v8::Object>::Cast(args[2])->GetPropertyNames ();
        for(uint32_t propertyIndex=0;propertyIndex<propertyNames->Length();propertyIndex++)
        {
//        Local<Object> prop=propertyNames->CloneElementAt(propertyIndex);
//       std::cout<<" property "<<prop->IsString()<<" "<<std::endl;
//        String::Utf8Value property_name (prop->ToString());
//       std::cout<<" property "<<(*property_name)<<" "<<std::endl;
            
        }
       if(strncmp("Float64Array", *constructor_name, 12)==0 ) //Float64Array
       {
           type_id=H5T_NATIVE_DOUBLE;
            buffer = Local<v8::Object>::Cast(args[2]);
//            std::cout<<(*dset_name)<<" make_dataset "<<(*constructor_name)<<" "<<buffer->GetIndexedPropertiesExternalArrayDataLength ()<<std::endl;
//            String::Utf8Value details (buffer->ObjectProtoToString());
//            std::cout<<(*dset_name)<<" make_dataset "<<(*constructor_name)<<" "<<(*details)<<std::endl;
       }
       else if(strncmp("Float32Array", *constructor_name, 12)==0) //Float32Array
       {
           type_id=H5T_NATIVE_FLOAT;
           buffer = Local<v8::Object>::Cast(args[2]);
       }
   //    else if(args[2]->IsInt64Array())
   //    {
   //        type_id=H5T_NATIVE_LLONG;
   //        buffer = Local<int64Array>::Cast(args[2]);
   //    }
       else if(strncmp("Int32Array", *constructor_name, 10)==0)
       {
           type_id=H5T_NATIVE_INT;
           buffer = Local<v8::Object>::Cast(args[2]);
       }
       else if(strncmp("Uint32Array", *constructor_name, 11)==0)
       {
           type_id=H5T_NATIVE_UINT;
           buffer = Local<v8::Object>::Cast(args[2]);
       }
       else if(strncmp("Int8Array", *constructor_name, 11)==0)
       {
           type_id=H5T_NATIVE_INT8;
           buffer = Local<v8::Object>::Cast(args[2]);
       }
       else if(strncmp("Uint8Array", *constructor_name, 11)==0)
       {
           type_id=H5T_NATIVE_UINT8;
           buffer = Local<v8::Object>::Cast(args[2]);
       }
       else
       {
           ThrowException(v8::Exception::SyntaxError(String::New("unsupported data type")));
   //        args.GetReturnValue().SetUndefined();
           return scope.Close(Undefined());;
       }
    }
    else
    {
        ThrowException(v8::Exception::SyntaxError(String::New("unsupported data type")));
//        args.GetReturnValue().SetUndefined();
        return scope.Close(Undefined());;
    }
    int rank=1;
    int length=0;
    if(buffer->Has(String::New("rank")))
    {
        Local<Value> rankValue=buffer->Get(String::New("rank"));
        rank=rankValue->ToInt32()->Value();
//        std::cout<<"has rank "<<rank<<std::endl;
    }
    if(buffer->Has(String::New("length")))
    {
        Local<Value> lengthValue=buffer->Get(String::New("length"));
        length=lengthValue->ToInt32()->Value();
//        std::cout<<"has length "<<length<<std::endl;
    }
    if(rank==1)
    {
        hsize_t dims[1]={(hsize_t)length};
        herr_t err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, (const unsigned char *)(buffer->GetIndexedPropertiesExternalArrayData()) );
        if(err<0)
        {
            ThrowException(v8::Exception::SyntaxError(String::New("failed to make dataset")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
        }
    }
    else if(rank==2)
    {
//        Local<Value> rankValue=buffer->Get(String::New("rows"))->ToInt32()->Value();
//        Local<Value> rankValue=buffer->Get(String::New("columns"))->ToInt32()->Value();
        hsize_t dims[2]={(hsize_t)buffer->Get(String::New("rows"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::New("columns"))->ToInt32()->Value()};
        herr_t err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, (const unsigned char *)(buffer->GetIndexedPropertiesExternalArrayData()) );
        if(err<0)
        {
            ThrowException(v8::Exception::SyntaxError(String::New("failed to make dataset")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());;
        }
    }
    else
    {
        ThrowException(v8::Exception::SyntaxError(String::New("unsupported rank")));
//        args.GetReturnValue().SetUndefined();
        return scope.Close(Undefined());;
    }
//    args.GetReturnValue().SetUndefined();
    return scope.Close(Undefined());
}

static v8::Handle<v8::Value> read_dataset (const v8::Arguments& args)
{
    v8::HandleScope scope;

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    herr_t err=H5LTget_dataset_ndims(args[0]->ToInt32()->Value(), *dset_name, &rank);
    if(err<0)
    {
        ThrowException(v8::Exception::SyntaxError(String::New("failed to find dataset rank")));
//        args.GetReturnValue().SetUndefined();
        return scope.Close(Undefined());
    }
    hsize_t values_dim[rank];
    err=H5LTget_dataset_info(args[0]->ToInt32()->Value(), *dset_name, values_dim, &class_id, &bufSize);
    if(err<0)
    {
        ThrowException(v8::Exception::SyntaxError(String::New("failed to find dataset info")));
//        args.GetReturnValue().SetUndefined();
        return scope.Close(Undefined());
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
                ThrowException(v8::Exception::SyntaxError(String::New("unsupported rank")));
//                args.GetReturnValue().SetUndefined();
                return scope.Close(Undefined());
                break;
          }
//        std::cout<<"rank "<<rank<<" "<<bufSize<<" "<<values_dim[0]<<" "<<class_id<<std::endl;
        switch(class_id)
        { 
            case H5T_STRING:
            {
                std::string buffer(theSize+1, 0);
                err=H5LTread_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)buffer.c_str());
                if(err<0)
                {
                    ThrowException(v8::Exception::SyntaxError(String::New("failed to read dataset into string")));
//                    args.GetReturnValue().SetUndefined();
                    return scope.Close(Undefined());
                }
//                std::cout<<"c side\n"<<buffer<<std::endl;
//                args.GetReturnValue().Set(String::New(buffer.c_str(), String::kNormalString, theSize));
                return scope.Close(String::New(buffer.c_str(), theSize));
            }
                break;
            default:
        
                hid_t type_id;
                Local<v8::Object> buffer;
                if(class_id==H5T_FLOAT && bufSize==8)
                {
                    type_id=H5T_NATIVE_DOUBLE;
                    buffer=Float64Array::Instantiate(ArrayBuffer::Instantiate(bufSize*theSize));
                    buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                    buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                    buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(bufSize*theSize), DontDelete);
                }
                else if(class_id==H5T_FLOAT && bufSize==4)
                {
                    type_id=H5T_NATIVE_FLOAT;
                    buffer=Float32Array::Instantiate(ArrayBuffer::Instantiate(bufSize*theSize));
                    buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                    buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                    buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(bufSize*theSize), DontDelete);
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
                        buffer=Int32Array::Instantiate(ArrayBuffer::Instantiate(bufSize*theSize));
                        buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                        buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                        buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(bufSize*theSize), DontDelete);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT;
                        buffer=Uint32Array::Instantiate(ArrayBuffer::Instantiate(bufSize*theSize));
                        buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                        buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                        buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(bufSize*theSize), DontDelete);
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
                        buffer=Int8Array::Instantiate(ArrayBuffer::Instantiate(theSize));
                        buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                        buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                        buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(theSize), DontDelete);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT8;
                        buffer=Uint8Array::Instantiate(ArrayBuffer::Instantiate(theSize));
                        buffer->Set(v8::String::NewSymbol("length"), v8::Uint32::New((unsigned int)theSize));
                        buffer->Set(v8::String::NewSymbol("byteOffset"), v8::Uint32::New((unsigned int)0));
                        buffer->Set(v8::String::NewSymbol("byteLength"), v8::Uint32::New(theSize), DontDelete);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else
                {
                          ThrowException(v8::Exception::SyntaxError(String::New("unsupported data type")));
//                          args.GetReturnValue().SetUndefined();
                          return scope.Close(Undefined());
                }
//                hsize_t dims[1]={buffer->Length()};
//                buffer->;
                err=H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, type_id, (unsigned char *)(buffer->GetIndexedPropertiesExternalArrayData()) );
                if(err<0)
                {
                    ThrowException(v8::Exception::SyntaxError(String::New("failed to read dataset")));
//                    args.GetReturnValue().SetUndefined();
                    return scope.Close(Undefined());
                }
//                std::cout<<"first "<<((double*)buffer->GetIndexedPropertiesExternalArrayData())[0]<<std::endl;
//                std::cout<<"rank "<<rank<<std::endl;
                buffer->Set(String::New("rank"), v8::Uint32::New(rank));
//                std::cout<<"rows "<<values_dim[0]<<std::endl;
                buffer->Set(String::New("rows"), v8::Uint32::New(values_dim[0]));
                if(rank>1)buffer->Set(String::New("columns"), Number::New(values_dim[1]));
//                args.GetReturnValue().Set(buffer);
//        std::cout<<"ret Float64Array  "<<std::endl;
                return scope.Close(buffer);
                break;
        }
}
    };
    Persistent<FunctionTemplate> H5lt::Constructor;
}