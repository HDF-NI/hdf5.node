#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_buffer.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <functional>

#include "file.h"
#include "group.h"
#include "H5LTpublic.h"

static herr_t
H5LT_make_dataset_numerical( hid_t loc_id,
                            const char *dset_name,
                            int rank,
                            const hsize_t *dims,
                            hid_t tid,
                            hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id,
                            const void *data )
{
    hid_t   did = -1, sid = -1;

    /* check the arguments */
    if (dset_name == NULL) 
      return -1;

    /* Create the data space for the dataset. */
    if((sid = H5Screate_simple(rank, dims, NULL)) < 0)
        return -1;

    /* Create the dataset. */
    if((did = H5Dcreate2(loc_id, dset_name, tid, sid, lcpl_id, dcpl_id, dapl_id)) < 0)
        goto out;

    /* Write the dataset only if there is data to write */
    if(data)
        if(H5Dwrite(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
            goto out;

    /* End access to the dataset and release resources used by it. */
    if(H5Dclose(did) < 0)
        return -1;

    /* Terminate access to the data space. */
    if(H5Sclose(sid) < 0)
        return -1;

    return 0;

out:
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Sclose(sid);
    } H5E_END_TRY;
    return -1;
}

namespace NodeHDF5 {

    static std::map<H5T, hid_t> toTypeMap{
                                     {NODE_H5T_STD_I8BE,H5T_STD_I8BE},
                                     {NODE_H5T_STD_I8LE,H5T_STD_I8LE},
                                     {NODE_H5T_STD_I16BE,H5T_STD_I16BE},
                                     {NODE_H5T_STD_I16LE,H5T_STD_I16LE},
                                     {NODE_H5T_STD_I32BE,H5T_STD_I32BE},
                                     {NODE_H5T_STD_I32LE,H5T_STD_I32LE},
                                     {NODE_H5T_STD_I64BE,H5T_STD_I64BE},
                                     {NODE_H5T_STD_I64LE,H5T_STD_I64LE},
                                     {NODE_H5T_STD_U8BE,H5T_STD_U8BE},
                                     {NODE_H5T_STD_U8LE,H5T_STD_U8LE},
                                     {NODE_H5T_STD_U16BE,H5T_STD_U16BE},
                                     {NODE_H5T_STD_U16LE,H5T_STD_U16LE},
                                     {NODE_H5T_STD_U32BE,H5T_STD_U32BE},
                                     {NODE_H5T_STD_U32LE,H5T_STD_U32LE},
                                     {NODE_H5T_STD_U64BE,H5T_STD_U64BE},
                                     {NODE_H5T_STD_U64LE,H5T_STD_U64LE},
                                     {NODE_H5T_NATIVE_CHAR,H5T_NATIVE_CHAR},
                                     {NODE_H5T_NATIVE_SCHAR,H5T_NATIVE_SCHAR},
                                     {NODE_H5T_NATIVE_UCHAR,H5T_NATIVE_UCHAR},
                                     {NODE_H5T_NATIVE_SHORT,H5T_NATIVE_SHORT},
                                     {NODE_H5T_NATIVE_USHORT,H5T_NATIVE_USHORT},
                                     {NODE_H5T_NATIVE_INT,H5T_NATIVE_INT},
                                     {NODE_H5T_NATIVE_UINT,H5T_NATIVE_UINT},
                                     {NODE_H5T_NATIVE_LONG,H5T_NATIVE_LONG},
                                     {NODE_H5T_NATIVE_ULONG,H5T_NATIVE_ULONG},
                                     {NODE_H5T_NATIVE_LLONG,H5T_NATIVE_LLONG},
                                     {NODE_H5T_NATIVE_ULLONG,H5T_NATIVE_ULLONG},
                                     {NODE_H5T_IEEE_F32BE,H5T_IEEE_F32BE},
                                     {NODE_H5T_IEEE_F32LE,H5T_IEEE_F32LE},
                                     {NODE_H5T_IEEE_F64BE,H5T_IEEE_F64BE},
                                     {NODE_H5T_IEEE_F64LE,H5T_IEEE_F64LE},
                                     {NODE_H5T_NATIVE_FLOAT,H5T_NATIVE_FLOAT},
                                     {NODE_H5T_NATIVE_DOUBLE,H5T_NATIVE_DOUBLE},
                                     {NODE_H5T_NATIVE_LDOUBLE,H5T_NATIVE_LDOUBLE},
                                     {NODE_H5T_STRING,H5T_STRING},
                                     //{NODE_STRSIZE,STRSIZE},
                                     //{NODE_STRPAD,STRPAD},
                                     //{NODE_CSET,CSET},
                                     //{NODE_CTYPE,CTYPE},
                                     {NODE_H5T_VARIABLE,H5T_VARIABLE},
                                     {NODE_H5T_STR_NULLTERM,H5T_STR_NULLTERM},
                                     {NODE_H5T_STR_NULLPAD,H5T_STR_NULLPAD},
                                     {NODE_H5T_STR_SPACEPAD,H5T_STR_SPACEPAD},
                                     {NODE_H5T_CSET_ASCII,H5T_CSET_ASCII},
                                     {NODE_H5T_CSET_UTF8,H5T_CSET_UTF8},
                                     {NODE_H5T_C_S1,H5T_C_S1},
                                     {NODE_H5T_FORTRAN_S1,H5T_FORTRAN_S1},
                                     {NODE_H5T_OPAQUE,H5T_OPAQUE},
                                     //{NODE_OPQ_SIZE,OPQ_SIZE},
                                     //{NODE_OPQ_TAG,OPQ_TAG},
                                     {NODE_H5T_COMPOUND,H5T_COMPOUND},
                                     {NODE_H5T_ENUM,H5T_ENUM},
                                     {NODE_H5T_ARRAY,H5T_ARRAY},
                                     {NODE_H5T_VLEN,H5T_VLEN},
                                     //{NODE_STRING,STRING},
                                     //{NODE_NUMBER,NUMBER},
                                     {NODE_H5T_NATIVE_INT8,H5T_NATIVE_INT8},
                                     {NODE_H5T_NATIVE_UINT8,H5T_NATIVE_UINT8}};

    static std::map<hid_t, H5T> toEnumMap{
                                     {H5T_STD_I8BE,NODE_H5T_STD_I8BE},
                                     {H5T_STD_I8LE,NODE_H5T_STD_I8LE},
                                     {H5T_STD_I16BE,NODE_H5T_STD_I16BE},
                                     {H5T_STD_I16LE,NODE_H5T_STD_I16LE},
                                     {H5T_STD_I32BE,NODE_H5T_STD_I32BE},
                                     {H5T_STD_I32LE,NODE_H5T_STD_I32LE},
                                     {H5T_STD_I64BE,NODE_H5T_STD_I64BE},
                                     {H5T_STD_I64LE,NODE_H5T_STD_I64LE},
                                     {H5T_STD_U8BE,NODE_H5T_STD_U8BE},
                                     {H5T_STD_U8LE,NODE_H5T_STD_U8LE},
                                     {H5T_STD_U16BE,NODE_H5T_STD_U16BE},
                                     {H5T_STD_U16LE,NODE_H5T_STD_U16LE},
                                     {H5T_STD_U32BE,NODE_H5T_STD_U32BE},
                                     {H5T_STD_U32LE,NODE_H5T_STD_U32LE},
                                     {H5T_STD_U64BE,NODE_H5T_STD_U64BE},
                                     {H5T_STD_U64LE,NODE_H5T_STD_U64LE},
                                     {H5T_NATIVE_CHAR,NODE_H5T_NATIVE_CHAR},
                                     {H5T_NATIVE_SCHAR,NODE_H5T_NATIVE_SCHAR},
                                     {H5T_NATIVE_UCHAR,NODE_H5T_NATIVE_UCHAR},
                                     {H5T_NATIVE_SHORT,NODE_H5T_NATIVE_SHORT},
                                     {H5T_NATIVE_USHORT,NODE_H5T_NATIVE_USHORT},
                                     {H5T_NATIVE_INT,NODE_H5T_NATIVE_INT},
                                     {H5T_NATIVE_UINT,NODE_H5T_NATIVE_UINT},
                                     {H5T_NATIVE_LONG,NODE_H5T_NATIVE_LONG},
                                     {H5T_NATIVE_ULONG,NODE_H5T_NATIVE_ULONG},
                                     {H5T_NATIVE_LLONG,NODE_H5T_NATIVE_LLONG},
                                     {H5T_NATIVE_ULLONG,NODE_H5T_NATIVE_ULLONG},
                                     {H5T_IEEE_F32BE,NODE_H5T_IEEE_F32BE},
                                     {H5T_IEEE_F32LE,NODE_H5T_IEEE_F32LE},
                                     {H5T_IEEE_F64BE,NODE_H5T_IEEE_F64BE},
                                     {H5T_IEEE_F64LE,NODE_H5T_IEEE_F64LE},
                                     {H5T_NATIVE_FLOAT,NODE_H5T_NATIVE_FLOAT},
                                     {H5T_NATIVE_DOUBLE,NODE_H5T_NATIVE_DOUBLE},
                                     {H5T_NATIVE_LDOUBLE,NODE_H5T_NATIVE_LDOUBLE},
                                     {H5T_STRING,NODE_H5T_STRING},
                                     //{STRSIZE,NODE_STRSIZE},
                                     //{STRPAD,NODE_STRPAD},
                                     //{CSET,NODE_CSET},
                                     //{CTYPE,NODE_CTYPE},
                                     {H5T_VARIABLE,NODE_H5T_VARIABLE},
                                     {H5T_STR_NULLTERM,NODE_H5T_STR_NULLTERM},
                                     {H5T_STR_NULLPAD,NODE_H5T_STR_NULLPAD},
                                     {H5T_STR_SPACEPAD,NODE_H5T_STR_SPACEPAD},
                                     {H5T_CSET_ASCII,NODE_H5T_CSET_ASCII},
                                     {H5T_CSET_UTF8,NODE_H5T_CSET_UTF8},
                                     {H5T_C_S1,NODE_H5T_C_S1},
                                     {H5T_FORTRAN_S1,NODE_H5T_FORTRAN_S1},
                                     {H5T_OPAQUE,NODE_H5T_OPAQUE},
                                     //{NODE_OPQ_SIZE,OPQ_SIZE},
                                     //{NODE_OPQ_TAG,OPQ_TAG},
                                     {H5T_COMPOUND,NODE_H5T_COMPOUND},
                                     {H5T_ENUM,NODE_H5T_ENUM},
                                     {H5T_ARRAY,NODE_H5T_ARRAY},
                                     {H5T_VLEN,NODE_H5T_VLEN},
                                     //{STRING,NODE_STRING},
                                     //{NUMBER,NODE_NUMBER},
                                     {H5T_NATIVE_INT8,NODE_H5T_NATIVE_INT8},
                                     {H5T_NATIVE_UINT8,NODE_H5T_NATIVE_UINT8}};

    class H5lt {
    protected:

    public:
    static void Initialize (Handle<Object> target) {
        
        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::make_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::write_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetAsBuffer"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::readDatasetAsBuffer)->GetFunction());
        
    }

static void make_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    if(node::Buffer::HasInstance(args[2]))
    {
        Local<Value> encodingValue=args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding"));
        String::Utf8Value encoding (encodingValue->ToString());
        if(!std::strcmp("binary",(*encoding)))
        {
            herr_t err=H5LTmake_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)node::Buffer::Data(args[2]));
            if(err<0)
            {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
                args.GetReturnValue().SetUndefined();
                return;
            }
            args.GetReturnValue().SetUndefined();
            return;
        }
        hid_t type_id=toTypeMap[(H5T)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->ToInt32()->Value()];
        int rank=1;
        if(args[2]->ToObject()->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank")))
        {
            Local<Value> rankValue=args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
            rank=rankValue->ToInt32()->Value();
    //        std::cout<<"has rank "<<rank<<std::endl;
        }
        hsize_t dims[rank];
        switch(rank)
        {
            case 1:
                dims[0]={node::Buffer::Length(args[2])/H5Tget_size(type_id)};
                break;
            case 3:
                dims[2]=(hsize_t)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->ToInt32()->Value();
            case 2:
                dims[1]=(hsize_t)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value();
                dims[0]=(hsize_t)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value();
                break;
            default:
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
            args.GetReturnValue().SetUndefined();
            return;
                break;
        }
        unsigned int compression=0;
        if(args.Length() == 4){
             Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
             for(uint32_t index=0;index<names->Length();index++){
                 names->CloneElementAt(index);
                String::Utf8Value _name (names->Get(index));
                std::string name(*_name);
                if(name.compare("compression")==0){
                     compression=args[3]->ToObject()->Get(names->Get(index))->Uint32Value();
                }
             }
        }
        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
        if(compression>0){
                herr_t err=H5Pset_deflate(dcpl, compression );
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                err=H5Pset_chunk(dcpl, rank, dims);
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
        }
        herr_t err=H5LT_make_dataset_numerical (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(args[2]));
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        H5Pclose(dcpl);
        
    //Atributes
        v8::Local<v8::Array> propertyNames=args[2]->ToObject()->GetPropertyNames();
        for(unsigned int index=args[2]->ToObject()->GetIndexedPropertiesExternalArrayDataLength();index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!args[2]->ToObject()->Get(name)->IsFunction() && !args[2]->ToObject()->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
             {
//                std::cout<<index<<" "<<name->IsString()<<std::endl;
//                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<" rnp "<<buffer->HasRealNamedProperty( Local<String>::Cast(name))<<std::endl;
                if(args[2]->ToObject()->Get(name)->IsObject() || args[2]->ToObject()->Get(name)->IsExternal())
                {
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsUint32())
                {
                    uint32_t value=args[2]->ToObject()->Get(name)->ToUint32()->Uint32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsInt32())
                {
                    int32_t value=args[2]->ToObject()->Get(name)->ToInt32()->Int32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args[2]->ToObject()->Get(name)->ToString())));
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsNumber())
                {
                    double value=args[2]->ToObject()->Get(name)->ToNumber()->NumberValue();
//                std::cout<<index<<" "<<(*dset_name)<<" "<<name->IsString()<<" "<<value<<std::endl;
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
                    
                }
             }
        }
        return;
    }
    if(args[2]->IsString())
    {
        String::Utf8Value buffer (args[2]->ToString());
        herr_t err=H5LTmake_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)(*buffer));
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        return;
    }
    hid_t type_id;
    Local<TypedArray> buffer;
    if(args[2]->IsFloat64Array())
    {
        type_id=H5T_NATIVE_DOUBLE;
         buffer = Local<Float64Array>::Cast(args[2]);
    }
    else if(args[2]->IsFloat32Array())
    {
        type_id=H5T_NATIVE_FLOAT;
        buffer = Local<Float32Array>::Cast(args[2]);
    }
