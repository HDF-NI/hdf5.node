
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

#include "H5Lpublic.h"

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
    if( flags & (H5F_ACC_EXCL|H5F_ACC_TRUNC|H5F_ACC_DEBUG))
    {
            plist_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_deflate(plist_id, compression);
            id= H5Fcreate(path, flags, H5P_DEFAULT, plist_id);
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
        if(id<0)
        {
        std::stringstream ss;
        ss << "Failed to open file, "<<path<<" and flags "<<flags<<" with return: " << id << ".\n";
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
        NODE_SET_PROTOTYPE_METHOD(t, "move", Move);
        NODE_SET_PROTOTYPE_METHOD(t, "delete", Delete);
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
        if (args.Length() != 1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected name, callback")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        String::Utf8Value group_name (args[0]->ToString());
        
        Local<Object> instance=Group::Instantiate(args.This());
        
        // unwrap parent object
        File* parent = ObjectWrap::Unwrap<File>(args.This());
        std::vector<std::string> trail;
        std::vector<hid_t> hidPath;
        std::istringstream buf(*group_name);
        for(std::string token; getline(buf, token, '/'); )
            if(!token.empty())trail.push_back(token);
        hid_t previous_hid = parent->getId();
        bool created=false;
        for(unsigned int index=0;index<trail.size();index++)
        {
            //check existence of stem
            if(H5Lexists(previous_hid, trail[index].c_str(), H5P_DEFAULT )){
                hid_t hid=H5Gopen(previous_hid, trail[index].c_str(), H5P_DEFAULT);
                if(hid>=0)
                {
                    if(index<trail.size()-1)hidPath.push_back(hid);
                    previous_hid=hid;
                    continue;
                }
            }
            // create group
//            std::cout<<previous_hid<<" group create  "<<trail[index]<<" in "<<parent->getFileName()<<std::endl;
            hid_t hid=H5Gcreate(previous_hid, trail[index].c_str(), H5P_DEFAULT, parent->getGcpl(), H5P_DEFAULT);
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
            if(index<trail.size()-1)hidPath.push_back(hid);
            if(index==trail.size()-1)
            {
                Group* group = new Group(hid);
                group->name.assign(trail[index].c_str());
                group->gcpl_id=H5Pcreate(H5P_GROUP_CREATE);
                herr_t err = H5Pset_link_creation_order(group->gcpl_id, args[2]->ToUint32()->IntegerValue());
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                for (std::vector<hid_t>::iterator it = hidPath.begin() ; it != hidPath.end(); ++it)
                    group->hidPath.push_back(*it);
                instance->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
                group->Wrap(instance);

                // attach various properties
//                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
                created=true;
            }
            previous_hid=hid;
        }
        if(!created)
        {
            Group* group = new Group(previous_hid);
            group->name.assign(trail[trail.size()-1].c_str());
            group->gcpl_id=H5Pcreate(H5P_GROUP_CREATE);
            herr_t err = H5Pset_link_creation_order(group->gcpl_id, args[2]->ToUint32()->IntegerValue());
            if (err < 0) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set link creation order")));
                args.GetReturnValue().SetUndefined();
                return;
            }
            for (std::vector<hid_t>::iterator it = hidPath.begin() ; it != hidPath.end(); ++it)
                group->hidPath.push_back(*it);
            instance->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
            group->Wrap(instance);

            // attach various properties
//            args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"), Number::New(v8::Isolate::GetCurrent(), group->id));
            created=true;
        }
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
        args.GetReturnValue().Set(instance);
        return;
        
    }

    void File::Move (const v8::FunctionCallbackInfo<Value>& args) {
        // fail out if arguments are not correct
        if (args.Length() !=1) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        File* file = ObjectWrap::Unwrap<File>(args.This());
        Group* otherGroup = ObjectWrap::Unwrap<Group>(args[0]->ToObject());
        herr_t err=H5Lmove(file->id, file->name.c_str(), otherGroup->id, otherGroup->name.c_str(), H5P_DEFAULT, H5P_DEFAULT);
        if (err < 0) {
            std::string errStr="Failed move link to , " + otherGroup->name + " with return: " + std::to_string(err) + ".\n";
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
            args.GetReturnValue().SetUndefined();
            return;
        }
    }

    void File::Delete (const v8::FunctionCallbackInfo<Value>& args) {
        // fail out if arguments are not correct
        if (args.Length() !=1 || !args[0]->IsString()) {
            
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected group name")));
            args.GetReturnValue().SetUndefined();
            return;
            
        }
        
        // unwrap group
        File* file = ObjectWrap::Unwrap<File>(args.This());
        // delete specified group name
        String::Utf8Value group_name (args[0]->ToString());
        H5Ldelete(file->id, (*group_name), H5P_DEFAULT);
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
        H5Pclose(file->gcpl);
        ssize_t size = 0;
        if (H5Fget_obj_count(file->id, H5F_OBJ_FILE) == 1) {
            if ((size = H5Fget_obj_count(file->id, H5F_OBJ_GROUP)) > 0) {
                std::unique_ptr < hid_t[] > groupList(new hid_t[size]);
                H5Fget_obj_ids(file->id, H5F_OBJ_GROUP, size, groupList.get());
                std::stringstream ss;
                ss << "H5 has not closed all groups: " << H5Fget_obj_count(file->id, H5F_OBJ_GROUP) << " H5F_OBJ_GROUPs OPEN" << std::endl;
                for (int i = 0; i < (int) size; i++) {
                    std::unique_ptr<char[] > buffer(new char [1024]);
                    H5Iget_name(groupList[i], buffer.get(), 1024);
                    ss << groupList[i] << " " << buffer.get() << std::endl;
                }
                std::cout<<ss.str()<<std::endl;
    //                    throw PersistenceException(ss.str());
            }
            if ((size = H5Fget_obj_count(file->id, H5F_OBJ_ATTR)) > 0) {
                std::unique_ptr < hid_t[] > groupList(new hid_t[size]);
                H5Fget_obj_ids(file->id, H5F_OBJ_ATTR, size, groupList.get());
                std::stringstream ss;
                ss << "H5 has not closed all attributes: " << H5Fget_obj_count(file->id, H5F_OBJ_ATTR) << " H5F_OBJ_ATTRs OPEN" << std::endl;
                for (int i = 0; i < (int) size; i++) {
                    std::unique_ptr<char[] > buffer(new char [1024]);
                    H5Iget_name(groupList[i], buffer.get(), 1024);
                    ss << groupList[i] << " " << buffer.get() << std::endl;
                }
                std::cout<<ss.str()<<std::endl;
    //                    throw PersistenceException(ss.str());
            }
            if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATASET)) > 0) {
                std::unique_ptr < hid_t[] > groupList(new hid_t[size]);
                H5Fget_obj_ids(file->id, H5F_OBJ_DATASET, size, groupList.get());
                std::stringstream ss;
                ss << "H5 has not closed all datasets" << H5Fget_obj_count(file->id, H5F_OBJ_DATASET) << " H5F_OBJ_DATASETs OPEN" << std::endl;
                for (int i = 0; i < (int) size; i++) {
                    std::unique_ptr<char[] > buffer(new char [1024]);
                    H5Iget_name(groupList[i], buffer.get(), 1024);
                    ss << groupList[i] << " " << buffer.get() << std::endl;
                }
                std::cout<<ss.str()<<std::endl;
    //                    throw PersistenceException(ss.str());
            }

            if ((size = H5Fget_obj_count(file->id, H5F_OBJ_DATATYPE)) > 0) {
                std::unique_ptr < hid_t[] > groupList(new hid_t[size]);
                H5Fget_obj_ids(file->id, H5F_OBJ_DATATYPE, size, groupList.get());
                std::stringstream ss;
                ss << "H5 has not closed all datatypes" << H5Fget_obj_count(file->id, H5F_OBJ_DATATYPE) << " H5F_OBJ_DATATYPEs OPEN" << std::endl;
                for (int i = 0; i < (int) size; i++) {
                    std::unique_ptr<char[] > buffer(new char [1024]);
                    H5Iget_name(groupList[i], buffer.get(), 1024);
                    ss << groupList[i] << " " << buffer.get() << std::endl;
                }
                std::cout<<ss.str()<<std::endl;
    //                    throw PersistenceException(ss.str());
            }
        }
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