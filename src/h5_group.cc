#include <node.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>

#include "file.h"
#include "group.h"
#include "H5LTpublic.h"
#include "H5PTpublic.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    Group::Group(hid_t id) : Attributes(id) {
//        m_group.setId(id);
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
        NODE_SET_PROTOTYPE_METHOD(t, "openGroup", OpenGroup);
        NODE_SET_PROTOTYPE_METHOD(t, "refresh", Refresh);
        NODE_SET_PROTOTYPE_METHOD(t, "flush", Flush);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
        NODE_SET_PROTOTYPE_METHOD(t, "getNumAttrs", GetNumAttrs);
        NODE_SET_PROTOTYPE_METHOD(t, "getNumObjs", GetNumObjs);
        NODE_SET_PROTOTYPE_METHOD(t, "getMemberNames", GetMemberNames);
        NODE_SET_PROTOTYPE_METHOD(t, "getMemberNamesByCreationOrder", GetMemberNamesByCreationOrder);
        NODE_SET_PROTOTYPE_METHOD(t, "getChildType", GetChildType);
        NODE_SET_PROTOTYPE_METHOD(t, "getDatasetType", getDatasetType);
        
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
        try
        {
        if (args.Length() == 3 && args[0]->IsString() && args[1]->IsObject()) {
            // store specified group name
            String::Utf8Value group_name (args[0]->ToString());

            // unwrap parent object
            std::string constructorName="File";
            if(constructorName.compare(*String::Utf8Value(args[1]->ToObject()->GetConstructorName()))==0)
            {

                File* parent = ObjectWrap::Unwrap<File>(args[1]->ToObject());

                // create group H5Gopen
//                Group* group = new Group(parent->FileObject()->openGroup(*group_name));
                Group* group = new Group(H5Gopen(parent->getId(), (*group_name), H5P_DEFAULT));
                group->gcpl_id=H5Pcreate(H5P_GROUP_CREATE);
                herr_t err = H5Pset_link_creation_order(group->gcpl_id, args[2]->ToUint32()->IntegerValue());
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
                    args.GetReturnValue().SetUndefined();
                    return;
                    }
        //        group->m_group.
                group->name.assign(*group_name);
                group->Wrap(args.This());

                // attach various properties
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
            }
            else
            {
               Group* parent = ObjectWrap::Unwrap<Group>(args[1]->ToObject());

                // open group
//                Group* group = new Group(parent->m_group.openGroup(*group_name));
                Group* group = new Group(H5Gopen(parent->id, (*group_name), H5P_DEFAULT));
                group->gcpl_id=H5Pcreate(H5P_GROUP_CREATE);
                herr_t err = H5Pset_link_creation_order(group->gcpl_id, args[2]->ToUint32()->IntegerValue());
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
                    args.GetReturnValue().SetUndefined();
                    return;
                    }
        //        group->m_group.
                group->name.assign(*group_name);
                group->Wrap(args.This());

                // attach various properties
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
            }
        }
        else
        {
            args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), -1));
            
        }
        }
