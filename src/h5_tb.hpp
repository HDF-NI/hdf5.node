#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <functional>

#include "hdf5.h"
#include "H5TBpublic.h"
#include "H5Dpublic.h"
#include "H5Tpublic.h"

namespace NodeHDF5 {

    class H5tb {
    public:
        static void Initialize (Handle<Object> target) {

            // append this function to the target object
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::make_table)->GetFunction());
            target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readTable"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5tb::read_table)->GetFunction());
        }

        static void make_table (const v8::FunctionCallbackInfo<Value>& args)
        {
            String::Utf8Value table_name (args[1]->ToString());
            Local<v8::Array> table=Local<v8::Array>::Cast(args[2]);
            char* field_names[table->Length()];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < table->Length(); i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
                std::memset ( field_names[i], 0, 255 );
            }
            std::unique_ptr<size_t[]> field_offsets(new size_t[table->Length()]);
            std::unique_ptr<hid_t[]> field_types(new hid_t[table->Length()]);
            hsize_t nrecords=0;
            size_t type_size=0;
            for (uint32_t i = 0; i < table->Length(); i++)
            {
            std::cout<<"table->Length() "<<table->Length()<<" "<<std::endl;
                String::Utf8Value field_name (table->Get(i)->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(),"name"))->ToString());
                std::string fieldName((*field_name)); 
                std::memcpy(field_names[i], fieldName.c_str(), fieldName.length());
            std::cout<<"table->Length() "<<field_names[i]<<" "<<std::endl;
                if(table->Get(i)->IsFloat64Array())
                {
                    Local<v8::Float64Array> field=Local<v8::Float64Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    type_size+=8;
                    field_types[i]=H5T_NATIVE_DOUBLE;
                }
                else if(table->Get(i)->IsUint32Array())
                {
                    Local<v8::Uint32Array> field=Local<v8::Uint32Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    type_size+=4;
                    field_types[i]=H5T_NATIVE_UINT;
                }
                else if(table->Get(i)->IsInt32Array())
                {
                    Local<v8::Int32Array> field=Local<v8::Int32Array>::Cast(table->Get(i));
                    nrecords=field->Length();
                    field_offsets[i]=type_size;
                    type_size+=4;
                    field_types[i]=H5T_NATIVE_INT;
                }
                else
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
            }
            std::cout<<"type_size "<<type_size<<" "<<nrecords<<std::endl;
            std::unique_ptr<char[]> data(new char[type_size*nrecords]);
            for (uint32_t i = 0; i < table->Length(); i++)
            {
                if(table->Get(i)->IsFloat64Array())
                {
                    Local<v8::Float64Array> field=Local<v8::Float64Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((double*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToNumber()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsUint32Array())
                {
                    Local<v8::Uint32Array> field=Local<v8::Uint32Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToUint32()->Value ();
                    }
                    
                }
                else if(table->Get(i)->IsInt32Array())
                {
                    Local<v8::Int32Array> field=Local<v8::Int32Array>::Cast(table->Get(i));
                    for(uint32_t j=0;j<nrecords;j++){
                        ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]=field->Get(j)->ToInt32()->Value ();
                    }
                    
                }
                
            }
            hsize_t chunk_size = 10;
            int *fill_data = NULL;
            herr_t err=H5TBmake_table( (*table_name), args[0]->ToInt32()->Value(), (*table_name), table->Length(), nrecords, type_size, (const char**)field_names, field_offsets.get(), field_types.get(), chunk_size, fill_data, 0, (const void*)data.get());
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed making table , " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
//            /* release */
            for (unsigned int i = 0; i < table->Length(); i++)
            {
                free ( field_names[i] );
            }
        }
        
        static void read_table (const v8::FunctionCallbackInfo<Value>& args)
        {
            String::Utf8Value table_name (args[1]->ToString());
            hsize_t nfields;
            hsize_t nrecords;
            herr_t err = H5TBget_table_info(args[0]->ToInt32()->Value(), (*table_name), &nfields, &nrecords);
            if (err < 0) {
                std::string tableName(*table_name);
                std::string errStr="Failed getting table info, " + tableName + " with return: " + std::to_string(err) + " " + std::to_string(args[0]->ToInt32()->Value()) + ".\n";
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), errStr.c_str())));
                args.GetReturnValue().SetUndefined();
                return;
            }
            char* field_names[nfields];
//            char** field_names= (char**) HDmalloc( sizeof(char*) * (size_t)nfields );
            for (unsigned int i = 0; i < nfields; i++)
            {
                field_names[i] = (char*) malloc( sizeof(char) * 255 );
            }
            std::unique_ptr<size_t[]> field_size(new size_t[nfields]);
            std::unique_ptr<size_t[]> field_offsets(new size_t[nfields]);
            size_t type_size;
            std::cout<<"H5TBget_field_info "<<nfields<<" "<<std::endl;
            err=H5TBget_field_info(args[0]->ToInt32()->Value(), (*table_name), field_names, field_size.get(), field_offsets.get(), &type_size );
            for (unsigned int i = 0; i < nfields; i++)
            {
                std::cout<<" "<<field_names[i]<<" "<<field_size[i]<<" "<<field_offsets[i]<<std::endl;

            }

            std::unique_ptr<char[]> data(new char[type_size*nrecords]);
            err=H5TBread_records (args[0]->ToInt32()->Value(), (*table_name), 0, nrecords, type_size,  field_offsets.get(), field_size.get(),  (void*) data.get() );
                hid_t dataset = H5Dopen(args[0]->ToInt32()->Value(), (*table_name), H5P_DEFAULT);
                hid_t dataset_type=H5Dget_type(dataset );
                v8::Local< v8::Array > 	table=v8::Array::New (v8::Isolate::GetCurrent(), nfields);
                for (uint32_t i = 0; i < nfields; i++)
                {
                    hid_t type=H5Tget_member_type(dataset_type, i);
//                    std::cout<<" "<<field_names[i]<<" "<<type<<std::endl;
                    switch(H5Tget_class(type)){
                        case H5T_FLOAT:
                            if(H5Tget_precision(type)==64)
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 8*nrecords);
                                Local<Float64Array> buffer = Float64Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((double*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            else if(H5Tget_precision(type)==32)
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                Local<Float32Array> buffer = Float32Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((double*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            break;
                        case H5T_INTEGER:
                            if(H5Tget_sign(type)==H5T_SGN_NONE)
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                Local<Uint32Array> buffer = Uint32Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((unsigned int*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            else
                            {
                                Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), 4*nrecords);
                                Local<Int32Array> buffer = Int32Array::New(arrayBuffer, 0, nrecords);
                                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "name"), String::NewFromUtf8(v8::Isolate::GetCurrent(), field_names[i]));
                                for(uint32_t j=0;j<nrecords;j++){
//                                    std::cout<<" "<<j<<" "<<(((double*)&data[j*type_size+field_offsets[i]])[0])<<" "<<field_offsets[i]<<std::endl;
                                    
                                    buffer->Set(j, v8::Number::New(v8::Isolate::GetCurrent(), ((int*)&data[j*type_size+field_offsets[i]])[0]));
                                }

                                table->Set(i, buffer);
                            }
                            break;
                        default:
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                            args.GetReturnValue().SetUndefined();
                            return;
                            break;
                    }
                    H5Tclose(type);
                }
                H5Tclose(dataset_type);
                H5Dclose(dataset);
            /* release */
            for (unsigned int i = 0; i < nfields; i++)
            {
                free ( field_names[i] );
            }
                args.GetReturnValue().Set(table);
            
        }
        
    };
}