//    else if(args[2]->IsInt64Array())
//    {
//        type_id=H5T_NATIVE_LLONG;
//        buffer = Local<int64Array>::Cast(args[2]);
//    }
    else if(args[2]->IsInt32Array())
    {
        type_id=H5T_NATIVE_INT;
        buffer = Local<Int32Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint32Array())
    {
        type_id=H5T_NATIVE_UINT;
        buffer = Local<Uint32Array>::Cast(args[2]);
    }
    else if(args[2]->IsInt16Array())
    {
        type_id=H5T_NATIVE_SHORT;
        buffer = Local<Int16Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint16Array())
    {
        type_id=H5T_NATIVE_USHORT;
        buffer = Local<Uint16Array>::Cast(args[2]);
    }
    else if(args[2]->IsInt8Array())
    {
        type_id=H5T_NATIVE_INT8;
        buffer = Local<Int8Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint8Array())
    {
        type_id=H5T_NATIVE_UINT8;
        buffer = Local<Uint8Array>::Cast(args[2]);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    int rank=1;
    if(buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank")))
    {
        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank=rankValue->ToInt32()->Value();
//        std::cout<<"has rank "<<rank<<std::endl;
    }
    if(rank==1)
    {
        hsize_t dims[1]={buffer->Length()};
        unsigned int compression=0;
        if(args.Length() == 4){
             Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
             for(uint32_t index=0;index<names->Length();index++){
                 names->CloneElementAt(index);
                String::Utf8Value _name (names->Get(index));
                std::string name(*_name);
                if(name.compare("compression")==0){
                     compression=args[3]->ToObject()->Get(names->Get(index))->Uint32Value();
                }
             }
        }
        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
        if(compression>0){
                herr_t err=H5Pset_deflate(dcpl, compression );
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                err=H5Pset_chunk(dcpl, rank, dims);
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
        }
        herr_t err=H5LT_make_dataset_numerical (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        H5Pclose(dcpl);
    }
    else if(rank==2)
    {
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value();
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value();
        hsize_t dims[2]={(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value()};
        unsigned int compression=0;
        if(args.Length() == 4){
             Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
             for(uint32_t index=0;index<names->Length();index++){
                 names->CloneElementAt(index);
                String::Utf8Value _name (names->Get(index));
                std::string name(*_name);
                if(name.compare("compression")==0){
                     compression=args[3]->ToObject()->Get(names->Get(index))->Uint32Value();
                }
             }
        }
        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
        if(compression>0){
                herr_t err=H5Pset_deflate(dcpl, compression );
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                err=H5Pset_chunk(dcpl, rank, dims);
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
        }
        herr_t err=H5LT_make_dataset_numerical (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        H5Pclose(dcpl);
    }
    else if(rank==3)
    {
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value();
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value();
        hsize_t dims[3]={(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->ToInt32()->Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->ToInt32()->Value(),  (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->ToInt32()->Value()};
        unsigned int compression=0;
        if(args.Length() == 4){
             Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
             for(uint32_t index=0;index<names->Length();index++){
                 names->CloneElementAt(index);
                String::Utf8Value _name (names->Get(index));
                std::string name(*_name);
                if(name.compare("compression")==0){
                     compression=args[3]->ToObject()->Get(names->Get(index))->Uint32Value();
                }
             }
        }
        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
        if(compression>0){
                herr_t err=H5Pset_deflate(dcpl, compression );
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                err=H5Pset_chunk(dcpl, rank, dims);
                if (err < 0) {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
        }
        herr_t err=H5LT_make_dataset_numerical (args[0]->ToInt32()->Value(), *dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        H5Pclose(dcpl);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    
    //Atributes
        v8::Local<v8::Array> propertyNames=buffer->GetPropertyNames();
        for(unsigned int index=buffer->GetIndexedPropertiesExternalArrayDataLength();index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
             {
//                std::cout<<index<<" "<<name->IsString()<<std::endl;
//                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<" rnp "<<buffer->HasRealNamedProperty( Local<String>::Cast(name))<<std::endl;
                if(buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal())
                {
                    
                }
                else if(buffer->Get(name)->IsUint32())
                {
                    uint32_t value=buffer->Get(name)->ToUint32()->Uint32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);
                    
                }
                else if(buffer->Get(name)->IsInt32())
                {
                    int32_t value=buffer->Get(name)->ToInt32()->Int32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);
                    
                }
                else if(buffer->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
                    
                }
                else if(buffer->Get(name)->IsNumber())
                {
                    double value=buffer->Get(name)->ToNumber()->NumberValue();
//                std::cout<<index<<" "<<(*dset_name)<<" "<<name->IsString()<<" "<<value<<std::endl;
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
                    
                }
             }
        }

    args.GetReturnValue().SetUndefined();
}

static void write_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    // fail out if arguments are not correct
    if (args.Length() < 3 || args.Length() > 4 || !args[0]->IsUint32() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]")));
        args.GetReturnValue().SetUndefined();
        return;

    }
    String::Utf8Value dset_name (args[1]->ToString());
    std::cout<<"dset_name "<<(*dset_name)<<std::endl;
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank=1;
    herr_t err=H5LTget_dataset_ndims(args[0]->ToInt32()->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t values_dim[rank];
    err=H5LTget_dataset_info(args[0]->ToInt32()->Value(), *dset_name, values_dim, &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t theSize=bufSize;
    bool subsetOn=false;
    hsize_t start[rank];
    hsize_t stride[rank];
    hsize_t count[rank];
    if(args.Length() == 4){
         Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
         for(uint32_t index=0;index<names->Length();index++){
             names->CloneElementAt(index);
            String::Utf8Value _name (names->Get(index));
            std::string name(*_name);
            if(name.compare("start")==0){
                Local<Object> starts=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<starts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    start[arrayIndex]=starts->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("stride")==0){
                Local<Object> strides=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<strides->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    stride[arrayIndex]=strides->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("count")==0){
                 Local<Object> counts=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<counts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    count[arrayIndex]=counts->Get(arrayIndex)->Uint32Value();
                }
                subsetOn=true;
            }
         }
    }
    if(node::Buffer::HasInstance(args[2]))
    {
        Local<Value> encodingValue=args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding"));
        String::Utf8Value encoding (encodingValue->ToString());
        if(!std::strcmp("binary",(*encoding)))
        {
            hid_t did = H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
            herr_t err=H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)node::Buffer::Data(args[2]));
            if(err<0)
            {
                H5Dclose(did);
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset")));
                args.GetReturnValue().SetUndefined();
                return;
            }
            H5Dclose(did);
            args.GetReturnValue().SetUndefined();
            return;
        }
        hid_t type_id=toTypeMap[(H5T)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->ToInt32()->Value()];
        hid_t did = H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
        hid_t dataspace_id=H5S_ALL;
        hid_t memspace_id=H5S_ALL;
        if(subsetOn){
            memspace_id = H5Screate_simple (rank, count, NULL);
            dataspace_id = H5Dget_space (did);
            herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start,
                                          stride, count, NULL);
            if(err<0)
            {
                if(subsetOn){
                    H5Sclose (memspace_id);
                    H5Sclose (dataspace_id);
                }
                H5Dclose(did);
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        herr_t err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, node::Buffer::Data(args[2]));
        if(err<0)
        {
            if(subsetOn){
                H5Sclose (memspace_id);
                H5Sclose (dataspace_id);
            }
            H5Dclose(did);
            //H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        if(subsetOn){
            H5Sclose (memspace_id);
            H5Sclose (dataspace_id);
        }
        H5Dclose(did);
        //H5Pclose(dcpl);
        
    //Atributes
        /*v8::Local<v8::Array> propertyNames=args[2]->ToObject()->GetPropertyNames();
        for(unsigned int index=args[2]->ToObject()->GetIndexedPropertiesExternalArrayDataLength();index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!args[2]->ToObject()->Get(name)->IsFunction() && !args[2]->ToObject()->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
             {
//                std::cout<<index<<" "<<name->IsString()<<std::endl;
//                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<" rnp "<<buffer->HasRealNamedProperty( Local<String>::Cast(name))<<std::endl;
                if(args[2]->ToObject()->Get(name)->IsObject() || args[2]->ToObject()->Get(name)->IsExternal())
                {
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsUint32())
                {
                    uint32_t value=args[2]->ToObject()->Get(name)->ToUint32()->Uint32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsInt32())
                {
                    int32_t value=args[2]->ToObject()->Get(name)->ToInt32()->Int32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args[2]->ToObject()->Get(name)->ToString())));
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
                    
                }
                else if(args[2]->ToObject()->Get(name)->IsNumber())
                {
                    double value=args[2]->ToObject()->Get(name)->ToNumber()->NumberValue();
//                std::cout<<index<<" "<<(*dset_name)<<" "<<name->IsString()<<" "<<value<<std::endl;
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
                    
                }
             }
        }*/
        args.GetReturnValue().SetUndefined();
        return;
    }
    if(args[2]->IsString())
    {
        String::Utf8Value buffer (args[2]->ToString());
        hid_t did = H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
        herr_t err = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (char*)(*buffer));
        if(err<0)
        {
            H5Dclose(did);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to overwrite char dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        H5Dclose(did);
        return;
    }
    hid_t type_id;
    Local<TypedArray> buffer;
    if(args[2]->IsFloat64Array())
    {
        type_id=H5T_NATIVE_DOUBLE;
         buffer = Local<Float64Array>::Cast(args[2]);
    }
    else if(args[2]->IsFloat32Array())
    {
        type_id=H5T_NATIVE_FLOAT;
        buffer = Local<Float32Array>::Cast(args[2]);
    }
//    else if(args[2]->IsInt64Array())
//    {
//        type_id=H5T_NATIVE_LLONG;
//        buffer = Local<int64Array>::Cast(args[2]);
//    }
    else if(args[2]->IsInt32Array())
    {
        type_id=H5T_NATIVE_INT;
        buffer = Local<Int32Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint32Array())
    {
        type_id=H5T_NATIVE_UINT;
        buffer = Local<Uint32Array>::Cast(args[2]);
    }
    else if(args[2]->IsInt16Array())
    {
        type_id=H5T_NATIVE_SHORT;
        buffer = Local<Int16Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint16Array())
    {
        type_id=H5T_NATIVE_USHORT;
        buffer = Local<Uint16Array>::Cast(args[2]);
    }
    else if(args[2]->IsInt8Array())
    {
        type_id=H5T_NATIVE_INT8;
        buffer = Local<Int8Array>::Cast(args[2]);
    }
    else if(args[2]->IsUint8Array())
    {
        type_id=H5T_NATIVE_UINT8;
        buffer = Local<Uint8Array>::Cast(args[2]);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        args.GetReturnValue().SetUndefined();
        return;
    }
        hid_t did = H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT);
        hid_t dataspace_id=H5S_ALL;
        hid_t memspace_id=H5S_ALL;
        if(subsetOn){
            memspace_id = H5Screate_simple (rank, count, NULL);
            dataspace_id = H5Dget_space (did);
            herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start,
                                          stride, count, NULL);
            if(err<0)
            {
                if(subsetOn){
                    H5Sclose (memspace_id);
                    H5Sclose (dataspace_id);
                }
                H5Dclose(did);
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
                args.GetReturnValue().SetUndefined();
                return;
            }
        }
        err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, buffer->Buffer()->Externalize().Data());
        if(err<0)
        {
            if(subsetOn){
                H5Sclose (memspace_id);
                H5Sclose (dataspace_id);
            }
            H5Dclose(did);
            //H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            args.GetReturnValue().SetUndefined();
            return;
        }
        if(subsetOn){
            H5Sclose (memspace_id);
            H5Sclose (dataspace_id);
        }
        H5Dclose(did);
        //H5Pclose(dcpl);
    //Atributes
        /*v8::Local<v8::Array> propertyNames=buffer->GetPropertyNames();
        for(unsigned int index=buffer->GetIndexedPropertiesExternalArrayDataLength();index<propertyNames->Length();index++)
        {
             v8::Local<v8::Value> name=propertyNames->Get (index);
             if(!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
             {
//                std::cout<<index<<" "<<name->IsString()<<std::endl;
//                std::cout<<index<<" "<<(*String::Utf8Value(name->ToString()))<<" rnp "<<buffer->HasRealNamedProperty( Local<String>::Cast(name))<<std::endl;
                if(buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal())
                {
                    
                }
                else if(buffer->Get(name)->IsUint32())
                {
                    uint32_t value=buffer->Get(name)->ToUint32()->Uint32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);
                    
                }
                else if(buffer->Get(name)->IsInt32())
                {
                    int32_t value=buffer->Get(name)->ToInt32()->Int32Value();
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);
                    
                }
                else if(buffer->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());
                    
                }
                else if(buffer->Get(name)->IsNumber())
                {
                    double value=buffer->Get(name)->ToNumber()->NumberValue();
//                std::cout<<index<<" "<<(*dset_name)<<" "<<name->IsString()<<" "<<value<<std::endl;
                    if(H5Aexists_by_name(args[0]->ToInt32()->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(args[0]->ToInt32()->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);
                    
                }
             }
        }*/

    args.GetReturnValue().SetUndefined();
}

static void read_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    herr_t err=H5LTget_dataset_ndims(args[0]->ToInt32()->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t values_dim[rank];
    err=H5LTget_dataset_info(args[0]->ToInt32()->Value(), *dset_name, values_dim, &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t theSize=bufSize;
          switch(rank)
          {
              case 3:
              theSize=values_dim[0]*values_dim[1]*values_dim[2];
              break;
              case 2:
              theSize=values_dim[0]*values_dim[1];
              break;
              case 1:
                  theSize=values_dim[0];
                  break;
              case 0:
                  theSize=bufSize;
                  break;
              default:
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
                args.GetReturnValue().SetUndefined();
                return;
                break;
          }
        //std::cout<<"rank "<<rank<<" "<<bufSize<<" "<<values_dim[0]<<" "<<class_id<<std::endl;
        switch(class_id)
        { 
            case H5T_STRING:
            {
                std::string buffer(theSize+1, 0);
                err=H5LTread_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)buffer.c_str());
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
//                std::cout<<"c side\n"<<buffer<<std::endl;
                args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
            }
                break;
            default:
        
          hid_t type_id;
              Local<ArrayBuffer> arrayBuffer=ArrayBuffer::New(v8::Isolate::GetCurrent(), bufSize*theSize);
                Local<TypedArray> buffer;
                if(class_id==H5T_FLOAT && bufSize==8)
                {
                    type_id=H5T_NATIVE_DOUBLE;
                    buffer = Float64Array::New(arrayBuffer, 0, theSize);
                }
                else if(class_id==H5T_FLOAT && bufSize==4)
                {
                    type_id=H5T_NATIVE_FLOAT;
                    buffer = Float32Array::New(arrayBuffer, 0, theSize);
                }
//                else if(class_id==H5T_INTEGER && bufSize==8)
//                {
//                    type_id=H5T_NATIVE_LLONG;
//                    buffer = int64Array::New(arrayBuffer, 0, (size_t)values_dim[0]);
//                }
                else if(class_id==H5T_INTEGER && bufSize==4)
                {
                    hid_t h=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    hid_t t=H5Dget_type(h);
                    if(H5Tget_sign(H5Dget_type(h))==H5T_SGN_2)
                    {
                        type_id=H5T_NATIVE_INT;
                        buffer = Int32Array::New(arrayBuffer, 0, theSize);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT;
                        buffer = Uint32Array::New(arrayBuffer, 0, theSize);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else if(class_id==H5T_INTEGER && bufSize==2)
                {
                    hid_t h=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    hid_t t=H5Dget_type(h);
                    if(H5Tget_sign(H5Dget_type(h))==H5T_SGN_2)
                    {
                        type_id=H5T_NATIVE_SHORT;
                        buffer = Int16Array::New(arrayBuffer, 0, theSize);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_USHORT;
                        buffer = Uint16Array::New(arrayBuffer, 0, theSize);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else if(class_id==H5T_INTEGER && bufSize==1)
                {
                    hid_t h=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    hid_t t=H5Dget_type(h);
                    if(H5Tget_sign(H5Dget_type(h))==H5T_SGN_2)
                    {
                        type_id=H5T_NATIVE_INT8;
                        buffer = Int8Array::New(arrayBuffer, 0, theSize);
                    }
                    else
                    {
                        type_id=H5T_NATIVE_UINT8;
                        buffer = Uint8Array::New(arrayBuffer, 0, theSize);
                    }
                    H5Tclose(t);
                    H5Dclose(h);
                }
                else
                {
                          v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                          args.GetReturnValue().SetUndefined();
                          return;
                }
//                hsize_t dims[1]={buffer->Length()};
//                buffer->;
                err=H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, type_id, buffer->Buffer()->Externalize().Data() );
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
                switch(rank)
                {
                    case 3:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[1]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim[2]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                        break;
                    case 2:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                        if(rank>1)buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim[1]));
                        break;
                    case 1:
                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                    break;
                }
                
        //Attributes
        uint32_t index=0;
        hsize_t idx=0;
        std::vector<std::string> holder;
//        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//        }, &index, &holder);
        H5Aiterate_by_name(args[0]->ToInt32()->Value(), *dset_name, H5_INDEX_CRT_ORDER ,
        H5_ITER_INC , &idx, [] (hid_t location_id, const char* attr_name, const H5A_info_t *ainfo, void *operator_data){
                if(ainfo->data_size>0)((std::vector<std::string>*)operator_data)->push_back(std::string(attr_name));
                return (herr_t)((std::vector<std::string>*)operator_data)->size();
            }, (void*)&holder, H5P_DEFAULT);
//            std::cout<<(*dset_name)<<" attr loop "<<holder.size()<<" "<<idx<<std::endl;
        for(index=0;index<(uint32_t)holder.size();index++)
        {
            hsize_t values_dim[1] = {1};
            size_t bufSize = 0;
            H5T_class_t class_id;
            err = H5LTget_attribute_info(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), values_dim, &class_id, &bufSize);
            if (err >=0) {
                switch(class_id)
                {
                    case H5T_INTEGER:
                        long long intValue;
                         H5LTget_attribute_int(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), (int*)&intValue);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                        break;
                    case H5T_FLOAT:
                        double value;
                         H5LTget_attribute_double(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), &value);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                        break;
                    case H5T_STRING:
                    {
        //                std::cout<<"H5T_STRING "<<attr.getDataType().getSize ()<<" "<<attr.getDataType().fromClass()<<std::endl;
                        std::string strValue(bufSize+1,'\0');
                         H5LTget_attribute_string(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), (char *)strValue.c_str());
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
                    }
                        break;
                    case H5T_NO_CLASS:
                    default:
        //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        //                    args.GetReturnValue().SetUndefined();
//                            return;
                        break;
                }
            }
        }
                
                //
                args.GetReturnValue().Set(buffer);
                break;
        }
}
static void readDatasetAsBuffer (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if (args.Length() < 2 || args.Length() > 3 || !args[0]->IsUint32() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    herr_t err=H5LTget_dataset_ndims(args[0]->ToInt32()->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t values_dim[rank];
    err=H5LTget_dataset_info(args[0]->ToInt32()->Value(), *dset_name, values_dim, &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t theSize=bufSize;
    bool subsetOn=false;
    hsize_t start[rank];
    hsize_t stride[rank];
    hsize_t count[rank];
    if(args.Length() == 3){
         Local<Array> names=args[2]->ToObject()->GetOwnPropertyNames();
         for(uint32_t index=0;index<names->Length();index++){
             names->CloneElementAt(index);
            String::Utf8Value _name (names->Get(index));
            std::string name(*_name);
            if(name.compare("start")==0){
                Local<Object> starts=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<starts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    start[arrayIndex]=starts->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("stride")==0){
                Local<Object> strides=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<strides->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    stride[arrayIndex]=strides->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("count")==0){
                 Local<Object> counts=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<counts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    count[arrayIndex]=counts->Get(arrayIndex)->Uint32Value();
                }
                subsetOn=true;
            }
         }
    }
          switch(rank)
          {
              case 3:
              theSize=values_dim[0]*values_dim[1]*values_dim[2];
              break;
              case 2:
              theSize=values_dim[0]*values_dim[1];
              break;
              case 1:
                  theSize=values_dim[0];
                  break;
              case 0:
                  theSize=bufSize;
                  break;
              default:
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
                args.GetReturnValue().SetUndefined();
                return;
                break;
          }
        //std::cout<<"rank "<<rank<<" "<<bufSize<<" "<<values_dim[0]<<" "<<class_id<<std::endl;
        switch(class_id)
        { 
            case H5T_STRING:
            {
                std::string buffer(theSize+1, 0);
                err=H5LTread_dataset_string (args[0]->ToInt32()->Value(), *dset_name,  (char*)buffer.c_str());
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
//                std::cout<<"c side\n"<<buffer<<std::endl;
//                args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
                args.GetReturnValue().Set(node::Buffer::New(v8::Isolate::GetCurrent(), buffer.c_str(), theSize));
            }
                break;
            case H5T_INTEGER:
            case H5T_FLOAT:
            case H5T_BITFIELD:
            {
                
                hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                hid_t t=H5Dget_type(did);
                hid_t type_id=H5Tget_native_type(t,H5T_DIR_ASCEND);
                hid_t dataspace_id=H5S_ALL;
                hid_t memspace_id=H5S_ALL;
                if(subsetOn){
                    memspace_id = H5Screate_simple (rank, count, NULL);
                    dataspace_id = H5Dget_space (did);
            std::cout<<"H5Sselect_hyperslab start "<<(start[0])<<" "<<(start[1])<<std::endl;
            std::cout<<"H5Sselect_hyperslab stride "<<(stride[0])<<" "<<(stride[1])<<std::endl;
            std::cout<<"H5Sselect_hyperslab count "<<(count[0])<<" "<<(count[1])<<std::endl;
                    herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start,
                                                  stride, count, NULL);
            std::cout<<"H5Sselect_hyperslab err "<<(err)<<std::endl;
                    if(err<0)
                    {
                        if(subsetOn){
                            H5Sclose (memspace_id);
                            H5Sclose (dataspace_id);
                        }
                        H5Dclose(did);
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to select hyperslab")));
                        args.GetReturnValue().SetUndefined();
                        return;
                    }
                    theSize=1;
                    for(unsigned int rankIndex=0;rankIndex<rank;rankIndex++){
                        theSize*=count[rankIndex];
                    }
                }
                v8::Local<v8::Object> buffer=node::Buffer::New(v8::Isolate::GetCurrent(),bufSize*theSize);
                err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, (char*)node::Buffer::Data(buffer));
                //err=H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, type_id, (char*)node::Buffer::Data(buffer) );
                if(err<0)
                {
                    if(subsetOn){
                        H5Sclose (memspace_id);
                        H5Sclose (dataspace_id);
                    }
                    H5Tclose(t);
                    H5Dclose(did);
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"), Int32::New(v8::Isolate::GetCurrent(), toEnumMap[type_id]));
                H5Tclose(t);
                if(subsetOn){
                    H5Sclose (memspace_id);
                    H5Sclose (dataspace_id);
                    H5Dclose(did);
                    args.GetReturnValue().Set(buffer);
                    return;
                }
                H5Dclose(did);
                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
                switch(rank)
                {
                    case 3:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[1]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim[2]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                        break;
                    case 2:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                        if(rank>1)buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim[1]));
                        break;
                    case 1:
                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim[0]));
                    break;
                }
                
        //Attributes
        uint32_t index=0;
        hsize_t idx=0;
        std::vector<std::string> holder;
