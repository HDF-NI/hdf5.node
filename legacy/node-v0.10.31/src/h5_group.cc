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
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        
        // set properties
        t->SetClassName(String::New("Group"));
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
//        std::cout<<"group Constructor  "<<std::endl;
        Constructor=Persistent<FunctionTemplate>::New(t);
//        std::cout<<"create  "<<std::endl;

    }
    
    v8::Handle<v8::Value> Group::New (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
        if (args.Length() == 3 && args[0]->IsString() && args[1]->IsObject()) {
        // store specified group name
        String::Utf8Value group_name (args[0]->ToString());
        std::string name;
        name.assign(*group_name);
//        std::cout<<"  group->name "<<name<<std::endl;
       
        // unwrap file object
        File* file = ObjectWrap::Unwrap<File>(args[1]->ToObject());
//         std::cout<<" unwrapped "<<std::endl;
        
        // create group
        Group* group = new Group(file->FileObject()->openGroup(*group_name));
//         std::cout<<" gcpl.reset "<<std::endl;
        group->gcpl.reset(new H5::PropList(H5Pcreate(H5P_GROUP_CREATE) ));
        herr_t err = H5Pset_link_creation_order(group->gcpl->getId(), args[2]->ToUint32()->IntegerValue());
//         std::cout<<" err "<<err<<std::endl;
        if (err < 0) {
            ThrowException(v8::Exception::SyntaxError(String::New("Failed to set link creation order")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
            }
//        group->m_group.
        group->name.assign(*group_name);
//        std::cout<<"  group->name "<<group->name<<std::endl;
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::New("id"), Number::New(group->m_group.getId()));
        }
        else
        {
            args.This()->Set(String::New("id"), Number::New( -1));
            
        }
        
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::Create (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
            
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
        args.This()->Set(String::New("id"), Number::New( group->m_group.getId()));
        
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::Open (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
            
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
        args.This()->Set(String::New("id"), Number::New( group->m_group.getId()));
        
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::Refresh (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected arguments")));
            return scope.Close(Undefined());
            
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
                args.This()->Set(String::New(holder[index].c_str()), Int32::New(intValue));
                break;
            case H5T_FLOAT:
                double value;
                attr.read(attr.getDataType(), &value);
                args.This()->Set(String::New(holder[index].c_str()), Number::New(value));
                break;
            case H5T_STRING:
            {
                std::string strValue(attr.getStorageSize(),'\0');
                attr.read(attr.getDataType(), (void*)strValue.c_str());
                args.This()->Set(String::New(holder[index].c_str()), String::New(strValue.c_str()));
            }
                break;
            case H5T_NO_CLASS:
            default:
//                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
//                    args.GetReturnValue().SetUndefined();
                    return scope.Close(Undefined());
                break;
        }
        attr.close();
        }
        
        return scope.Close(Undefined());
        
    }
    
    v8::Handle<v8::Value> Group::Flush (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected arguments")));

            return scope.Close(Undefined());
            
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
                     std::cout<<"currentExtent "<<currentExtent<<std::endl;
                     ds.setExtentSimple(1, &currentExtent);
                    group->m_group.createAttribute((*String::Utf8Value(name->ToString())), H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()),ds).write(H5::StrType(H5::PredType::C_S1, name->ToString()->Utf8Length()), value);
                    
                }
             }
        }
        
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::Close (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected arguments")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
            
        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        group->m_group.close();
        
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::GetNumAttrs (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
//        args.GetReturnValue().Set(group->m_group.getNumAttrs());
        return scope.Close(Uint32::New(group->m_group.getNumAttrs()));
        
    }
    
    v8::Handle<v8::Value> Group::GetAttributeNames (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(group->m_group.getNumAttrs());
        uint32_t index=0;
        std::vector<std::string> holder;
        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
        }, &index, &holder);
        for(index=0;index<(uint32_t)group->m_group.getNumAttrs();index++)
        {
            array->Set(index, v8::String::New(holder[index].c_str()));
        }
//        args.GetReturnValue().Set(array);
        return scope.Close(array);
        
    }
    
    v8::Handle<v8::Value> Group::ReadAttribute (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            ThrowException(v8::Exception::SyntaxError(String::New("expected attribute name")));
//            args.GetReturnValue().SetUndefined();
            return scope.Close(Undefined());
            
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
//                args.GetReturnValue().Set(value);
                return scope.Close(v8::NumberObject::New(value));
                break;
            default:
                    ThrowException(v8::Exception::SyntaxError(String::New("unsupported data type")));
//                    args.GetReturnValue().SetUndefined();
                    return scope.Close(Undefined());
                break;
        }
        attr.close();
        return args.This();
        
    }
    
    v8::Handle<v8::Value> Group::GetNumObjs (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // fail out if arguments are not correct
//        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsObject()) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::New("expected name, file")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
//        args.GetReturnValue().Set((uint32_t) group->m_group.getNumObjs());
        return scope.Close(v8::Uint32::New((uint32_t) group->m_group.getNumObjs()));
        
    }
    
    v8::Handle<v8::Value> Group::GetMemberNames (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(group->m_group.getNumObjs());
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
            array->Set(index, v8::String::New(group->m_group.getObjnameByIdx(index).c_str()));
        }
//        args.GetReturnValue().Set(array);
        return scope.Close(array);
        
    }
    
    v8::Handle<v8::Value> Group::GetMemberNamesByCreationOrder (const v8::Arguments& args) {
        
        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(group->m_group.getNumObjs());
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
                ThrowException(v8::Exception::SyntaxError(String::New(" has no info")));
//                args.GetReturnValue().SetUndefined();
                return scope.Close(Undefined());
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
            array->Set(index, v8::String::New(datasetTitle.c_str()));
        }
//        args.GetReturnValue().Set(array);
        return scope.Close(array);
        
    }
    
    Local<Object> Group::Instantiate (Local<Object> file) {
        
        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[1] = {
                
                file
                
        };
        // return new group instance
        return Local<FunctionTemplate>::New(Constructor)->GetFunction()->NewInstance(1, argv);
        
    }

    Local<Object> Group::Instantiate (const char* name, Local<Object> file, unsigned long creationOrder) {
        
//        HandleScope scope;
        
        // group name and file reference
        Local<Value> argv[3] = {
                
                Local<Value>::New(String::New(name)),
                file,
                Uint32::New(creationOrder)
                
        };
        
        // return new group instance
        return Local<FunctionTemplate>::New(Constructor)->GetFunction()->NewInstance(3, argv);
        
    }

};