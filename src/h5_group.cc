#include <node.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>

#include "H5Cpp.h"
#include "hdf5.h"
#include <H5Location.h>
#include <H5Attribute.h>

namespace NodeHDF5 {
    
    using namespace v8;
    
    Group::Group(H5::Group group) : m_group (group) {
        
    }
    
    Persistent<FunctionTemplate> Group::Constructor;
    
    void Group::Initialize () {
        
        // instantiate constructor template
        Local<FunctionTemplate> t = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
        
        // set properties
        t->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Group"));
        t->InstanceTemplate()->SetInternalFieldCount(1);
        // member method prototypes
        NODE_SET_PROTOTYPE_METHOD(t, "create", Create);
        NODE_SET_PROTOTYPE_METHOD(t, "open", Open);
        NODE_SET_PROTOTYPE_METHOD(t, "refresh", Refresh);
        NODE_SET_PROTOTYPE_METHOD(t, "flush", Flush);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "getNumAttrs", GetNumAttrs);
//        NODE_SET_PROTOTYPE_METHOD(t, "getAttributeNames", GetAttributeNames);
//        NODE_SET_PROTOTYPE_METHOD(t, "readAttribute", ReadAttribute);
        NODE_SET_PROTOTYPE_METHOD(t, "getNumObjs", GetNumObjs);
        NODE_SET_PROTOTYPE_METHOD(t, "getMemberNames", GetMemberNames);
        NODE_SET_PROTOTYPE_METHOD(t, "getMemberNamesByCreationOrder", GetMemberNamesByCreationOrder);
        
        // initialize constructor reference
        Constructor.Reset(v8::Isolate::GetCurrent(), t);
        
    }
    
    void Group::New (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
        if (args.Length() == 3 && args[0]->IsString() && args[1]->IsObject()) {
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        
        // create group
        Group* group = new Group(file->FileObject()->openGroup(*group_name));
        group->gcpl.reset(new H5::PropList(H5Pcreate(H5P_GROUP_CREATE) ));
        herr_t err = H5Pset_link_creation_order(group->gcpl->getId(), args[2]->ToUint32()->IntegerValue());
        if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
            args.GetReturnValue().SetUndefined();
            return;
            }
//        group->m_group.
        group->name.assign(*group_name);
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->m_group.getId()));
        }
        else
        {
            args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), -1));
            
        }
        
        return;
        
    }
    
    void Group::Create (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        
        // create group
        Group* group = new Group(H5::Group((const hid_t)H5Gcreate(file->FileObject()->getId(), *group_name, H5P_DEFAULT, file->getGcpl(), H5P_DEFAULT)));
        group->name.assign(*group_name);
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->m_group.getId()));
        
        return;
        
    }
    
    void Group::Open (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        
        // create group
        Group* group = new Group(file->FileObject()->openGroup(*group_name));
        group->name.assign(*group_name);
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->m_group.getId()));
        
        return;
        
    }
    
    void Group::Refresh (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        uint32_t index=0;
        std::vector<std::string> holder;
        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
        }, &index, &holder);
        for(index=0;index<(uint32_t)group->m_group.getNumAttrs();index++)
        {
        H5::Attribute attr=group->m_group.openAttribute (holder[index].c_str());
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
//                std::cout<<"H5T_STRING "<<attr.getDataType().getSize ()<<" "<<attr.getDataType().fromClass()<<std::endl;
                std::string strValue(attr.getStorageSize(),'\0');
                attr.read(attr.getDataType(), strValue);
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
    
    void Group::Flush (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());

        v8::Local<v8::Array> propertyNames=args.This()->GetPropertyNames();
        for(unsigned int index=0;index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!args.This()->Get(name)->IsFunction() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0)
             {
//                std::cout<<index<<" "<<name->IsString()<<std::endl;
//                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<std::endl;
                if(args.This()->Get(name)->IsUint32())
                {
                    uint32_t value=args.This()->Get(name)->ToUint32()->Uint32Value();
                    if(group->m_group.attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        group->m_group.removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    group->m_group.createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_UINT, H5::DataSpace()).write(H5::PredType::NATIVE_UINT, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsInt32())
                {
                    int32_t value=args.This()->Get(name)->ToInt32()->Int32Value();
                    if(group->m_group.attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        group->m_group.removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    group->m_group.createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_INT, H5::DataSpace()).write(H5::PredType::NATIVE_INT, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsNumber())
                {
                    double value=args.This()->Get(name)->ToNumber()->NumberValue();
                    if(group->m_group.attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        group->m_group.removeAttr((*String::Utf8Value(name->ToString())));
                    }
                    group->m_group.createAttribute((*String::Utf8Value(name->ToString())), H5::PredType::NATIVE_DOUBLE, H5::DataSpace()).write(H5::PredType::NATIVE_DOUBLE, (void*)&value);
                    
                }
                else if(args.This()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args.This()->Get(name)->ToString())));
                    if(group->m_group.attrExists((*String::Utf8Value(name->ToString()))))
                    {
                        group->m_group.removeAttr((*String::Utf8Value(name->ToString())));
                    }
                     H5::DataSpace ds(H5S_SIMPLE);
                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
                     ds.setExtentSimple(1, &currentExtent);
                    group->m_group.createAttribute((*String::Utf8Value(name->ToString())), H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()),ds).write(H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()), value);
                    
                }
             }
        }
        
        
        return;
        
    }
    
    void Group::Close (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        group->m_group.close();
        
        return;
        
    }
    
    void Group::GetNumAttrs (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        args.GetReturnValue().Set(group->m_group.getNumAttrs());
        return;
        
    }
    
    void Group::GetAttributeNames (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->m_group.getNumAttrs());
        uint32_t index=0;
        std::vector<std::string> holder;
        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
        }, &index, &holder);
        for(index=0;index<(uint32_t)group->m_group.getNumAttrs();index++)
        {
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    void Group::ReadAttribute (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected attribute name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // store specified attribute name
        String::Utf8Value attribute_name (args[0]->ToString());
       
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        H5::Attribute attr=group->m_group.openAttribute (*attribute_name);
        switch(attr.getDataType().getClass())
        {
            case 1:
                double value;
                attr.read(attr.getDataType(), &value);
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), (*attribute_name)), Number::New(v8::Isolate::GetCurrent(), value));
                args.GetReturnValue().Set(value);
                break;
            default:
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                    args.GetReturnValue().SetUndefined();
                    return;
                break;
        }
        attr.close();
        return;
        
    }
    
    void Group::GetNumObjs (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        args.GetReturnValue().Set((uint32_t) group->m_group.getNumObjs());
        return;
        
    }
    
    void Group::GetMemberNames (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->m_group.getNumObjs());
        uint32_t index=0;
        std::vector<std::string> holder;
