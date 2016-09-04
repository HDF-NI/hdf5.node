
#include "hdf5.h"
#include "hdf5_hl.h"

#include "attributes.hpp"
#include "methods.hpp"
#include "filters.hpp"
#include "H5LTpublic.h"
#include "H5PTpublic.h"
#include "H5Lpublic.h"

namespace NodeHDF5 {

    void Methods::GetNumAttrs (const v8::FunctionCallbackInfo<v8::Value>& args) {

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
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

        args.GetReturnValue().Set(group->getNumAttrs());
        return;

    }

    void Methods::GetAttributeNames (const v8::FunctionCallbackInfo<v8::Value>& args) {

//        HandleScope scope;

        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->getNumAttrs());
        hsize_t index=0;
        std::vector<std::string> holder;
        H5Aiterate(group->id,H5_INDEX_NAME, H5_ITER_INC, &index, [](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
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

    void Methods::ReadAttribute (const v8::FunctionCallbackInfo<Value>& args) {

//        HandleScope scope;

        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected attribute name")));
            args.GetReturnValue().SetUndefined();
            return;

        }

        // store specified attribute name
        v8::String::Utf8Value attribute_name (args[0]->ToString());

        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
        hid_t attr_id = H5Aopen(group->id, (*attribute_name), H5P_DEFAULT);
        hid_t datatype_id=H5Aget_type( attr_id );
        switch(H5Tget_class( datatype_id ))
        {
            case 1:
                double value;
                H5Aread(attr_id, datatype_id, &value );
                args.This()->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), (*attribute_name)), v8::Number::New(v8::Isolate::GetCurrent(), value));
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

    void Methods::GetNumObjs (const v8::FunctionCallbackInfo<v8::Value>& args) {

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
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
        H5G_info_t 		ginfo;                  /* Group information */

        /*herr_t ret_value = */H5Gget_info(group->id, &ginfo);
     //   if(ret_value < 0)
     //      throwException("getNumObjs", "H5Gget_info failed");
        args.GetReturnValue().Set((uint32_t) ginfo.nlinks);
        return;

    }

    void Methods::GetMemberNames (const v8::FunctionCallbackInfo<v8::Value>& args) {

//        HandleScope scope;

        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

        v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
        uint32_t index=0;
        std::vector<std::string> holder;
        H5G_info_t 		ginfo;                  /* Group information */

        /*herr_t ret_value = */H5Gget_info(group->id, &ginfo);
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

    void Methods::GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<v8::Value>& args) {

//        HandleScope scope;

        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());

        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), group->getNumObjs());
        uint32_t index=0;
        std::vector<std::string> holder;
