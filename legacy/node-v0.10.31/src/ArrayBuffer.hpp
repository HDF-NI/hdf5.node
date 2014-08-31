#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>
#include <memory>

#include "hdf5.h"
#include "H5LTpublic.h"

namespace NodeHDF5 {

    class ArrayBuffer : public node::ObjectWrap {
        friend class Float64Array;
        friend class Float32Array;
        friend class Int32Array;
        friend class Uint32Array;
        friend class Int8Array;
        friend class Uint8Array;
    private:
        unsigned long long length;
        std::unique_ptr<unsigned char[]> buffer;
            static v8::Persistent<v8::FunctionTemplate> Constructor;
            static v8::Handle<v8::Value> New (const v8::Arguments& args){

        if (args.Length() == 1 && args[0]->IsUint32()) {
                
//        std::cout<<"New  "<<args[0]->ToUint32()->IntegerValue()<<std::endl;
                ArrayBuffer* array = new ArrayBuffer(args[0]->ToUint32()->IntegerValue());
                array->Wrap(args.This());
        }
//    std::cout<<"old ArrayBuffer New "<<std::endl;
                return args.This();
            }
    public:
    ArrayBuffer(unsigned long long length): length (length), buffer(new unsigned char[length]) {
    }
    static void Initialize () {
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        
        // set properties
        t->Set(String::NewSymbol("BYTES_PER_ELEMENT"), v8::Uint32::New(8));
        t->SetClassName(String::New("ArrayBuffer"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
//	t->InstanceTemplate()->SetAccessor(String::New("learning_momentum"), GetLearningMomentum, SetLearningMomentum);
        // member method prototypes
//        NODE_SET_PROTOTYPE_METHOD(t, "get", get);
//        NODE_SET_PROTOTYPE_METHOD(t, "set", get);
//        NODE_SET_METHOD(t->InstanceTemplate(), "slice", slice);
//        NODE_SET_METHOD(t->PrototypeTemplate(), "slice", slice);
//        NODE_SET_PROTOTYPE_METHOD(t, "length", length);
        Local<Function> func=v8::FunctionTemplate::New(ArrayBuffer::slice)->GetFunction();
        func->SetName(v8::String::NewSymbol("slice"));
        t->InstanceTemplate()->Set(v8::String::NewSymbol("slice"), func);

        // initialize constructor reference
//        std::cout<<"ArrayBuffer Constructor  "<<std::endl;
        Constructor=Persistent<FunctionTemplate>::New(t);
//        std::cout<<"create  "<<std::endl;
    Constructor->SetClassName(String::NewSymbol("ArrayBuffer"));
//    Constructor->InstanceTemplate()->Set(String::NewSymbol("BYTES_PER_ELEMENT"), v8::Uint32::New(8));
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "get", get);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "set", get);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "slice", slice);
//        NODE_SET_PROTOTYPE_METHOD(Constructor, "subarray", subarray);
//
//    // object has one internal filed ( the C++ object)
    Constructor->InstanceTemplate()->SetInternalFieldCount(1);
//        std::cout<<"created  "<<std::endl;
        
    }
    
    static Handle<v8::Value> NewInstance(const v8::Arguments& args){
        HandleScope scope;
        Local<Value> argv[1] = {
                
                Uint32::New(args[0]->ToUint32()->IntegerValue())
                
        };
        Local<Object> instance = Constructor->GetFunction()->NewInstance(1,argv);
        ArrayBuffer* pThis = node::ObjectWrap::Unwrap<ArrayBuffer>(instance);
        instance->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalDoubleArray, (int)pThis->length);
        //pThis->setShape(shape);
        return scope.Close(instance);

    }

    static v8::Handle<v8::Value> slice (const v8::Arguments& args) {
        
        HandleScope scope;
        
        if (args.Length() <1 || args.Length() >2 ||!args[0]->IsUint32() || (args.Length() ==2 && !args[1]->IsUint32())) {
            return scope.Close(Undefined());
            
        }
        // unwrap array
        ArrayBuffer* array = ArrayBuffer::Unwrap<ArrayBuffer>(args.This());
        unsigned long begin =args[0]->ToUint32()->IntegerValue();
        unsigned long end=array->length;
        if(args.Length()==2)end=args[1]->ToUint32()->IntegerValue();
        Local<Object> slice=ArrayBuffer::Instantiate(std::max((unsigned long)0, end-begin-1));
        ArrayBuffer* pThis = node::ObjectWrap::Unwrap<ArrayBuffer>(slice);
        slice->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalUnsignedByteArray, (int)pThis->length);
        std::memcpy(slice->GetIndexedPropertiesExternalArrayData(), (array->buffer.get()+begin), pThis->length);
        return scope.Close(slice);
    }
    

    static Local<Object> Instantiate (unsigned long long size) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[1] = {
                
                Uint32::New(size)
                
        };

        Local<Object> instance=Local<FunctionTemplate>::New(Constructor)->GetFunction()->NewInstance(1, argv);
        ArrayBuffer* pThis = node::ObjectWrap::Unwrap<ArrayBuffer>(instance);
        instance->SetIndexedPropertiesToExternalArrayData((void * )pThis->buffer.get(), v8::kExternalUnsignedByteArray, (int)pThis->length);
        instance->Set(String::NewSymbol("byteLength"), v8::Uint32::New(pThis->length));
        // return new array instance
        return instance;
        
    }
    
    };
    Persistent<FunctionTemplate> ArrayBuffer::Constructor;
}