//        catch(H5::GroupIException& ex)
//        {
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.getCDetailMsg())));
//            args.GetReturnValue().SetUndefined();
//            return;
//        }
//        catch(H5::FileIException& ex)
//        {
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ex.getCDetailMsg())));
//            args.GetReturnValue().SetUndefined();
//            return;
//        }
        catch(...)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Group open failed")));
            args.GetReturnValue().SetUndefined();
            return;
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
        
        // unwrap parent object
        File* parent = ObjectWrap::Unwrap<File>(args[1]->ToObject());
        std::vector<std::string> trail;
        std::istringstream buf(*group_name);
        for(std::string token; getline(buf, token, '/'); )
            if(!token.empty())trail.push_back(token);
        hid_t previous_hid = parent->getId();
        bool created=false;
        for(unsigned int index=0;index<trail.size();index++)
        {
            //check existence of stem
            hid_t hid=H5Gopen(previous_hid, trail[index].c_str(), H5P_DEFAULT);
            if(hid>=0)
            {
                previous_hid=hid;
                continue;
            }
            // create group
//            std::cout<<previous_hid<<" group create  "<<trail[index]<<" in "<<parent->getFileName()<<std::endl;
            hid=H5Gcreate(previous_hid, trail[index].c_str(), H5P_DEFAULT, parent->getGcpl(), H5P_DEFAULT);
            if(hid<0){
                std::cout<<"group create error num "<<H5Eget_num(H5Eget_current_stack())<<std::endl;
                //if(H5Eget_num(H5Eget_current_stack())>0)
                std::string desc;
                {
                    H5Ewalk2(H5Eget_current_stack(), H5E_WALK_UPWARD, [&](unsigned int n, const H5E_error2_t *err_desc, void *client_data) -> herr_t {
    //                std::cout<<"n="<<n<<" "<<err_desc[0].desc<<std::endl;
                    if(((std::string*)client_data)->empty())((std::string*)client_data)->assign(err_desc[0].desc, strlen(err_desc[0].desc));
                    return 0;
                }, (void*)&desc);
                }
                desc="Group create failed: "+desc;
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), desc.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            if(index==trail.size()-1)
            {
                Group* group = new Group(hid);
                group->name.assign(trail[index].c_str());
                group->Wrap(args.This());

                // attach various properties
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
                created=true;
            }
            previous_hid=hid;
        }
        if(!created)
        {
            Group* group = new Group(previous_hid);
            group->name.assign(trail[trail.size()-1].c_str());
            group->Wrap(args.This());

            // attach various properties
            args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
            created=true;
        }
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
//        Group* group = new Group(file->FileObject()->openGroup(*group_name));
        Group* group = new Group(H5Gopen(file->getId(), (*group_name), H5P_DEFAULT));
        group->name.assign(*group_name);
        group->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
        
        return;
        
    }
    
    void Group::OpenGroup (const v8::FunctionCallbackInfo<Value>& args) {
        
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

//    void Group::Refresh (const v8::FunctionCallbackInfo<Value>& args) {
//        
//        // fail out if arguments are not correct
//        if (args.Length() >0 ) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
//        // unwrap group
//        Group* group = ObjectWrap::Unwrap<Group>(args.This());
//        hsize_t index=0;
//        std::vector<std::string> holder;
//        H5Aiterate(group->id,H5_INDEX_NAME, H5_ITER_INC, &index, [&](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//            return 0;
//        }, &holder);
////        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
////            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
////        }, &index, &holder);
//        for(index=0;index<(uint32_t)group->getNumAttrs();index++)
//        {
//        hid_t attr_id = H5Aopen(group->id, holder[index].c_str(), H5P_DEFAULT);
//        hid_t attr_type=H5Aget_type(attr_id);
//        switch(H5Tget_class(attr_type))
//        {
//            case H5T_INTEGER:
//                long long intValue;
//                H5Aread(attr_id, attr_type, &intValue);
//                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
//                break;
//            case H5T_FLOAT:
//                double value;
//                H5Aread(attr_id, attr_type, &value);
//                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
//                break;
//            case H5T_STRING:
//            {
//                std::string strValue(H5Aget_storage_size(attr_id),'\0');
//                H5Aread(attr_id, attr_type, (void*)strValue.c_str());
//                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
//            }
//                break;
//            case H5T_NO_CLASS:
//            default:
////                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
////                    args.GetReturnValue().SetUndefined();
//                    return;
//                break;
//        }
//        H5Tclose(attr_type);
//        H5Aclose(attr_id);
//        }
//        
//        return;
//        
//    }
//    
//    void Group::Flush (const v8::FunctionCallbackInfo<Value>& args) {
//        
//        // fail out if arguments are not correct
//        if (args.Length() >0 ) {
//            
//            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
//            args.GetReturnValue().SetUndefined();
//            return;
//            
//        }
//        
//        // unwrap group
//        Group* group = ObjectWrap::Unwrap<Group>(args.This());
//
//        v8::Local<v8::Array> propertyNames=args.This()->GetPropertyNames();
//        for(unsigned int index=0;index<propertyNames->Length();index++)
//        {
//             v8::Local<v8::Value> name=propertyNames->Get (index);
//             if(!args.This()->Get(name)->IsFunction() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0)
//             {
////                std::cout<<index<<" "<<name->IsString()<<std::endl;
////                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<std::endl;
//                htri_t attrExists=H5Aexists(group->id, *String::Utf8Value(name->ToString()));
//                if(args.This()->Get(name)->IsUint32())
//                {
//                    uint32_t value=args.This()->Get(name)->ToUint32()->Uint32Value();
//                    if(attrExists)
//                    {
//                        H5Adelete(group->id, *String::Utf8Value(name->ToString()));
//                    }
//                    hid_t attr_type=H5Tcopy(H5T_NATIVE_UINT);
//                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
//                    hid_t attr_id=H5Acreate2(group->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
//                    if(attr_id<0)
//                    {
//                        H5Sclose(attr_space);
//                        H5Tclose(attr_type);
//                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
//                        args.GetReturnValue().SetUndefined();
//                        return;
//                        
//                    }
//                    H5Awrite(attr_id, attr_type, &value);
//                    H5Sclose(attr_space);
//                    H5Tclose(attr_type);
//                    H5Aclose(attr_id);
//                    
//                }
//                else if(args.This()->Get(name)->IsInt32())
//                {
//                    int32_t value=args.This()->Get(name)->ToInt32()->Int32Value();
//                    if(attrExists)
//                    {
//                        H5Adelete(group->id, *String::Utf8Value(name->ToString()));
//                    }
//                    hid_t attr_type=H5Tcopy(H5T_NATIVE_INT);
//                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
//                    hid_t attr_id=H5Acreate2(group->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
//                    if(attr_id<0)
//                    {
//                        H5Sclose(attr_space);
//                        H5Tclose(attr_type);
//                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
//                        args.GetReturnValue().SetUndefined();
//                        return;
//                        
//                    }
//                    H5Awrite(attr_id, attr_type, &value);
//                    H5Sclose(attr_space);
//                    H5Tclose(attr_type);
//                    H5Aclose(attr_id);
//                    
//                }
//                else if(args.This()->Get(name)->IsNumber())
//                {
//                    double value=args.This()->Get(name)->ToNumber()->NumberValue();
//                    
//                    if(attrExists)
//                    {
//                        H5Adelete(group->id, *String::Utf8Value(name->ToString()));
//                    }
//                    hid_t attr_type=H5Tcopy(H5T_NATIVE_DOUBLE);
//                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
//                    hid_t attr_id=H5Acreate2(group->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
//                    if(attr_id<0)
//                    {
//                        H5Sclose(attr_space);
//                        H5Tclose(attr_type);
//                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
//                        args.GetReturnValue().SetUndefined();
//                        return;
//                        
//                    }
//                    H5Awrite(attr_id, attr_type, &value);
//                    H5Sclose(attr_space);
//                    H5Tclose(attr_type);
//                    H5Aclose(attr_id);
//                    
//                }
//                else if(args.This()->Get(name)->IsString())
//                {
//                    std::string value((*String::Utf8Value(args.This()->Get(name)->ToString())));
//                    if(attrExists)
//                    {
//                        H5Adelete(group->id, *String::Utf8Value(name->ToString()));
//                    }
//                    hid_t attr_type=H5Tcopy(H5T_C_S1);
//                    H5Tset_size(attr_type, std::strlen(*String::Utf8Value(name->ToString())));
//                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
//                    hid_t attr_id=H5Acreate2(group->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
//                    if(attr_id<0)
//                    {
//                        H5Sclose(attr_space);
//                        H5Tclose(attr_type);
//                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
//                        args.GetReturnValue().SetUndefined();
//                        return;
//                        
//                    }
//                    H5Awrite(attr_id, attr_type, value.c_str());
//                    H5Sclose(attr_space);
//                    H5Tclose(attr_type);
//                    H5Aclose(attr_id);
//                    
//                }
//             }
//        }
//        
//        
//        return;
//        
//    }
    
    void Group::Close (const v8::FunctionCallbackInfo<Value>& args) {
        
        // fail out if arguments are not correct
        if (args.Length() >0 ) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        H5Gclose(group->id);
        
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
        
        args.GetReturnValue().Set(group->getNumAttrs());
        return;
        
    }

    hsize_t Group::getNumObjs()
    {
       H5G_info_t 		ginfo;                  /* Group information */

       herr_t ret_value = H5Gget_info(id, &ginfo);
       if(ret_value < 0){
//          throwException("getNumObjs", "H5Gget_info failed");
       }
       return (ginfo.nlinks);
    }
    
    int Group::getNumAttrs()
    {
       H5O_info_t oinfo;    /* Object info */

       H5Oget_info(id, &oinfo);
       return( (int)oinfo.num_attrs );
    }

    H5O_type_t Group::childObjType(const char* objname)
    {
        H5O_info_t objinfo;
        H5O_type_t objtype = H5O_TYPE_UNKNOWN;

        // Use C API to get information of the object
        herr_t ret_value = H5Oget_info_by_name(id, objname, &objinfo, H5P_DEFAULT);

        // Throw exception if C API returns failure
        if (ret_value < 0){
    //	throwException("childObjType", "H5Oget_info_by_name failed");
        // Return a valid type or throw an exception for unknown type
        }
        else
          switch (objinfo.type)
          {
            case H5O_TYPE_GROUP:
            case H5O_TYPE_DATASET:
            case H5O_TYPE_NAMED_DATATYPE:
                objtype = objinfo.type;
                break;
            default:
    //	    throwException("childObjType", "Unknown type of object");
                break;
          }
        return(objtype);
    }

    std::string Group::getObjnameByIdx(hsize_t idx)
    {
        // call H5Lget_name_by_idx with name as NULL to get its length
        ssize_t name_len = H5Lget_name_by_idx(id, ".", H5_INDEX_NAME, H5_ITER_INC, idx, NULL, 0, H5P_DEFAULT);
        if(name_len < 0){
//          throwException("getObjnameByIdx", "H5Lget_name_by_idx failed");
        }
        // now, allocate C buffer to get the name
        char* name_C = new char[name_len+1];
        std::memset(name_C, 0, name_len+1); // clear buffer

        name_len = H5Lget_name_by_idx(id, ".", H5_INDEX_NAME, H5_ITER_INC, idx, name_C, name_len+1, H5P_DEFAULT);

        // clean up and return the string
        std::string name = std::string(name_C);
        delete []name_C;
        return (name);
    }
    
    void Group::GetAttributeNames (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->getNumAttrs());
        hsize_t index=0;
        std::vector<std::string> holder;
        H5Aiterate(group->id,H5_INDEX_NAME, H5_ITER_INC, &index, [&](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
            return 0;
        }, &holder);
        for(index=0;index<(uint32_t)group->getNumAttrs();index++)
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
        hid_t attr_id = H5Aopen(group->id, (*attribute_name), H5P_DEFAULT);
        hid_t datatype_id=H5Aget_type( attr_id );
        switch(H5Tget_class( datatype_id ))
        {
            case 1:
                double value;
                H5Aread(attr_id, datatype_id, &value );
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), (*attribute_name)), Number::New(v8::Isolate::GetCurrent(), value));
                args.GetReturnValue().Set(value);
                break;
            default:
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                    args.GetReturnValue().SetUndefined();
                    return;
                break;
        }
         H5Tclose(datatype_id);
         H5Aclose(attr_id);
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
        H5G_info_t 		ginfo;                  /* Group information */

        herr_t ret_value = H5Gget_info(group->id, &ginfo);
     //   if(ret_value < 0)
     //      throwException("getNumObjs", "H5Gget_info failed");
        args.GetReturnValue().Set((uint32_t) ginfo.nlinks);
        return;
        
    }
    
    void Group::GetMemberNames (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
        uint32_t index=0;
        std::vector<std::string> holder;
        H5G_info_t 		ginfo;                  /* Group information */

        herr_t ret_value = H5Gget_info(group->id, &ginfo);
     //   if(ret_value < 0)
     //      throwException("getNumObjs", "H5Gget_info failed");
        args.GetReturnValue().Set((uint32_t) ginfo.nlinks);

        for(index=0;index<(uint32_t)ginfo.nlinks;index++)
        {
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), group->getObjnameByIdx(index).c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    void Group::GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
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
            if ((err = H5Gget_info(group->id, &group_info)) < 0) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), " has no info")));
                args.GetReturnValue().SetUndefined();
                return;
            }
        for(index=0;index<(uint32_t)group_info.nlinks;index++)
        {
            std::string datasetTitle;
            H5L_info_t link_buff;
            herr_t err=H5Lget_info_by_idx(group->id, ".", H5_INDEX_NAME, H5_ITER_INC, index, &link_buff, H5P_DEFAULT);
            if(err>=0)
            {
//                std::cout<<"group link_buff.corder "<<link_buff.corder_valid<<" "<<link_buff.corder<<std::endl;
                H5_index_t index_field=(link_buff.corder_valid) ? H5_INDEX_CRT_ORDER : H5_INDEX_NAME;
            /*
             * Get size of name, add 1 for null terminator.
             */
            ssize_t size = 1 + H5Lget_name_by_idx(group->id, ".", index_field,
                    H5_ITER_INC, index, NULL, 0, H5P_DEFAULT);

            /*
             * Allocate storage for name.
             */
            datasetTitle.resize(size);

            /*
             * Retrieve name, print it, and free the previously allocated
             * space.
             */
            size = H5Lget_name_by_idx(group->id, ".", index_field, H5_ITER_INC, index,
                    (char*) datasetTitle.c_str(), (size_t) size, H5P_DEFAULT);
            }
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), datasetTitle.c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    void Group::GetChildType (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        // store specified child name
        String::Utf8Value child_name (args[0]->ToString());
        args.GetReturnValue().Set((uint32_t) group->childObjType(*child_name));
        args.This()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"));
        return;
        
    }
    
    void Group::getDatasetType (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        // unwrap group
        Group* group = ObjectWrap::Unwrap<Group>(args.This());
        // store specified child name
        String::Utf8Value child_name (args[0]->ToString());
        size_t id=args.This()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"))->ToNumber()->NumberValue();
        HLType hlType=HLType::HL_TYPE_LITE;
        if(H5IMis_image(id, (*child_name))){
            hlType=HLType::HL_TYPE_IMAGE;
        }
        else{
            hid_t ds = H5Dopen(id, (*child_name), H5P_DEFAULT);
            if (ds >= 0) {
    //            if(H5PTis_valid(ds)>=0){
    //                hlType=HLType::HL_TYPE_PACKET_TABLE;
    //            }
    //            else
                {
                    hid_t type = H5Dget_type(ds);
                    switch(H5Tget_class(type))
                    {
                        case H5T_COMPOUND:
                        {
    //                std::cout<<(*child_name)<<" H5Dget_type "<<H5Tget_class(type)<<" "<<H5T_COMPOUND<<std::endl;
                            int nmembers = H5Tget_nmembers(type);
    //                        std::cout << H5Tget_nmembers(type) << " pt type=" << H5Tis_variable_str(type) << std::endl;
                            bool variableType=true;
                            for (int memberIndex = 0; memberIndex < nmembers; memberIndex++) {
                                hid_t memberType = H5Tget_member_type(type, memberIndex);
    //                            std::cout<<" H5Tget_member_type "<<H5Tget_class(memberType)<<" "<<H5T_VLEN<<" "<<H5Tis_variable_str(memberType)<<std::endl;
                                if (H5Tis_variable_str(memberType)) {
                                } else {
                                    variableType=false;
                                }
                                H5Tclose(memberType);
                            }
                            if(variableType){
                                hlType=HLType::HL_TYPE_PACKET_TABLE;
                            }
                            else {
                                hlType=HLType::HL_TYPE_TABLE;
                            }
                        }
                        break;
                        case H5T_STRING:
                            break;
                    }
                    if (type >= 0)H5Tclose(type);
                    H5Dclose(ds);
                }
            }
        }
        args.GetReturnValue().Set((uint32_t) hlType);
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

    Local<Object> Group::Instantiate (const char* name, Local<Object> parent, unsigned long creationOrder) {
        
//        HandleScope scope;
        
        // group name and parent reference
        Local<Value> argv[3] = {
                
                Local<Value>::New(v8::Isolate::GetCurrent(), String::NewFromUtf8(v8::Isolate::GetCurrent(), name)),
                parent,
                Uint32::New(v8::Isolate::GetCurrent(), creationOrder)
                
        };
        
        // return new group instance
        return Local<FunctionTemplate>::New(v8::Isolate::GetCurrent(), Constructor)->GetFunction()->NewInstance(3, argv);
        
    }

};