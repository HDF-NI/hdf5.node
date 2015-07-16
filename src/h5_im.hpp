#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_buffer.h>

#include <memory>
#include <iostream>

#include "file.h"
#include "group.h"
#include "H5IMpublic.h"

namespace NodeHDF5 {

    class H5im {
    public:
static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_image)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::read_image)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "isImage"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::is_image)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makePalette"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5im::make_palette)->GetFunction());
        
    }

static void make_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    Local<v8::Object> buffer =  args[2]->ToObject();
//    Local<Number> buffer =  Local<Number>::Cast(args[2]);
//    std::cout<<"planes "<<buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"))->ToInt32()->Value()<<std::endl;
    String::Utf8Value interlace (buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace"))->ToString());
    herr_t err;
            hsize_t dims[3]={(hsize_t) buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"))->ToInt32()->Value()};
            err=H5LTmake_dataset (args[0]->ToInt32()->Value(), *dset_name, 3, dims, H5T_NATIVE_UCHAR,  (const char *)node::Buffer::Data(args[2]) );
            if(err<0)
            {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make image dataset")));
                args.GetReturnValue().SetUndefined();
                return;
            }
            H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, "CLASS", "IMAGE");
            H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, "IMAGE_SUBCLASS", "IMAGE_BITMAP");
            H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, "IMAGE_SUBCLASS", "IMAGE_TRUECOLOR");
            H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, "IMAGE_VERSION", "1.2");
            H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, "INTERLACE_MODE", "INTERLACE_PIXEL");
//                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
//                H5Dclose(dataset);
            
//    switch(buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"))->ToInt32()->Value())
//    {
//        case 4:
//            break;
//        case 3:
//            err=H5IMmake_image_24bit (args[0]->ToInt32()->Value(), *dset_name,  buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"))->ToInt32()->Value(), buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"))->ToInt32()->Value(), *interlace, (const unsigned char *)buffer->Buffer()->Externalize().Data());
//            if(err<0)
//            {
//                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make image 24 bit")));
//                args.GetReturnValue().SetUndefined();
//                return;
//            }
//            break;
//        case 1:
//            err=H5IMmake_image_8bit (args[0]->ToInt32()->Value(), *dset_name,  buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"))->ToInt32()->Value(), buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"))->ToInt32()->Value(), (const unsigned char *)buffer->Buffer()->Externalize().Data());
//            if(err<0)
//            {
//                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make image 8 bit")));
//                args.GetReturnValue().SetUndefined();
//                return;
//            }
//            break;
//    }
    args.GetReturnValue().SetUndefined();
}

static void read_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    hsize_t width;
    hsize_t height;
    hsize_t planes;
    char interlace[255];
    hssize_t npals;
    herr_t err=H5IMget_image_info(args[0]->ToInt32()->Value(), *dset_name, &width, &height, &planes, interlace, &npals);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get image info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::unique_ptr<unsigned char[]> contentBuffer(new unsigned char[(size_t)(planes*width*height)]);
//    err=H5IMread_image (args[0]->ToInt32()->Value(), *dset_name, contentBuffer.get() );
    err=H5LTread_dataset(args[0]->ToInt32()->Value(), *dset_name, H5T_NATIVE_UCHAR, contentBuffer.get() );
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read image")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    v8::Local<v8::Object> buffer=node::Buffer::New(v8::Isolate::GetCurrent(), (const char*)contentBuffer.get(), planes*width*height);
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "width"), Number::New(v8::Isolate::GetCurrent(), width));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "height"), Number::New(v8::Isolate::GetCurrent(), height));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "planes"), Number::New(v8::Isolate::GetCurrent(), planes));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace"), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace));
    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "npals"), Number::New(v8::Isolate::GetCurrent(), npals));

    args.GetReturnValue().Set(buffer);
}

static void is_image (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    herr_t err=H5IMis_image ( args[0]->ToInt32()->Value(), *dset_name );
    args.GetReturnValue().Set(err? true:false);
}

static void make_palette (const v8::FunctionCallbackInfo<Value>& args)
{

    Local<Uint8Array> buffer =  Local<Uint8Array>::Cast(args[2]);
    String::Utf8Value dset_name (args[1]->ToString());
    Local<Value> rankValue=args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "size"));
    hsize_t pal_dims[1]{rankValue->ToInt32()->Value()};
    herr_t err=H5IMmake_palette ( args[0]->ToInt32()->Value(), *dset_name, pal_dims, (const unsigned char *)buffer->Buffer()->Externalize().Data());
    args.GetReturnValue().SetUndefined();
}

    };
}