//        //std::cout<<"group_name "<<group->name<std::endl;
//        String::Utf8Value group_name (group->name);
//            //std::cout<<"group_name "<<(*group_name)<<std::endl;
//        group->m_group.iterateElems("Geometries", &index, [&](hid_t group_id, const char * member_name, void *operator_data) -> herr_t {
//            //std::cout<<" "<<(*member_name)<<std::endl;
//            ((std::vector<std::string>*)operator_data)->push_back(std::string(member_name));
//            return 0;
//        }, &holder);
        herr_t err;
            H5G_info_t group_info;
            if ((err = H5Gget_info(group->id, &group_info)) < 0) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), " has no info")));
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
//                //std::cout<<"group link_buff.corder "<<link_buff.corder_valid<<" "<<link_buff.corder<<std::endl;
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

    void Methods::GetChildType (const v8::FunctionCallbackInfo<v8::Value>& args) {

//        HandleScope scope;

        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name")));
            args.GetReturnValue().SetUndefined();
            return;

        }
        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
        // store specified child name
        v8::String::Utf8Value child_name (args[0]->ToString());
        args.GetReturnValue().Set((uint32_t) group->childObjType(*child_name));
        //args.This()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"));
        return;

    }

    void Methods::getDatasetType (const v8::FunctionCallbackInfo<v8::Value>& args) {

//        HandleScope scope;

        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected child object's name")));
            args.GetReturnValue().SetUndefined();
            return;

        }
        // unwrap group
        //Group* group = ObjectWrap::Unwrap<Group>(args.This());
        // store specified child name
        v8::String::Utf8Value child_name (args[0]->ToString());
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args.This()->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "id"))->ToObject());
        hid_t id=idWrap->Value();
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
    //                //std::cout<<(*child_name)<<" H5Dget_type "<<H5Tget_class(type)<<" "<<H5T_COMPOUND<<std::endl;
                            int nmembers = H5Tget_nmembers(type);
    //                        std::cout << H5Tget_nmembers(type) << " pt type=" << H5Tis_variable_str(type) << std::endl;
                            bool variableType=true;
                            for (int memberIndex = 0; memberIndex < nmembers; memberIndex++) {
                                hid_t memberType = H5Tget_member_type(type, memberIndex);
    //                            //std::cout<<" H5Tget_member_type "<<H5Tget_class(memberType)<<" "<<H5T_VLEN<<" "<<H5Tis_variable_str(memberType)<<std::endl;
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
//                            //std::cout<<"HL_TYPE_TEXT "<<*child_name<<std::endl;
                            hlType=HLType::HL_TYPE_TEXT;
                            break;
                        default:
//                            //std::cout<<"default "<<H5Tget_class(type)<<std::endl;
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

    void Methods::getDatasetAttributes (const v8::FunctionCallbackInfo<v8::Value>& args) {

        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected dataset name")));
            args.GetReturnValue().SetUndefined();
            return;

        }
        v8::String::Utf8Value dset_name (args[0]->ToString());
        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
        std::string name(*dset_name);
        H5O_info_t object_info;
        herr_t err=H5Oget_info_by_name(group->id, *dset_name, &object_info, H5P_DEFAULT );
        if(err<0){
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to get attr info")));
            args.GetReturnValue().SetUndefined();
            return;

        }
        v8::Local<v8::Object> attrs=v8::Object::New(v8::Isolate::GetCurrent());
        for(unsigned int index=0;index<object_info.num_attrs;index++){
            hid_t  attr_id=H5Aopen_by_idx(group->id, *dset_name, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, index, H5P_DEFAULT,  H5P_DEFAULT );
            if(attr_id<0){
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to open attr")));
                args.GetReturnValue().SetUndefined();
                return;
            }
            H5A_info_t ainfo;
            /*herr_t err=*/H5Aget_info(attr_id, &ainfo );
            size_t nameSize=H5Aget_name(attr_id, 0, NULL);
            std::string attrName(nameSize+1,'\0');
            H5Aget_name(attr_id, nameSize+1, (char*)attrName.c_str());
            hid_t attr_type=H5Aget_type(attr_id);
            hid_t space_id = H5Aget_space(attr_id);
            hssize_t num_elements = H5Sget_simple_extent_npoints(space_id);
            ////std::cout<<"num_elements "<<num_elements<<" "<<H5Tget_size(attr_type)<<" "<<H5Aget_storage_size(attr_id)<<std::endl;
            switch(H5Tget_class(attr_type))
            {
                case H5T_BITFIELD:
                    //std::cout<<" H5T_BITFIELD "<<std::endl;
                    break;
                case H5T_OPAQUE:
                    //std::cout<<" H5T_OPAQUE "<<std::endl;
                    break;
                case H5T_REFERENCE:
                    //std::cout<<" H5T_REFERENCE "<<std::endl;
                    break;
                case H5T_ARRAY:
                    //std::cout<<" H5T_ARRAY "<<std::endl;
                    break;
                case H5T_ENUM:
                    //std::cout<<" H5T_ENUM "<<std::endl;
                    break;
                case H5T_COMPOUND:
                {
                    //std::cout<<" H5T_COMPOUND "<<H5Tget_nmembers(attr_type)<<std::endl;
                    v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), H5Tget_nmembers(attr_type));
                    std::unique_ptr<char[]> buf(new char[H5Aget_storage_size(attr_id)]);
                    H5Aread(attr_id, attr_type, buf.get());
                    for(int mIndex=0;mIndex<H5Tget_nmembers(attr_type);mIndex++){
                        hid_t mType=H5Tget_member_type(attr_type, mIndex);
                        //std::cout<<attrName<<" mType "<<mType<<" "<<H5Tget_member_class(attr_type, mIndex)<<std::endl;
                        switch(H5Tget_member_class(attr_type, mIndex))
                        {
                            case H5T_BITFIELD:
                                //std::cout<<" H5T_BITFIELD "<<std::endl;
                                break;
                            case H5T_OPAQUE:
                                //std::cout<<" H5T_OPAQUE "<<std::endl;
                                break;
                            case H5T_REFERENCE:
                            {
                                //std::cout<<" H5T_REFERENCE "<<std::endl;
                                H5O_type_t obj_type;
                                /*herr_t err=*/H5Rget_obj_type(attr_id, H5R_OBJECT, buf.get(), &obj_type );
                                ssize_t size=H5Rget_name(attr_id, H5R_OBJECT, buf.get(), NULL, 0);
                                std::string refName(size, '\0');
                                size=H5Rget_name(attr_id, H5R_OBJECT, buf.get(), (char *)refName.c_str(), size+1);
                                std::string ref="->"+refName;
                                array->Set(mIndex, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::String::kNormalString, ref.length()));
                            }
                                break;
                            case H5T_ARRAY:
                                //std::cout<<"array inner "<<std::endl;
                                break;
                            case H5T_ENUM:
                                //std::cout<<"enum inner "<<std::endl;
                                break;
                            case H5T_COMPOUND:
                                //std::cout<<"compound inner "<<std::endl;
                                break;
                            case H5T_INTEGER:
                                //std::cout<<"integer inner "<<((long long*)(buf.get()+ H5Tget_member_offset(attr_type, mIndex)))[0]<<std::endl;
                                array->Set(mIndex, v8::Int32::New(v8::Isolate::GetCurrent(), ((long long*)(buf.get()+ H5Tget_member_offset(attr_type, mIndex)))[0]));
                                break;
                            case H5T_FLOAT:
                                //std::cout<<"float inner "<<std::endl;
                                //double value;
                                array->Set(mIndex, v8::Int32::New(v8::Isolate::GetCurrent(), ((double*)(buf.get()+ H5Tget_member_offset(attr_type, mIndex)))[0]));
                                break;
                            case H5T_VLEN:
                                //std::cout<<"H5T_VLEN inner "<<std::endl;
                                break;
                            case H5T_STRING:
                            {
                                //std::cout<<"compound H5T_STRING "<<std::endl;
                                array->Set(mIndex, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(buf.get()+ H5Tget_member_offset(attr_type, mIndex))));
                            }
                                break;
                            case H5T_NO_CLASS:
                            default:
                //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                //                    args.GetReturnValue().SetUndefined();
            //                        return;
                                break;
                        }

                        H5Tclose(mType);
                    }
                    attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), array);
                }
                    break;
                case H5T_INTEGER:
                {
                    std::unique_ptr<long long> buf(new long long[num_elements]);
                    H5Aread(attr_id, attr_type, buf.get());
                    if(num_elements>1){
                        v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
                        for(unsigned int elementIndex=0;elementIndex<num_elements;elementIndex++){
                            array->Set(elementIndex, v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[elementIndex]));
                        }
                        attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), array);
                    }
                    else attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::Int32::New(v8::Isolate::GetCurrent(), buf.get()[0]));
                }
                    break;
                case H5T_FLOAT:
                    double value;
                    H5Aread(attr_id, attr_type, &value);
                    attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::Number::New(v8::Isolate::GetCurrent(), value));
                    break;
                case H5T_VLEN:
                {
                    hid_t super_type=H5Tget_super(attr_type);
                    //std::cout<<" H5T_VLEN "<<attrName.c_str()<<" "<<H5Tget_class(super_type)<<std::endl;
                    if(H5Tget_class(super_type)==H5T_REFERENCE){
                        H5O_type_t obj_type;
                        std::unique_ptr<hvl_t> vl(new hvl_t[num_elements]);
                        H5Aread(attr_id, attr_type, (void*)vl.get());
                        if(num_elements>0){
                            v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
                            for(unsigned int elementIndex=0;elementIndex<num_elements;elementIndex++){
                                /*herr_t err=*/H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, &obj_type );
                                ssize_t size=H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, NULL, 0);
                                std::string refName(size, '\0');
                                size=H5Rget_name(attr_id, H5R_OBJECT, vl.get()[elementIndex].p, (char *)refName.c_str(), size+1);
                                std::string ref="->"+refName;
                                array->Set(elementIndex, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::String::kNormalString, ref.length()));
                            }
                            attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), array);
                        }
                        else {
                            /*herr_t err=*/H5Rget_obj_type(attr_id, H5R_OBJECT, vl.get()[0].p, &obj_type );
                            //std::cout<<"obj_type "<<obj_type<<std::endl;
                            ssize_t size=H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, NULL, 0);
                            std::string refName(size, '\0');
                            size=H5Rget_name(attr_id, H5R_OBJECT, vl.get()[0].p, (char *)refName.c_str(), size+1);
                            std::string ref="->"+refName;
                            attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(ref.c_str()), v8::String::kNormalString, ref.length()));
                        }
                    }
                    else{
                        std::unique_ptr<hvl_t> vl(new hvl_t[num_elements]);
                        H5Aread(attr_id, attr_type, (void*)vl.get());
                        attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(vl.get()[0].p), v8::String::kNormalString, vl.get()[0].len));
                    }
                    if(super_type>=0)H5Tclose(super_type);
                }
                    break;
                case H5T_STRING:
                {
                    if(H5Tis_variable_str(attr_type)>=0){
                        //std::cout<<"var H5T_STRING "<<attrName.c_str()<<" "<<H5Aget_storage_size(attr_id)<<" "<<H5Tis_variable_str(attr_type)<<std::endl;
                        if(num_elements>1){
                            std::unique_ptr<char*> buf(new char*[num_elements]);
                            /*herr_t err=*/H5Aread(attr_id, attr_type, buf.get());
                            v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), num_elements);
                            for(unsigned int elementIndex=0;elementIndex<num_elements;elementIndex++){
                            std::string attrValue="";
                            if(buf.get()[elementIndex]!=NULL)attrValue=buf.get()[elementIndex];
                            array->Set(elementIndex, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::String::kNormalString, attrValue.length()));

                            }
                            attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), array);

                        }
                        else{
                            std::unique_ptr<char> buf(new char[H5Aget_storage_size(attr_id)]);
                            /*herr_t err=*/H5Aread(attr_id, attr_type, buf.get());
                            std::string attrValue="";
                            if(buf.get()!=NULL)attrValue=buf.get();
                            attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (char*)(attrValue.c_str()), v8::String::kNormalString, attrValue.length()));
                        }
                    }
                    else{
                        //std::cout<<" H5T_STRING "<<attrName.c_str()<<" "<<H5Aget_storage_size(attr_id)<<std::endl;
                        std::string strValue(H5Aget_storage_size(attr_id)+1,'\0');
                        H5Aread(attr_id, attr_type, (void*)strValue.c_str());
                        attrs->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), attrName.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str(), v8::String::kNormalString, std::strlen(strValue.c_str())));
                    }
                }
                    break;
                case H5T_NO_CLASS:
                default:
    //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
    //                    args.GetReturnValue().SetUndefined();