//        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//        }, &index, &holder);
        H5Aiterate_by_name(args[0]->ToInt32()->Value(), *dset_name, H5_INDEX_CRT_ORDER ,
        H5_ITER_INC , &idx, [] (hid_t location_id, const char* attr_name, const H5A_info_t *ainfo, void *operator_data){
                if(ainfo->data_size>0)((std::vector<std::string>*)operator_data)->push_back(std::string(attr_name));
                return (herr_t)((std::vector<std::string>*)operator_data)->size();
            }, (void*)&holder, H5P_DEFAULT);
//            std::cout<<(*dset_name)<<" attr loop "<<holder.size()<<" "<<idx<<std::endl;
        for(index=0;index<(uint32_t)holder.size();index++)
        {
            hsize_t values_dim[1] = {1};
            size_t bufSize = 0;
            H5T_class_t class_id;
            err = H5LTget_attribute_info(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), values_dim, &class_id, &bufSize);
            if (err >=0) {
                switch(class_id)
                {
                    case H5T_INTEGER:
                        long long intValue;
                         H5LTget_attribute_int(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), (int*)&intValue);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                        break;
                    case H5T_FLOAT:
                        double value;
                         H5LTget_attribute_double(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), &value);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                        break;
                    case H5T_STRING:
                    {
        //                std::cout<<"H5T_STRING "<<attr.getDataType().getSize ()<<" "<<attr.getDataType().fromClass()<<std::endl;
                        std::string strValue(bufSize+1,'\0');
                         H5LTget_attribute_string(args[0]->ToInt32()->Value(), *dset_name, holder[index].c_str(), (char *)strValue.c_str());
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), String::NewFromUtf8(v8::Isolate::GetCurrent(), strValue.c_str()));
                    }
                        break;
                    case H5T_NO_CLASS:
                    default:
        //                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
        //                    args.GetReturnValue().SetUndefined();
//                            return;
                        break;
                }
            }
        }
                
                //
                args.GetReturnValue().Set(buffer);
            }
                break;
            default:
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
                            args.GetReturnValue().SetUndefined();
                            return;
                break;
        }
}
    };
}