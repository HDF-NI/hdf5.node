
#include "hdf5.h"
#include "hdf5_hl.h"

#include "attributes.hpp"

namespace NodeHDF5{

    void Attributes::make_attribute_from_typed_array(const hid_t &group_id, const char *attribute_name, v8::Handle<v8::TypedArray> buffer, hid_t type_id) {
        std::unique_ptr<hsize_t[]> currentDims(new hsize_t[1]);
        currentDims.get()[0]=buffer->Length();
        hid_t attr_space=H5Screate_simple( 1, currentDims.get(), NULL );
        if(attr_space>=0){
            hid_t attr_type=H5Tcopy(type_id);
            hid_t attr_id=H5Acreate2(group_id, attribute_name, attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
            
            H5Awrite(attr_id, attr_type, buffer->Buffer()->Externalize().Data());
            H5Aclose(attr_id);
            H5Tclose(attr_type);
            H5Sclose(attr_space);
        }
    }
    
    void Attributes::make_attribute_from_array(const hid_t &group_id, const char *attribute_name, v8::Handle<v8::Array> array) {
        //hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
        int rank=1;
        std::unique_ptr<hsize_t[]> countSpace(new hsize_t[rank]);
        countSpace.get()[0]=1;
        std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
        count.get()[0]=array->Length();
        hid_t memspace_id = H5Screate_simple (rank, countSpace.get(), NULL);
        hid_t type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(type_id, H5T_VARIABLE);
        hid_t arraytype_id =H5Tarray_create( type_id,  rank, count.get() );
        hid_t attr_id=H5Acreate2(group_id, attribute_name, arraytype_id, memspace_id, H5P_DEFAULT, H5P_DEFAULT);
        std::unique_ptr<char*[]> vl(new char*[array->Length()]);
        for(unsigned int arrayIndex=0;arrayIndex<array->Length();arrayIndex++){
            v8::String::Utf8Value buffer (array->Get(arrayIndex)->ToString());
            std::string s(*buffer);
            ////std::cout<<s<<std::endl;
            vl.get()[arrayIndex]=new char[s.length()+1];
            std::strncpy(vl.get()[arrayIndex], s.c_str(), s.length()+1);
    
        }
            //if(arrayIndex==0){
        //std::cout<<"write the data "<<(vl.get())<<std::endl;
        herr_t err=H5Awrite(attr_id, arraytype_id, vl.get());
        //std::cout<<"wrote the data "<<err<<std::endl;
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len array attribute")));
            //return;
        }
            //}
        H5Tclose(arraytype_id);
        H5Tclose(type_id);
        H5Aclose(attr_id);
        H5Sclose(memspace_id);
        //H5Pclose(dcpl);
    }

    void Attributes::Refresh (const v8::FunctionCallbackInfo<v8::Value>& args) {

        // fail out if arguments are not correct
        if (args.Length() >0 ) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;

        }

