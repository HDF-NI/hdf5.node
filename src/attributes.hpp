#pragma once

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <cstring>
#include <vector>
#include <memory>


namespace NodeHDF5{
    class Attributes : public node::ObjectWrap {
    protected:
            std::string name;
            hid_t id;
            hid_t gcpl_id;
    public:
        Attributes() {};
        Attributes(hid_t id) : id (id) {};
        Attributes(const Attributes& orig) = delete;
        virtual ~Attributes(){};
        static void Refresh (const v8::FunctionCallbackInfo<v8::Value>& args) {

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
            H5Aiterate(group->id,H5_INDEX_NAME, H5_ITER_INC, &index, [&](hid_t loc, const char* attr_name, const H5A_info_t* ainfo, void *operator_data) -> herr_t {
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
            switch(H5Tget_class(attr_type))
            {
                case H5T_INTEGER:
                    long long intValue;
                    H5Aread(attr_id, attr_type, &intValue);
                    args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::Int32::New(v8::Isolate::GetCurrent(), intValue));
                    break;
                case H5T_FLOAT:
                    double value;
                    H5Aread(attr_id, attr_type, &value);
                    args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::Number::New(v8::Isolate::GetCurrent(), value));
                    break;
                case H5T_STRING:
                {
                    std::string strValue(H5Aget_storage_size(attr_id),'\0');
                    H5Aread(attr_id, attr_type, (void*)strValue.c_str());
                    args.This()->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
                }
                    break;
                case H5T_NO_CLASS:
                default:
    //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
    //                    args.GetReturnValue().SetUndefined();
                        return;
                    break;
            }
            H5Tclose(attr_type);
            H5Aclose(attr_id);
            }

            return;

        };
    
        static void Flush (const v8::FunctionCallbackInfo<v8::Value>& args) {
        
        // fail out if arguments are not correct
            if (args.Length() >0 ) {

                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected arguments")));
                args.GetReturnValue().SetUndefined();
                return;

            }

            // unwrap group
            Attributes* group = ObjectWrap::Unwrap<Attributes>(args.This());

            v8::Local<v8::Array> propertyNames=args.This()->GetPropertyNames();
            for(unsigned int index=0;index<propertyNames->Length();index++)
            {
                 v8::Local<v8::Value> name=propertyNames->Get (index);
                 if(!args.This()->Get(name)->IsFunction() && strncmp("id",(*v8::String::Utf8Value(name->ToString())), 2)!=0)
                 {
    //                std::cout<<index<<" "<<name->IsString()<<std::endl;
    //                std::cout<<index<<" "<<(*v8::String::Utf8Value(name->ToString()))<<std::endl;
                    htri_t attrExists=H5Aexists(group->id, *v8::String::Utf8Value(name->ToString()));
                    if(args.This()->Get(name)->IsUint32())
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
                    else if(args.This()->Get(name)->IsString())
                    {
                        std::string value((*v8::String::Utf8Value(args.This()->Get(name)->ToString())));
                        if(attrExists)
                        {
                            H5Adelete(group->id, *v8::String::Utf8Value(name->ToString()));
                        }
                        hid_t attr_type=H5Tcopy(H5T_C_S1);
                        H5Tset_size(attr_type, std::strlen(value.c_str()));
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
                 }
            }


            return;

        };
        
    protected:
        virtual int getNumAttrs() = 0;
    };
}
