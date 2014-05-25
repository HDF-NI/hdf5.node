#include <node.h>
#include <string>
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
        NODE_SET_PROTOTYPE_METHOD(t, "getNumAttrs", GetNumAttrs);
        NODE_SET_PROTOTYPE_METHOD(t, "getAttributeNames", GetAttributeNames);
        NODE_SET_PROTOTYPE_METHOD(t, "readAttribute", ReadAttribute);
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
        for(index=0;index<group->m_group.getNumAttrs();index++)
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
        int index=0;
        std::vector<std::string> holder;
//        std::cout<<"group_name "<<group->name<std::endl;
//        String::Utf8Value group_name (group->name);
//            std::cout<<"group_name "<<(*group_name)<<std::endl;
//        group->m_group.iterateElems("Geometries", &index, [&](hid_t group_id, const char * member_name, void *operator_data) -> herr_t {
//            std::cout<<" "<<(*member_name)<<std::endl;
//            ((std::vector<std::string>*)operator_data)->push_back(std::string(member_name));
//            return 0;
//        }, &holder);
        for(index=0;index<group->m_group.getNumObjs();index++)
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
        int index=0;
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
        for(index=0;index<group_info.nlinks;index++)
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