
#include <iostream>

#include <node.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "file.h"
#include "group.h"

namespace NodeHDF5 {
    
    using namespace v8;
    
    File::File (const char* path) {
        
        bool exists=std::ifstream(path).good();
        if(exists)id=H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
        if(!exists && id<0)
        {
            plist_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_deflate(plist_id, compression);
            id= H5Fcreate(path, H5F_ACC_RDONLY, H5P_DEFAULT, plist_id);            
            if(id<0)
            {
            std::stringstream ss;
            ss << "Failed to create file, with return: " << id << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            error=true;
            return;
            }
        }
//        m_file = new H5::H5File(path, H5F_ACC_RDONLY);
        gcpl = H5Pcreate(H5P_GROUP_CREATE);
        if(exists)
        {
            unsigned int crt_order_flags;
//            herr_t err = H5Pget_link_creation_order(, crt_order_flags);
        }
        herr_t err = H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED);
        if (err < 0) {
            std::stringstream ss;
            ss << "Failed to set link creation order, with return: " << err << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            error=true;
            return;
        }
        
    }
    
    File::File (const char* path, unsigned int flags) {
        bool exists=std::ifstream(path).good();
        //bool exists=std::experimental::filesystem::exists(path);
        std::cout<<"flags "<<flags<<std::endl;
    if( flags & (H5F_ACC_EXCL|H5F_ACC_TRUNC|H5F_ACC_DEBUG))
    {
            plist_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_deflate(plist_id, compression);
            id= H5Fcreate(path, flags, H5P_DEFAULT, plist_id);
            std::cout<<"id "<<id<<std::endl;
            if(id<0)
            {
            std::stringstream ss;
            ss << "Failed to create file, with return: " << id << ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
            error=true;
            return;
            }
    }
    else
    {
        id=H5Fopen(path, flags, H5P_DEFAULT);
            std::cout<<exists<<" 2 id "<<id<<std::endl;
        if(id<0)
        {
        std::stringstream ss;
        ss << "Failed to create file, with return: " << id << ".\n";
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), ss.str().c_str())));
        error=true;
        return;
        }
        
    }
