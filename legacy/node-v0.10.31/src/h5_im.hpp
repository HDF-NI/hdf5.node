#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <memory>
#include <iostream>

#include "hdf5.h"
#include "H5IMpublic.h"
#include "Uint8Array.hpp"
#include "Int8Array.hpp"

namespace NodeHDF5 {

    class H5im {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::New("makeImage"), FunctionTemplate::New(H5im::make_image)->GetFunction());
        target->Set(String::New("readImage"), FunctionTemplate::New(H5im::read_image)->GetFunction());
        target->Set(String::New("isImage"), FunctionTemplate::New(H5im::is_image)->GetFunction());
        
    }

static v8::Handle<v8::Value> make_image (const v8::Arguments& args)
{
//    v8::Isolate* isolate = Isolate::GetCurrent();
    v8::HandleScope scope;

    String::Utf8Value dset_name (args[1]->ToString());
    v8::Handle<v8::Object> buffer = v8::Handle<v8::Object>::Cast(args[2]);
//    Local<Number> buffer =  Local<Number>::Cast(args[2]);
//    std::cout<<"planes "<<buffer->Get(String::New("planes"))->ToInt32()->Value()<<std::endl;
    String::Utf8Value interlace (buffer->Get(String::New("interlace"))->ToString());
    if(buffer->Get(String::New("planes"))->ToInt32()->Value()==3)
    {
        herr_t err=H5IMmake_image_24bit (args[0]->ToInt32()->Value(), *dset_name,  buffer->Get(String::New("width"))->ToInt32()->Value(), buffer->Get(String::New("height"))->ToInt32()->Value(), *interlace, (const unsigned char *)(buffer->GetIndexedPropertiesExternalArrayData()));
        if(err<0)
        {
            
        }
    }
//    args.GetReturnValue().SetUndefined();
    return scope.Close(Undefined());
}

static v8::Handle<v8::Value> read_image (const v8::Arguments& args)
{
    v8::HandleScope scope;

    String::Utf8Value dset_name (args[1]->ToString());
    hsize_t width;
    hsize_t height;
    hsize_t planes;
    char interlace[255];
    hssize_t npals;
    herr_t err=H5IMget_image_info(args[0]->ToInt32()->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
    if(err<0)
    {
        
    }
//    Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), (size_t)(planes*width*height));
//    std::unique_ptr<unsigned char[]> contentBuffer(new unsigned char[(size_t)(planes*width*height)]);
//    std::cout<<"read_image "<<(planes*width*height)<<" "<<std::endl;
    v8::Local<v8::Object> buffer = Uint8Array::Instantiate(ArrayBuffer::Instantiate(planes*width*height));
//    std::cout<<"read_image now "<<(planes*width*height)<<" "<<std::endl;
    err=H5IMread_image (args[0]->ToInt32()->Value(), *dset_name, (unsigned char *)(buffer->GetIndexedPropertiesExternalArrayData()) );
//    for(size_t index=0;index<(size_t)(planes*width*height);index++)
//    {
//        buffer->Set(index, Number::New(contentBuffer[index]));
//    }
    buffer->Set(String::New("length"), Number::New(planes*width*height));
    buffer->Set(String::New("width"), Number::New(width));
    buffer->Set(String::New("height"), Number::New(height));
    buffer->Set(String::New("planes"), Number::New(planes));
    buffer->Set(String::New("interlace"), String::New(interlace));
    buffer->Set(String::New("npals"), Number::New(npals));

//    args.GetReturnValue().Set(buffer);
    return scope.Close(buffer);
}

static v8::Handle<v8::Value> is_image (const v8::Arguments& args)
{
    v8::HandleScope scope;

    String::Utf8Value dset_name (args[1]->ToString());
    herr_t err=H5IMis_image ( args[0]->ToInt32()->Value(), *dset_name );
//    args.GetReturnValue().Set(err? true:false);
    return scope.Close(err?True():False());
}

    };
}