//        std::cout<<"group_name "<<group->name<std::endl;
//        String::Utf8Value group_name (group->name);
//            std::cout<<"group_name "<<(*group_name)<<std::endl;
//        group->m_group.iterateElems("Geometries", &index, [&](hid_t group_id, const char * member_name, void *operator_data) -> herr_t {
//            std::cout<<" "<<(*member_name)<<std::endl;
//            ((std::vector<std::string>*)operator_data)->push_back(std::string(member_name));
//            return 0;
//        }, &holder);
        for(index=0;index<(uint32_t)group->m_group.getNumObjs();index++)
        {
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), group->m_group.getObjnameByIdx(index).c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    void Group::GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->m_group.getNumObjs());
        uint32_t index=0;
        std::vector<std::string> holder;
//        std::cout<<"group_name "<<group->name<std::endl;
//        String::Utf8Value group_name (group->name);
//            std::cout<<"group_name "<<(*group_name)<<std::endl;
//        group->m_group.iterateElems("Geometries", &index, [&](hid_t group_id, const char * member_name, void *operator_data) -> herr_t {
//            std::cout<<" "<<(*member_name)<<std::endl;
//            ((std::vector<std::string>*)operator_data)->push_back(std::string(member_name));
//            return 0;
//        }, &holder);
        herr_t err;
            H5G_info_t group_info;
            if ((err = H5Gget_info(group->m_group.getId(), &group_info)) < 0) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), " has no info")));
                args.GetReturnValue().SetUndefined();
                return;
            }
        for(index=0;index<(uint32_t)group_info.nlinks;index++)
        {
            std::string datasetTitle;
            /*
             * Get size of name, add 1 for null terminator.
             */
            ssize_t size = 1 + H5Lget_name_by_idx(group->m_group.getId(), ".", H5_INDEX_CRT_ORDER,
                    H5_ITER_INC, index, NULL, 0, H5P_DEFAULT);

            /*
             * Allocate storage for name.
             */
            datasetTitle.resize(size);

            /*
             * Retrieve name, print it, and free the previously allocated
             * space.
             */
            size = H5Lget_name_by_idx(group->m_group.getId(), ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, index,
                    (char*) datasetTitle.c_str(), (size_t) size, H5P_DEFAULT);
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), datasetTitle.c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    Local<Object> Group::Instantiate (Local<Object> file) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[1] = {
                
                file
                
        };
        
        // return new group instance
        return Local<FunctionTemplate>::New(v8::Isolate::GetCurrent(), Constructor)->GetFunction()->NewInstance(1, argv);
        
    }

    Local<Object> Group::Instantiate (const char* name, Local<Object> file, unsigned long creationOrder) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[3] = {
                
                Local<Value>::New(v8::Isolate::GetCurrent(), String::NewFromUtf8(v8::Isolate::GetCurrent(), name)),
                file,
                Uint32::New(v8::Isolate::GetCurrent(), creationOrder)
                
        };
        
        // return new group instance
        return Local<FunctionTemplate>::New(v8::Isolate::GetCurrent(), Constructor)->GetFunction()->NewInstance(3, argv);
        
    }

};