//        m_file = new H5::H5File(id);
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
        
        H5Fclose(id);
        
    }
    
    Persistent<FunctionTemplate> File::Constructor;
    
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
        NODE_SET_PROTOTYPE_METHOD(t, "getMemberNamesByCreationOrder", GetMemberNamesByCreationOrder);
        NODE_SET_PROTOTYPE_METHOD(t, "getChildType", GetChildType);
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
        if (args.Length() <2 && !H5Fis_hdf5(*path)) {
            
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
        if(f->error)return;
        // extend target object with file
        f->Wrap(args.This());
        
        // attach various properties
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "path"), String::NewFromUtf8(v8::Isolate::GetCurrent(), f->name.c_str()));
        hsize_t file_size;
        herr_t ret_value = H5Fget_filesize(f->id, &file_size);
        if (ret_value < 0)
        {
            std::cout<<f->id<<" H5Fget_filesize "<<file_size<<std::endl;
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "H5Fget_filesize failed")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "size"), Number::New(v8::Isolate::GetCurrent(), file_size));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "freeSpace"), Number::New(v8::Isolate::GetCurrent(), H5Fget_freespace(f->id)));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "objectCount"), Number::New(v8::Isolate::GetCurrent(), H5Fget_obj_count(f->id, H5F_OBJ_ALL)));
        args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), f->id));
        
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
        hsize_t index=0;
        std::vector<std::string> holder;
        H5Aiterate(file->id,H5_INDEX_NAME, H5_ITER_INC, &index, [&](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
            return 0;
        }, &holder);
        for(index=0;index<(uint32_t)file->getNumAttrs();index++)
        {
        hid_t attr_id = H5Aopen(file->id, holder[index].c_str(), H5P_DEFAULT);
        hid_t attr_type=H5Aget_type(attr_id);
        switch(H5Tget_class(attr_type))
        {
            case H5T_INTEGER:
                long long intValue;
                H5Aread(attr_id, attr_type, &intValue);
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                break;
            case H5T_FLOAT:
                double value;
                H5Aread(attr_id, attr_type, &value);
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                break;
            case H5T_STRING:
            {
                std::string strValue(H5Aget_storage_size(attr_id),'\0');
                H5Aread(attr_id, attr_type, (void*)strValue.c_str());
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
        H5Tclose(attr_type);
        H5Aclose(attr_id);
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
                htri_t attrExists=H5Aexists(file->id, *String::Utf8Value(name->ToString()));
                if(args.This()->Get(name)->IsUint32())
                {
                    uint32_t value=args.This()->Get(name)->ToUint32()->Uint32Value();
                    if(attrExists)
                    {
                        H5Adelete(file->id, *String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_UINT);
                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
                    hid_t attr_id=H5Acreate2(file->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;
                        
                    }
                    H5Awrite(attr_id, attr_type, &value);
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);
                    
                }
                else if(args.This()->Get(name)->IsInt32())
                {
                    int32_t value=args.This()->Get(name)->ToInt32()->Int32Value();
                    if(attrExists)
                    {
                        H5Adelete(file->id, *String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_INT);
                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
                    hid_t attr_id=H5Acreate2(file->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;
                        
                    }
                    H5Awrite(attr_id, attr_type, &value);
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);
                    
                }
                else if(args.This()->Get(name)->IsNumber())
                {
                    double value=args.This()->Get(name)->ToNumber()->NumberValue();
                    if(attrExists)
                    {
                        H5Adelete(file->id, *String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_DOUBLE);
                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
                    hid_t attr_id=H5Acreate2(file->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;
                        
                    }
                    H5Awrite(attr_id, attr_type, &value);
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);
                    
                }
                else if(args.This()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args.This()->Get(name)->ToString())));
                    if(attrExists)
                    {
                        H5Adelete(file->id, *String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_C_S1);
                    H5Tset_size(attr_type, std::strlen(*String::Utf8Value(name->ToString())));
                    hid_t attr_space=H5Screate( H5S_SCALAR ); 
                    hid_t attr_id=H5Acreate2(file->id, *String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;
                        
                    }
                    H5Awrite(attr_id, attr_type, value.c_str());
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);
                    
                }
             }
        }
        H5Fflush(file->id, H5F_SCOPE_GLOBAL);
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
        H5Fclose(file->id);
        
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
        
        args.GetReturnValue().Set(file->getNumAttrs());
        return;
        
    }

    hsize_t File::getNumObjs()
    {
       H5G_info_t 		ginfo;                  /* Group information */

       herr_t ret_value = H5Gget_info(id, &ginfo);
       if(ret_value < 0){
//          throwException("getNumObjs", "H5Gget_info failed");
       }
       return (ginfo.nlinks);
    }
    
    int File::getNumAttrs()
    {
       H5O_info_t oinfo;    /* Object info */

       H5Oget_info(id, &oinfo);
       return( (int)oinfo.num_attrs );
    }

    H5O_type_t File::childObjType(const char* objname)
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

    std::string File::getObjnameByIdx(hsize_t idx)
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
    
    void File::GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // unwrap file
        File* file = ObjectWrap::Unwrap<File>(args.This());
        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), file->getNumObjs());
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
            if ((err = H5Gget_info(file->id, &group_info)) < 0) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), " has no info")));
                args.GetReturnValue().SetUndefined();
                return;
            }
        for(index=0;index<(uint32_t)group_info.nlinks;index++)
        {
            std::string datasetTitle;
            H5L_info_t link_buff;
            herr_t err=H5Lget_info_by_idx(file->id, ".", H5_INDEX_NAME, H5_ITER_INC, index, &link_buff, H5P_DEFAULT);
            if(err>=0)
            {
                H5_index_t index_field=(link_buff.corder_valid) ? H5_INDEX_CRT_ORDER : H5_INDEX_NAME;
            /*
             * Get size of name, add 1 for null terminator.
             */
            ssize_t size = 1 + H5Lget_name_by_idx(file->id, ".",  index_field,
                    H5_ITER_INC, index, NULL, 0, H5P_DEFAULT);

            /*
             * Allocate storage for name.
             */
            datasetTitle.resize(size);

            /*
             * Retrieve name, print it, and free the previously allocated
             * space.
             */
            size = H5Lget_name_by_idx(file->id, ".",  index_field, H5_ITER_INC, index,
                    (char*) datasetTitle.c_str(), (size_t) size, H5P_DEFAULT);
            }
            array->Set(index, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), datasetTitle.c_str()));
        }
        args.GetReturnValue().Set(array);
        return;
        
    }
    
    void File::GetChildType (const v8::FunctionCallbackInfo<Value>& args) {
        
//        HandleScope scope;
        
        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        // unwrap group
        File* file = ObjectWrap::Unwrap<File>(args.This());
        // store specified child name
        String::Utf8Value child_name (args[0]->ToString());
        args.GetReturnValue().Set((uint32_t) file->childObjType(*child_name));
        return;
        
    }
    
};