//                        return;
                    break;
            }
            H5Sclose(space_id);
            H5Tclose(attr_type);
            H5Aclose(attr_id);
        }
        args.GetReturnValue().Set(attrs);
    }

    void Methods::getFilters (const v8::FunctionCallbackInfo<v8::Value>& args) {

        // fail out if arguments are not correct
        if (args.Length() != 1 || !args[0]->IsString()) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected dataset name")));
            args.GetReturnValue().SetUndefined();
            return;

        }
        v8::String::Utf8Value dset_name (args[0]->ToString());
        // unwrap group
        Methods* group = ObjectWrap::Unwrap<Methods>(args.This());
        std::string name(*dset_name);
            v8::Local<v8::Object>&& filters = Filters::Instantiate(group->id, name);
            args.GetReturnValue().Set(filters);

    }

    
    //protectd methods
    
    int Methods::getNumAttrs()
    {
       H5O_info_t oinfo;    /* Object info */

       H5Oget_info(id, &oinfo);
       return( (int)oinfo.num_attrs );
    }


    hsize_t Methods::getNumObjs()
    {
       H5G_info_t 		ginfo;                  /* Group information */

       herr_t ret_value = H5Gget_info(id, &ginfo);
       if(ret_value < 0){
//          throwException("getNumObjs", "H5Gget_info failed");
       }
       return (ginfo.nlinks);
    }

    H5O_type_t Methods::childObjType(const char* objname)
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

    std::string Methods::getObjnameByIdx(hsize_t idx)
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

    
}