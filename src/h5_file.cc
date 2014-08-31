
#include <iostream>

#include <node.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "H5Cpp.h"
#include "hdf5.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    File::File (const char* path) {
        
        m_file = new H5::H5File(path, H5F_ACC_RDONLY);
        gcpl = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED);
        if (err < 0) {
            std::stringstream ss;
            ss << "Failed to set link creation order, with return: " << err << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            return;
        }
        
    }
    
    File::File (const char* path, unsigned long flags) {
        m_file = new H5::H5File(path, toAccessMap[flags]);
        gcpl = H5Pcreate(H5P_GROUP_CREATE);
        herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED);
        if (err < 0) {
            std::stringstream ss;
            ss << "Failed to set link creation order, with return: " << err << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            return;
        }
        
    }
    
    File::~File () {
        
        m_file->close();
        delete m_file;
        
    }
    
    Persistent<FunctionTemplate> File::Constructor;
    
    H5::H5File* File::FileObject() {
        
        return m_file;
        
    }

    void File::Initialize (Handle<Object> target) {
        
        HandleScope scope(v8::Isolate::GetCurrent());
        
        // instantiate constructor function template
        Local<FunctionTemplate> t = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
        t->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
        Constructor.Reset(v8::Isolate::GetCurrent(), t);
        // member method prototypes
        NODE_SET_PROTOTYPE_METHOD(t, "createGroup", CreateGroup);
        NODE_SET_PROTOTYPE_METHOD(t, "openGroup", OpenGroup);
        NODE_SET_PROTOTYPE_METHOD(t, "getNumAttrs", GetNumAttrs);
        NODE_SET_PROTOTYPE_METHOD(t, "refresh", Refresh);
        NODE_SET_PROTOTYPE_METHOD(t, "flush", Flush);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
//        Local<Function> f=t->GetFunction();
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "File"), t->GetFunction());
        
    }
    
    void File::New (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() <1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected file path")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        String::Utf8Value path (args[0]->ToString());
        
        // fail out if file is not valid hdf5
        if (args.Length() <2 && !H5::H5File::isHdf5(*path)) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "file is not hdf5 format")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        // create hdf file object
        File* f;
        if(args.Length() <2)
            f=new File(*path);
        else
            f=new File(*path,args[1]->ToUint32()->IntegerValue());
        
        // extend target object with file
        f->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "path"), String::NewFromUtf8(v8::Isolate::GetCurrent(), f->m_file->getFileName().c_str()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "size"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getFileSize()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "freeSpace"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getFreeSpace()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "objectCount"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getObjCount()));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), f->m_file->getId()));
        
        return;
        
    }
    
    void File::CreateGroup (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
//        if (args.Length() != 1 || !args[0]->IsString()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, callback")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
//        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Object> instance=Group::Instantiate(args.This());
        // create callback params
//        Local<Value> argv[2] = {
//                
//                Local<Value>::New(v8::Isolate::GetCurrent(), Null(v8::Isolate::GetCurrent())),
//                Local<Value>::New(v8::Isolate::GetCurrent(), instance)
//                
//        };
////        instance->
//        // execute callback
//        Local<Function> callback = Local<Function>::Cast(args[1]);
//        callback->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), 2, argv);
        
        args.GetReturnValue().Set(instance);
        return;
        
    }
    
    void File::OpenGroup (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() < 1 || args.Length() >2 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Object> instance=Group::Instantiate(*group_name, args.This(), args[1]->ToUint32()->Uint32Value());
        // create callback params
//        Local<Value> argv[2] = {
//                
//                Local<Value>::New(v8::Isolate::GetCurrent(), Null(v8::Isolate::GetCurrent())),
//                Local<Value>::New(v8::Isolate::GetCurrent(), Group::Instantiate(*group_name, args.This()))
//                
//        };
        
        // execute callback
//        Local<Function> callback = Local<Function>::Cast(args[1]);
//        callback->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), 2, argv);
        
        args.GetReturnValue().Set(instance);
        return;
        
    }

    void File::Refresh (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args.This());
        uint32_t index=0;
        std::vector<std::string> holder;
        file->FileObject()->iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
        }, &index, &holder);
        for(index=0;index<(uint32_t)file->FileObject()->getNumAttrs();index++)
        {
        H5::Attribute attr=file->FileObject()->openAttribute (holder[index].c_str());
        switch(attr.getDataType().getClass())
        {
            case H5T_INTEGER:
                long long intValue;
                attr.read(attr.getDataType(), &intValue);
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                break;
            case H5T_FLOAT:
                double value;
                attr.read(attr.getDataType(), &value);
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                break;
            case H5T_STRING:
            {
                std::string strValue(attr.getStorageSize(),'\0');
                attr.read(attr.getDataType(), (void*)strValue.c_str());
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
            }
                break;
            case H5T_NO_CLASS:
            default:
//                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
//                    args.GetReturnValue().SetUndefined();
                    return;
                break;
        }
        attr.close();
        }
        
        return;
        
    }
    
    void File::Flush (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
         // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args.This());
        v8::Local<v8::Array> propertyNames=args.This()->GetPropertyNames();
        std::cout<<(*String::Utf8Value(args.This()->GetConstructorName()))<<" PropertyNames "<<propertyNames->Length()<<std::endl;
        for(unsigned int index=0;index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!args.This()->Get(name)->IsFunction() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0)
             {
                std::cout<<index<<" "<<name->IsString()<<std::endl;
                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<std::endl;
                if(args.This()->Get(name)->IsUint32())
                {
                    uint32_t value=args.This()->Get(name)->ToUint32()->Uint32Value();
                    if(file->FileObject()->attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        file->FileObject()->removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    file->FileObject()->createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_UINT, H5::DataSpace()).write(H5::PredType::NATIVE_UINT, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsInt32())
                {
                    int32_t value=args.This()->Get(name)->ToInt32()->Int32Value();
                    if(file->FileObject()->attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        file->FileObject()->removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    file->FileObject()->createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_INT, H5::DataSpace()).write(H5::PredType::NATIVE_INT, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsNumber())
                {
                    double value=args.This()->Get(name)->ToNumber()->NumberValue();
                    if(file->FileObject()->attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        file->FileObject()->removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    file->FileObject()->createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_DOUBLE, H5::DataSpace()).write(H5::PredType::NATIVE_DOUBLE, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args.This()->Get(name)->ToString())));
                    if(file->FileObject()->attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        file->FileObject()->removeAttr((*String::Utf8Value(name->ToString())));
                    }
                     H5::DataSpace ds(H5S_SIMPLE);
                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
                     ds.setExtentSimple(1, &currentExtent);
                    file->FileObject()->createAttribute((*String::Utf8Value(name->ToString())), H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()),ds).write(H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()), value);
                    
                }
             }
        }
        
        return;
        
    }
    
    void File::Close (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args.This());
        file->FileObject()->close();
        
        return;
        
    }
    
    void File::GetNumAttrs (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args.This());
        
        args.GetReturnValue().Set(file->FileObject()->getNumAttrs());
        return;
        
    }
    
};