        // unwrap group
        Attributes* group = ObjectWrap::Unwrap<Attributes>(args.This());
        hsize_t index=0;
        std::vector<std::string> holder;
        H5Aiterate(group->id,H5_INDEX_NAME, H5_ITER_INC, &index, [](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
            return 0;
        }, &holder);
//        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//        }, &index, &holder);
        for(index=0;index<(uint32_t)group->getNumAttrs();index++)
        {
        hid_t attr_id = H5Aopen(group->id, holder[index].c_str(), H5P_DEFAULT);
        hid_t attr_type=H5Aget_type(attr_id);
        hid_t space = H5Aget_space (attr_id);
        switch(H5Sget_simple_extent_type(space)){
            case H5S_SIMPLE:
            {
                hssize_t numberOfElements=H5Sget_simple_extent_npoints(space);
                H5T_class_t class_id=H5Tget_class(attr_type);
                switch(class_id)
                {
                    case H5T_ARRAY:
                    {
                        hid_t basetype_id=H5Tget_super(attr_type);
                        if(H5Tis_variable_str(basetype_id)){
                            int arrayRank=H5Tget_array_ndims(attr_type);
                            std::unique_ptr<hsize_t[]> arrayDims(new hsize_t[arrayRank]);
                            /*int arrayLength=*/H5Tget_array_dims(attr_type, arrayDims.get());
                            std::unique_ptr<char*[]> vl(new char*[arrayDims.get()[0]]);
                            herr_t err = H5Aread(attr_id, attr_type, vl.get());
                            if(err<0)
                            {
                                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset")));
                                args.GetReturnValue().SetUndefined();
                                return;
                            }
                            v8::Local<v8::Array> array=v8::Array::New(v8::Isolate::GetCurrent(), arrayDims.get()[0]);
                            for(unsigned int arrayIndex=0;arrayIndex<arrayDims.get()[0];arrayIndex++){
                                std::string s(vl.get()[arrayIndex]);
                                //std::cout<<arrayIndex<<" "<<(s)<<" "<<std::strlen(vl.get()[arrayIndex])<<std::endl;
                                array->Set(arrayIndex, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), vl.get()[arrayIndex],v8::String::kNormalString, std::strlen(vl.get()[arrayIndex])));
                            }
                            args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), array);
        //                    args.GetReturnValue().Set(array);
                        }
                    }
                    break;
                    case H5T_STRING:
                    break;
                    default:
                        size_t size=H5Tget_size( attr_type );
                    v8::Local<v8::ArrayBuffer> arrayBuffer;
                    v8::Local<v8::TypedArray> buffer;
                    if(class_id==H5T_FLOAT && size==8) {
                        arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 8*numberOfElements);
                        buffer = v8::Float64Array::New(arrayBuffer, 0, numberOfElements);
                    }
                    else if(class_id==H5T_FLOAT && size==4) {
                        arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*numberOfElements);
                        buffer = v8::Float32Array::New(arrayBuffer, 0, numberOfElements);
                    }
                    else if(class_id==H5T_INTEGER && size==4) {
                        if(H5Tget_sign(attr_type)==H5T_SGN_2)
                        {
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*numberOfElements);
                            buffer = v8::Int32Array::New(arrayBuffer, 0, numberOfElements);
                        }
                        else{
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*numberOfElements);
                            buffer = v8::Uint32Array::New(arrayBuffer, 0, numberOfElements);
                        }
                    }
                    else if(class_id==H5T_FLOAT && size==2) {
                        if(H5Tget_sign(attr_type)==H5T_SGN_2)
                        {
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 2*numberOfElements);
                            buffer = v8::Int16Array::New(arrayBuffer, 0, numberOfElements);
                        }
                        else{
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), 2*numberOfElements);
                            buffer = v8::Uint16Array::New(arrayBuffer, 0, numberOfElements);
                        }
                    }
                    else if(class_id==H5T_FLOAT && size==1) {
                        if(H5Tget_sign(attr_type)==H5T_SGN_2)
                        {
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), numberOfElements);
                            buffer = v8::Int8Array::New(arrayBuffer, 0, numberOfElements);
                        }
                        else{
                            arrayBuffer=v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), numberOfElements);
                            buffer = v8::Uint8Array::New(arrayBuffer, 0, numberOfElements);
                        }
                    }
                    else
                    {
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                        args.GetReturnValue().SetUndefined();
                        H5Sclose (space);
                        H5Tclose(attr_type);
                        H5Aclose(attr_id);
                        return;
                    }
                        H5Aread(attr_id, attr_type, buffer->Buffer()->Externalize().Data());
                        args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), buffer);
                        break;
                }
                
            }
                break;
            case H5S_SCALAR:
                switch(H5Tget_class(attr_type))
                {
                    case H5T_INTEGER:
                        long long intValue;
                        H5Aread(attr_id, attr_type, &intValue);
                        args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::Int32::New(v8::Isolate::GetCurrent(), intValue));
                        break;
                    case H5T_FLOAT:
                    {
                        size_t size=H5Tget_size( attr_type );
                        switch(size){
                            case 8:
                            {
                                double value;
                                H5Aread(attr_id, attr_type, &value);
                                args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::Number::New(v8::Isolate::GetCurrent(), value));
                            }
                                break;
                            default:
                            {
                                float value;
                                H5Aread(attr_id, attr_type, &value);
                                args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::Number::New(v8::Isolate::GetCurrent(), value));
                            }
                                break;
                        }
                    }
                        break;
                    case H5T_STRING:
                    {
                        htri_t isVlen = H5Tis_variable_str( attr_type );
    
                        if (isVlen == -1) {
                            //std::cout << "The H5Tis_variable_str function did not return successfully" << std::endl;
                        } else if (isVlen == 0) {
                            /*
                             * Do whatever was done before I came along.
                             */
                            hsize_t storeSize = H5Aget_storage_size(attr_id);
                            std::string strValue(storeSize,'\0');
                            H5Aread(attr_id, attr_type, (void*)strValue.c_str());
                            args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
                        } else {
                            //std::cout << "Variable-length string attribute encountered" << std::endl;
    
                            H5A_info_t ainfo;
                            H5Aget_info(attr_id, &ainfo );
                            std::unique_ptr<char*[]> buffer(new char*[2]);
    
                            /*
                             * Create the memory datatype.
                             */
                            hid_t memtype = H5Tcopy (H5T_C_S1);
                            herr_t status = H5Tset_size (memtype, H5T_VARIABLE);
                            if(status<0)
                            {
                                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to set size variable")));
                                args.GetReturnValue().SetUndefined();
                                return;
        
                            }
    
                            hid_t type = H5Tget_native_type(attr_type, H5T_DIR_ASCEND);
                            /*
                             * Read the data.
                             */
                            status = H5Aread (attr_id, type, buffer.get());
                            v8::Local<v8::String> varLenStr=v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.get()[0]);
                            v8::Local<v8::Value> varLenStrObject=v8::StringObject::New(varLenStr);
                            varLenStrObject->ToObject()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "variable-length") );
                            args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), varLenStrObject);
    
                            /*
                             * Clean up the mess I made
                             */
                            //status = H5Dvlen_reclaim (memtype, space, H5P_DEFAULT, rdata);
                            status = H5Tclose (memtype);
                        }
                    }
                        break;
                    case H5T_NO_CLASS:
                    default:
                        //throw std::invalid_argument("unsupported data type");
        //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        //                    args.GetReturnValue().SetUndefined();
                            return;
                        break;
                }
                break;
            case H5T_NO_CLASS:
            default:
                break;
        }
        H5Sclose (space);
        H5Tclose(attr_type);
        H5Aclose(attr_id);
        }

        return;

    };

    void Attributes::Flush (const v8::FunctionCallbackInfo<v8::Value>& args) {

    // fail out if arguments are not correct
        if (args.Length() >0 ) {

            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
            args.GetReturnValue().SetUndefined();
            return;

        }

        // unwrap group
        Attributes* group = Unwrap<Attributes>(args.This());

        v8::Local<v8::Array> propertyNames=args.This()->GetPropertyNames();
        for(unsigned int index=0;index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!args.This()->Get(name)->IsFunction() && strncmp("id",(*v8::String::Utf8Value(name->ToString())), 2)!=0)
             {
//                //std::cout<<index<<" "<<name->IsString()<<std::endl;
//                //std::cout<<index<<" "<<(*v8::String::Utf8Value(name->ToString()))<<std::endl;
                htri_t attrExists=H5Aexists(group->id, *v8::String::Utf8Value(name->ToString()));
                if(args.This()->Get(name)->IsFloat64Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_DOUBLE);
                }
                else if(args.This()->Get(name)->IsFloat32Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_FLOAT);
                }
                else if(args.This()->Get(name)->IsInt32Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_INT);
                }
                else if(args.This()->Get(name)->IsUint32Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_UINT);
                }
                else if(args.This()->Get(name)->IsInt16Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_SHORT);
                }
                else if(args.This()->Get(name)->IsUint16Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_USHORT);
                }
                else if(args.This()->Get(name)->IsInt8Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_INT8);
                }
                else if(args.This()->Get(name)->IsUint8Array())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_typed_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Float64Array>::Cast(args.This()->Get(name)), H5T_NATIVE_UINT8);
                }
                else if(args.This()->Get(name)->IsUint32())
                {
                    uint32_t value=args.This()->Get(name)->ToUint32()->Uint32Value();
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_UINT);
                    hid_t attr_space=H5Screate( H5S_SCALAR );
                    hid_t attr_id=H5Acreate2(group->id, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
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
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_INT);
                    hid_t attr_space=H5Screate( H5S_SCALAR );
                    hid_t attr_id=H5Acreate2(group->id, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
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
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_NATIVE_DOUBLE);
                    hid_t attr_space=H5Screate( H5S_SCALAR );
                    hid_t attr_id=H5Acreate2(group->id, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;

                    }
                    H5Awrite(attr_id, attr_type, &value);
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);

                }
                else if(args.This()->Get(name)->IsStringObject())
                {
                    std::string value((*v8::String::Utf8Value(v8::StringObject::Cast(*args.This()->Get(name))->ValueOf())));
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_C_S1);
                    H5Tset_size(attr_type, H5T_VARIABLE);
                    hid_t attr_space=H5Screate( H5S_SCALAR );
                    hid_t attr_id=H5Acreate2(group->id, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;

                    }
                    std::unique_ptr<char*[]> vl(new char*[1]);
                    vl.get()[0]=new char[value.length()+1];
                    std::strncpy(vl.get()[0], value.c_str(), value.length()+1);
                    H5Awrite(attr_id, attr_type, vl.get());
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);

                }
                else if(args.This()->Get(name)->IsString())
                {
                    std::string value((*v8::String::Utf8Value(args.This()->Get(name)->ToString())));
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    hid_t attr_type=H5Tcopy(H5T_C_S1);
                    size_t s = std::strlen(value.c_str());
                    if (s) {
                        H5Tset_size(attr_type, s);
                    }
                    hid_t attr_space=H5Screate( H5S_SCALAR );
                    hid_t attr_id=H5Acreate2(group->id, *v8::String::Utf8Value(name->ToString()), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                    if(attr_id<0)
                    {
                        H5Sclose(attr_space);
                        H5Tclose(attr_type);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed creating attribute")));
                        args.GetReturnValue().SetUndefined();
                        return;

                    }
                    H5Awrite(attr_id, attr_type, value.c_str());
                    H5Sclose(attr_space);
                    H5Tclose(attr_type);
                    H5Aclose(attr_id);

                }
                else if(args.This()->Get(name)->IsArray())
                {
                    if(attrExists)
                    {
                        H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                    }
                    make_attribute_from_array(group->id, *v8::String::Utf8Value(name->ToString()), v8::Local<v8::Array>::Cast(args.This()->Get(name)));
                }
             }
        }


        return;

    };
}