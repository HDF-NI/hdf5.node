#pragma once
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_buffer.h>

#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include "file.h"
#include "group.h"
#include "int64.hpp"
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
    const hsize_t maxsize = H5S_UNLIMITED;
    if((sid = H5Screate_simple(rank, dims, &maxsize)) < 0)
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

    class H5lt {
    protected:

    public:
    static void Initialize (Handle<Object> target) {

        // append this function to the target object
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "makeDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::make_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "writeDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::write_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "lengthDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::length_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDataset"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::read_dataset)->GetFunction());
        target->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "readDatasetAsBuffer"), FunctionTemplate::New(v8::Isolate::GetCurrent(), H5lt::readDatasetAsBuffer)->GetFunction());

    }

    static unsigned int get_fixed_width(Handle<Object> options) {
        if(options.IsEmpty()) {
            return 0;
        }

        auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "fixed_width"));

        if(!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
            return 0;
        }

        return options->Get(name)->Uint32Value();
    }

    static unsigned int get_compression(Handle<Object> options) {
        if(options.IsEmpty()) {
            return 0;
        }

        auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "compression"));

        if(!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
            return 0;
        }

        return options->Get(name)->Uint32Value();
    }

    static unsigned int get_chunk_size(Handle<Object> options) {
        if(options.IsEmpty()) {
            return 0;
        }

        auto name(String::NewFromUtf8(v8::Isolate::GetCurrent(), "chunkSize"));

        if(!options->HasOwnProperty(v8::Isolate::GetCurrent()->GetCurrentContext(), name).FromJust()) {
            return 0;
        }

        return options->Get(name)->Uint32Value();
    }

    // TODO: permit other that square geometry
    static bool configure_chunked_layout(const hid_t &dcpl, const unsigned int &size, const int &rank, const hsize_t *ds_dim) {
        herr_t err;
        if(!size) {
            err = H5Pset_chunk(dcpl, rank, ds_dim);
            if(err) {
                v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
                return false;
            }

            return true;
        }

        std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
        for(int i=0; i<rank; ++i) {
            dims.get()[i] = size;
        }
        err = H5Pset_chunk(dcpl, rank, dims.get());
        if(err) {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set chunked layout")));
            return false;
        }

        return true;
    }

    static bool configure_compresion(const hid_t &dcpl, const unsigned int &compression) {
        herr_t err=H5Pset_deflate(dcpl, compression );
        if (err < 0) {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "Failed to set zip filter")));
            return false;
        }

        return true;
    }

static void make_dataset_from_buffer(const hid_t &group_id, const char *dset_name, Handle<Object> buffer, Handle<Object> options) {
    Local<Value> encodingValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "encoding"));
    String::Utf8Value encoding (encodingValue->ToString());
    if(!std::strcmp("binary",(*encoding)))
    {
        herr_t err=H5LTmake_dataset_string (group_id, dset_name,  const_cast<char*>(node::Buffer::Data(buffer)));
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
            return;
        }
        return;
    }
    hid_t type_id=toTypeMap[(H5T)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->Int32Value()];
    int rank=1;
    if(buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank")))
    {
        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank=rankValue->Int32Value();
    }
    std::unique_ptr<hsize_t[]> dims(new hsize_t[rank]);
    switch(rank)
    {
        case 1:
            dims.get()[0]={node::Buffer::Length(buffer)/H5Tget_size(type_id)};
            break;
        case 3:
            dims.get()[2]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->Int32Value();
        case 2:
            dims.get()[1]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
            dims.get()[0]=(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
            break;
        default:
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
        return;
            break;
    }
    unsigned int compression= get_compression(options);
    unsigned int chunk_size = get_chunk_size(options);
    hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
    if(compression>0){
        if(!configure_compresion(dcpl, compression)) {
            return;
        }
    }

    // if(compression>0 || chunk_size > 0) {
        if(!configure_chunked_layout(dcpl, chunk_size, rank, dims.get())) {
            return;
        }
    // }

    herr_t err=H5LT_make_dataset_numerical (group_id, dset_name, rank, dims.get(), type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, node::Buffer::Data(buffer));
    if(err<0)
    {
        H5Pclose(dcpl);
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
        return;
    }
    H5Pclose(dcpl);

    //Atributes
    v8::Local<v8::Array> propertyNames=buffer->GetPropertyNames();
    for(unsigned int index=propertyNames->Length();index<propertyNames->Length();index++)
    {
         v8::Local<v8::Value> name=propertyNames->Get (index);
         if(!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
         {
            if(buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal())
            {

            }
            else if(buffer->Get(name)->IsUint32())
            {
                uint32_t value=buffer->Get(name)->Uint32Value();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

            }
            else if(buffer->Get(name)->IsInt32())
            {
                int32_t value=buffer->Get(name)->Int32Value();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

            }
            else if(buffer->Get(name)->IsString())
            {
                std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());

            }
            else if(buffer->Get(name)->IsNumber())
            {
                double value=buffer->Get(name)->NumberValue();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);

            }
        }
    }
}

static void make_dataset_from_typed_array(const hid_t &group_id, const char *dset_name, Handle<TypedArray> buffer, Handle<Object> options, hid_t type_id) {
    int rank=1;
    if(buffer->Has(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank")))
    {
        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"));
        rank=rankValue->Int32Value();
    }

    if(rank==1)
    {
        hsize_t dims[1]={buffer->Length()};
        unsigned int compression= get_compression(options);
        unsigned int chunk_size = get_chunk_size(options);

        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);

        if(compression>0){
            if(!configure_compresion(dcpl, compression)) {
                return;
            }
        }

        // if(compression>0 || chunk_size > 0) {
            if(!configure_chunked_layout(dcpl, chunk_size, rank, dims)) {
                return;
            }
        // }

        herr_t err=H5LT_make_dataset_numerical (group_id, dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            return;
        }
        H5Pclose(dcpl);
    }
    else if(rank==2)
    {
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value();
//        Local<Value> rankValue=buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value();
        hsize_t dims[2]={(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value()};
        unsigned int compression= get_compression(options);
        unsigned int chunk_size = get_chunk_size(options);

        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);

        if(compression>0){
            if(!configure_compresion(dcpl, compression)) {
                return;
            }
        }

        // if(compression>0 || chunk_size > 0) {
            if(!configure_chunked_layout(dcpl, chunk_size, rank, dims)) {
                return;
            }
        // }

        herr_t err=H5LT_make_dataset_numerical (group_id, dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            return;
        }
        H5Pclose(dcpl);
    }
    else if(rank==3)
    {
        hsize_t dims[3]={(hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"))->Int32Value(), (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"))->Int32Value(),  (hsize_t)buffer->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"))->Int32Value()};
        unsigned int compression= get_compression(options);
        unsigned int chunk_size = get_chunk_size(options);

        hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);

        if(compression>0){
            if(!configure_compresion(dcpl, compression)) {
                return;
            }
        }

        // if(compression>0 || chunk_size > 0) {
            if(!configure_chunked_layout(dcpl, chunk_size, rank, dims)) {
                return;
            }
        // }

        herr_t err=H5LT_make_dataset_numerical (group_id, dset_name, rank, dims, type_id, H5P_DEFAULT, dcpl, H5P_DEFAULT, buffer->Buffer()->Externalize().Data() );
        if(err<0)
        {
            H5Pclose(dcpl);
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make dataset")));
            return;
        }
        H5Pclose(dcpl);
    }
    else
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported rank")));
        return;
    }

    //Atributes
    v8::Local<v8::Array> propertyNames=buffer->GetPropertyNames();
    for(unsigned int index=buffer->Length();index<propertyNames->Length();index++)
    {
         v8::Local<v8::Value> name=propertyNames->Get (index);
         if(!buffer->Get(name)->IsFunction() && !buffer->Get(name)->IsArray() && strncmp("id",(*String::Utf8Value(name->ToString())), 2)!=0 && strncmp("rank",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("rows",(*String::Utf8Value(name->ToString())), 4)!=0 && strncmp("columns",(*String::Utf8Value(name->ToString())), 7)!=0 && strncmp("buffer",(*String::Utf8Value(name->ToString())), 6)!=0)
         {
            if(buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal())
            {

            }
            else if(buffer->Get(name)->IsUint32())
            {
                uint32_t value=buffer->Get(name)->Uint32Value();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_uint(group_id, dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

            }
            else if(buffer->Get(name)->IsInt32())
            {
                int32_t value=buffer->Get(name)->Int32Value();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_int(group_id, dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

            }
            else if(buffer->Get(name)->IsString())
            {
                std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                H5LTset_attribute_string(group_id, dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());

            }
            else if(buffer->Get(name)->IsNumber())
            {
                double value=buffer->Get(name)->NumberValue();
                if(H5Aexists_by_name(group_id, dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                {
                    H5Adelete_by_name(group_id, dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                }
                H5LTset_attribute_double(group_id, dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);

            }
        }
    }
}

static void make_dataset_from_array(const hid_t &group_id, const char *dset_name, Handle<Array> array, Handle<Object> options) {
    hid_t dcpl=H5Pcreate(H5P_DATASET_CREATE);
    int rank=1;
    unsigned int fixedWidth=get_fixed_width(options);
    if(fixedWidth>0){
        std::unique_ptr<char[]> vl(new char[fixedWidth*array->Length()]);
        std::memset(vl.get(), 0, fixedWidth*array->Length());
        for(unsigned int arrayIndex=0;arrayIndex<array->Length();arrayIndex++){
            String::Utf8Value buffer (array->Get(arrayIndex)->ToString());
            std::string s(*buffer);
            if(fixedWidth<s.length()){
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed fixed length to make var len dataset")));
            return;

            }
            std::strncpy(&vl.get()[fixedWidth*arrayIndex], s.c_str(), s.length());
        }
        std::unique_ptr<hsize_t[]> countSpace(new hsize_t[rank]);
        countSpace.get()[0]=array->Length();
        hid_t memspace_id = H5Screate_simple (rank, countSpace.get(), NULL);
        hid_t type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(type_id, fixedWidth);
        hid_t did = H5Dcreate(group_id, dset_name, type_id, memspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);
            //if(arrayIndex==0){
        herr_t err=H5Dwrite( did, type_id, memspace_id, H5S_ALL, H5P_DEFAULT, vl.get() );
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset")));
            return;
        }
            //}
        //H5Tclose(arraytype_id);
        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
    }
    else{
        std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
        count.get()[0]=array->Length();
        std::unique_ptr<char*[]> vl(new char*[array->Length()]);
        for(unsigned int arrayIndex=0;arrayIndex<array->Length();arrayIndex++){
            String::Utf8Value buffer (array->Get(arrayIndex)->ToString());
            std::string s(*buffer);
            vl.get()[arrayIndex]=new char[s.length()+1];
            std::memset(vl.get()[arrayIndex], 0, s.length()+1);
            std::strncpy(vl.get()[arrayIndex], s.c_str(), s.length());

        }
        //hsize_t maxdims[1]={H5S_UNLIMITED};
        hid_t memspace_id = H5Screate_simple (rank, count.get(), NULL);
        hid_t type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(type_id, H5T_VARIABLE);
        hid_t did = H5Dcreate(group_id, dset_name, type_id, memspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);
            //if(arrayIndex==0){
        herr_t err=H5Dwrite( did, type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, vl.get() );
        if(err<0)
        {
            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make var len dataset")));
            return;
        }
            //}
        H5Tclose(type_id);
        H5Dclose(did);
        H5Sclose(memspace_id);
        H5Pclose(dcpl);
    }
}

static void make_dataset_from_string(const hid_t &group_id, const char *dset_name, Handle<String> buffer, Handle<Object> /*options*/) {
    String::Utf8Value str_buffer (buffer);
    herr_t err=H5LTmake_dataset_string (group_id, dset_name, const_cast<char*>(*str_buffer));
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to make char dataset")));
        return;
    }
}

static void make_dataset (const v8::FunctionCallbackInfo<Value>& args)
{
    String::Utf8Value dset_name_ptr (args[1]->ToString());
    const char *dset_name(*dset_name_ptr);
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    hid_t group_id = idWrap->Value();
    Local<Value> buffer = args[2];
    Local<Object> options;
    if(args.Length() >= 4 && args[3]->IsObject()) {
        options = args[3]->ToObject();
    }

    if(node::Buffer::HasInstance(buffer))
    {
        make_dataset_from_buffer(group_id, dset_name, buffer->ToObject(), options);
        return;
    }

    if(buffer->IsString())
    {
        make_dataset_from_string(group_id, dset_name, buffer->ToString(), options);
        return;
    }

    if(buffer->IsArray())
    {
        make_dataset_from_array(group_id, dset_name, Local<v8::Array>::Cast(buffer), options);
        return;
    }

    if(buffer->IsFloat64Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Float64Array>::Cast(buffer), options, H5T_NATIVE_DOUBLE);
        return;
    }
    if(buffer->IsFloat32Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Float32Array>::Cast(buffer), options, H5T_NATIVE_FLOAT);
        return;
    }
//    if(buffer->IsInt64Array())
//    {
//        make_dataset_from_typed_array(group_id, dset_name, Local<int64Array>::Cast(buffer), options, H5T_NATIVE_LLONG);
//        return;
//    }
    if(buffer->IsInt32Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int32Array>::Cast(buffer), options, H5T_NATIVE_INT);
        return;
    }
    if(buffer->IsUint32Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint32Array>::Cast(buffer), options, H5T_NATIVE_UINT);
        return;
    }
    if(buffer->IsInt16Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int16Array>::Cast(buffer), options, H5T_NATIVE_SHORT);
        return;
    }
    if(buffer->IsUint16Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint16Array>::Cast(buffer), options, H5T_NATIVE_USHORT);
        return;
    }
    if(buffer->IsInt8Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Int8Array>::Cast(buffer), options, H5T_NATIVE_INT8);
        return;
    }
    if(buffer->IsUint8Array())
    {
        make_dataset_from_typed_array(group_id, dset_name, Local<Uint8Array>::Cast(buffer), options, H5T_NATIVE_UINT8);
        return;
    }

    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "unsupported data type")));
}

static void write_dataset (const v8::FunctionCallbackInfo<Value>& args)
{

    // fail out if arguments are not correct
    if (args.Length() < 3 || args.Length() > 4 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]")));
        args.GetReturnValue().SetUndefined();
        return;

    }
    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank=1;
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    herr_t err=H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
    err=H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    //hsize_t theSize=bufSize;
    bool subsetOn=false;
    std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
    std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
    std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
    if(args.Length() == 4){
         Local<Array> names=args[3]->ToObject()->GetOwnPropertyNames();
         for(uint32_t index=0;index<names->Length();index++){
            String::Utf8Value _name (names->Get(index));
            std::string name(*_name);
            if(name.compare("start")==0){
                Local<Object> starts=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<starts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    start.get()[arrayIndex]=starts->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("stride")==0){
                Local<Object> strides=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<strides->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    stride.get()[arrayIndex]=strides->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("count")==0){
                 Local<Object> counts=args[3]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<counts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    count.get()[arrayIndex]=counts->Get(arrayIndex)->Uint32Value();
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
            Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
            hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
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
        hid_t type_id=toTypeMap[(H5T)args[2]->ToObject()->Get(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"))->Int32Value()];
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
        hid_t dataspace_id=H5S_ALL;
        hid_t memspace_id=H5S_ALL;
        if(subsetOn){
            memspace_id = H5Screate_simple (rank, count.get(), NULL);
            dataspace_id = H5Dget_space (did);
            herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start.get(),
                                          stride.get(), count.get(), NULL);
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
                if(args[2]->ToObject()->Get(name)->IsObject() || args[2]->ToObject()->Get(name)->IsExternal())
                {

                }
                else if(args[2]->ToObject()->Get(name)->IsUint32())
                {
                    uint32_t value=args[2]->ToObject()->Get(name)->Uint32Value();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

                }
                else if(args[2]->ToObject()->Get(name)->IsInt32())
                {
                    int32_t value=args[2]->ToObject()->Get(name)->Int32Value();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

                }
                else if(args[2]->ToObject()->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(args[2]->ToObject()->Get(name)->ToString())));
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());

                }
                else if(args[2]->ToObject()->Get(name)->IsNumber())
                {
                    double value=args[2]->ToObject()->Get(name)->NumberValue();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);

                }
             }
        }*/
        args.GetReturnValue().SetUndefined();
        return;
    }
    if(args[2]->IsString())
    {
        String::Utf8Value buffer (args[2]->ToString());
        Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
        hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
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
    if (args[2]->IsArray()) {
      write_dataset_from_array(args, Local<Array>::Cast(args[2]), subsetOn, start, stride, count);
      return;
    }
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
        hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
        hid_t dataspace_id=H5S_ALL;
        hid_t memspace_id=H5S_ALL;
        hsize_t dims;
        hsize_t maxdims;
        H5Sget_simple_extent_dims(dataspace_id, &dims, &maxdims);
        const int remainingRows = dims - (*start.get() + *count.get());
        if (remainingRows < 0) {
          dims -= remainingRows;
          H5Dset_extent(did, &dims);
          H5Sset_extent_simple(dataspace_id, rank, &dims, &maxdims);
        }
        if(subsetOn){
            memspace_id = H5Screate_simple (rank, count.get(), NULL);
            dataspace_id = H5Dget_space (did);
            herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start.get(),
                                          stride.get(), count.get(), NULL);
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
                if(buffer->Get(name)->IsObject() || buffer->Get(name)->IsExternal())
                {

                }
                else if(buffer->Get(name)->IsUint32())
                {
                    uint32_t value=buffer->Get(name)->Uint32Value();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_uint(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (unsigned int*)&value, 1);

                }
                else if(buffer->Get(name)->IsInt32())
                {
                    int32_t value=buffer->Get(name)->Int32Value();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_int(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (int*)&value, 1);

                }
                else if(buffer->Get(name)->IsString())
                {
                    std::string value((*String::Utf8Value(buffer->Get(name)->ToString())));
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
//                     H5::DataSpace ds(H5S_SIMPLE);
//                     const long long unsigned int currentExtent=name->ToString()->Utf8Length();
//                     ds.setExtentSimple(1, &currentExtent);
                    H5LTset_attribute_string(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (const char*)value.c_str());

                }
                else if(buffer->Get(name)->IsNumber())
                {
                    double value=buffer->Get(name)->NumberValue();
                    if(H5Aexists_by_name(idWrap->Value(), *dset_name,  (*String::Utf8Value(name->ToString())), H5P_DEFAULT)>0)
                    {
                        H5Adelete_by_name(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), H5P_DEFAULT);
                    }
                    H5LTset_attribute_double(idWrap->Value(), *dset_name, (*String::Utf8Value(name->ToString())), (double*)&value, 1);

                }
             }
        }*/

    args.GetReturnValue().SetUndefined();
}

static void write_dataset_from_array(const v8::FunctionCallbackInfo<Value>& args,
    Local<v8::Array> array,
    bool subsetOn,
    std::unique_ptr<hsize_t[]>& start,
    std::unique_ptr<hsize_t[]>& stride,
    std::unique_ptr<hsize_t[]>& count
    ) {
  int rank = 1;
  String::Utf8Value dset_name (args[1]->ToString());
  Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
  hid_t did = H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT);
  hid_t dataspace_id=H5Dget_space(did);
  hid_t memspace_id=H5S_ALL;
  hsize_t dims;
  hsize_t maxdims;
  herr_t err;
  H5Sget_simple_extent_dims(dataspace_id, &dims, &maxdims);
  const int remainingRows = dims - (*start.get() + *count.get());
  if (remainingRows < 0) {
    dims -= remainingRows;
    H5Dset_extent(did, &dims);
    H5Sset_extent_simple(dataspace_id, rank, &dims, &maxdims);
  }
  if(subsetOn){
      memspace_id = H5Screate_simple (rank, count.get(), NULL);
      herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start.get(),
                                    stride.get(), count.get(), NULL);
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
  hid_t type_id = H5Tcopy(H5T_C_S1);
  H5Tset_size(type_id, H5T_VARIABLE);
  const unsigned int arraySize = array->Length();
    std::unique_ptr<char* []> vl(new char*[arraySize]);
    std::vector<std::unique_ptr<String::Utf8Value>> string_values;

    for(unsigned int arrayIndex=0; arrayIndex<arraySize; arrayIndex++) {
        std::unique_ptr<String::Utf8Value> value(new String::Utf8Value(array->Get(arrayIndex)));
        vl.get()[arrayIndex] = **value;
        string_values.emplace_back(std::move(value));
    }
  err = H5Dwrite(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, vl.get());
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

  args.GetReturnValue().SetUndefined();
}

static void length_dataset(const v8::FunctionCallbackInfo<Value>& args) {

  if (args.Length() != 2 || !args[0]->IsUint32() || !args[1]->IsString()) {
      v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
      args.GetReturnValue().SetUndefined();
      return;
  }

  const String::Utf8Value dataset_name (args[1]->ToString());
  const hid_t location_id = args[0]->ToInt32()->Value();

  const hid_t dataset = H5Dopen(location_id, *dataset_name, H5P_DEFAULT);
  const hid_t dataspace = H5Dget_space(dataset);

  const int rank = H5Sget_simple_extent_ndims(dataspace);

  hsize_t* dims = new hsize_t[rank];
  hsize_t* maxdims = new hsize_t[rank];
  H5Sget_simple_extent_dims(dataspace, dims, maxdims);

  args.GetReturnValue().Set(Int32::New(v8::Isolate::GetCurrent(), dims[0]));

  H5Sclose(dataspace);
  H5Dclose(dataset);

  delete[] dims;
  delete[] maxdims;
}

static void read_dataset (const v8::FunctionCallbackInfo<Value>& args)
{
    // fail out if arguments are not correct
    if(args.Length() ==3 && (!args[0]->IsObject() || !args[1]->IsString() || !args[2]->IsFunction())){
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, callback")));
        args.GetReturnValue().SetUndefined();
        return;

    }
    else if (args.Length() ==2 && (!args[0]->IsObject() || !args[1]->IsString())) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name")));
        args.GetReturnValue().SetUndefined();
        return;
    }

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    herr_t err=H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
    err=H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
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
              theSize=values_dim.get()[0]*values_dim.get()[1]*values_dim.get()[2];
              break;
              case 2:
              theSize=values_dim.get()[0]*values_dim.get()[1];
              break;
              case 1:
                  theSize=values_dim.get()[0];
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
        switch(class_id)
        {
            case H5T_ARRAY:
            {
                hid_t did=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
                hid_t t=H5Dget_type(did);
                hid_t type_id=H5Tget_native_type(t,H5T_DIR_ASCEND);
                hid_t dataspace_id=H5S_ALL;
                hid_t memspace_id=H5S_ALL;
                hid_t basetype_id=H5Tget_super(type_id);
                int arrayRank=H5Tget_array_ndims(type_id);
                std::unique_ptr<hsize_t[]> arrayDims(new hsize_t[arrayRank]);
                /*int arrayLength=*/H5Tget_array_dims(type_id, arrayDims.get());
                std::unique_ptr<char*[]> vl(new char*[arrayDims.get()[0]]);
                if(!H5Tis_variable_str(basetype_id)){
                    //size_t nalloc;
                    //H5Tencode(type_id, NULL, &nalloc);
                    //H5Tencode(type_id, vl.get(), &nalloc);
                    size_t typeSize=H5Tget_size(basetype_id);
                    for(unsigned int arrayIndex=0;arrayIndex<arrayDims.get()[0];arrayIndex++){
                        vl.get()[arrayIndex]=new char[typeSize+1];
                        std::memset(vl.get()[arrayIndex], 0, typeSize+1);
                    }
                }
                err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, vl.get());
                if(err<0)
                {
                        //H5Sclose (memspace_id);
                        //H5Sclose (dataspace_id);
                    H5Tclose(t);
                    H5Tclose(type_id);
                    H5Tclose(basetype_id);
                    H5Dclose(did);
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
                Local<Array> array=Array::New(v8::Isolate::GetCurrent(), arrayDims.get()[0]);
                for(unsigned int arrayIndex=0;arrayIndex<arrayDims.get()[0];arrayIndex++){
                    std::string s(vl.get()[arrayIndex]);
                    array->Set(arrayIndex, String::NewFromUtf8(v8::Isolate::GetCurrent(), vl.get()[arrayIndex],String::kNormalString, std::strlen(vl.get()[arrayIndex])));
                }
                args.GetReturnValue().Set(array);
                //H5Sclose (memspace_id);
                //H5Sclose (dataspace_id);
                H5Tclose(t);
                H5Tclose(type_id);
                H5Tclose(basetype_id);
                H5Dclose(did);
            }
            break;
            case H5T_STRING:
            {
                hid_t did=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
                hid_t t=H5Dget_type(did);
                hid_t type_id=H5Tget_native_type(t,H5T_DIR_ASCEND);
                if(H5Tis_variable_str(type_id)){
                    hid_t dataspace_id=H5S_ALL;
                    hid_t memspace_id=H5S_ALL;
                    hid_t basetype_id=H5Tget_super(type_id);
                    std::unique_ptr<char*[]> tbuffer(new char*[values_dim.get()[0]]);
                    H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, tbuffer.get());
                    Local<Array> array=Array::New(v8::Isolate::GetCurrent(), values_dim.get()[0]);
                    for(unsigned int arrayIndex=0;arrayIndex<values_dim.get()[0];arrayIndex++){
                        std::string s(tbuffer.get()[arrayIndex]);
                        array->Set(arrayIndex, String::NewFromUtf8(v8::Isolate::GetCurrent(), tbuffer.get()[arrayIndex],String::kNormalString, std::strlen(tbuffer.get()[arrayIndex])));
                    }
                    args.GetReturnValue().Set(array);
                        H5Tclose(basetype_id);
                    }
                    else if(rank==1 && values_dim.get()[0]>0){
                        hid_t dataspace_id=H5S_ALL;
                        hid_t memspace_id=H5S_ALL;
                        size_t typeSize=H5Tget_size(t);
                        std::unique_ptr<char[]> tbuffer(new char[typeSize*values_dim.get()[0]]);
                        size_t nalloc;
                        H5Tencode(type_id, NULL, &nalloc);
                        H5Tencode(type_id, tbuffer.get(), &nalloc);
                        err = H5Dread(did, type_id, memspace_id, dataspace_id, H5P_DEFAULT, tbuffer.get());
                        if(err<0)
                        {
                                //H5Sclose (memspace_id);
                                //H5Sclose (dataspace_id);
                            H5Tclose(t);
                            H5Tclose(type_id);
                            H5Dclose(did);
                            v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read array dataset")));
                            args.GetReturnValue().SetUndefined();
                            return;
                        }
                        Local<Array> array=Array::New(v8::Isolate::GetCurrent(), values_dim.get()[0]);
                        for(unsigned int arrayIndex=0;arrayIndex<values_dim.get()[0];arrayIndex++){
                            std::string s(&tbuffer.get()[typeSize*arrayIndex]);
                            array->Set(arrayIndex, String::NewFromUtf8(v8::Isolate::GetCurrent(), &tbuffer.get()[typeSize*arrayIndex],String::kNormalString, std::min(typeSize, (size_t)std::strlen(&tbuffer.get()[typeSize*arrayIndex]))));
                        }
                        args.GetReturnValue().Set(array);
                    }
                else{
                    std::string buffer(theSize+1, 0);
                    err=H5LTread_dataset_string (idWrap->Value(), *dset_name,  (char*)buffer.c_str());
                    if(err<0)
                    {
                        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
                        args.GetReturnValue().SetUndefined();
                        return;
                    }

                    args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
                }
                H5Tclose(t);
                H5Tclose(type_id);
                H5Dclose(did);
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
                else if(class_id==H5T_INTEGER && bufSize==8)
                {
                    hid_t did=H5Dopen(args[0]->ToInt32()->Value(), *dset_name, H5P_DEFAULT );
                    type_id=H5Dget_type(did);
                    hid_t native_type_id=H5Tget_native_type(type_id,H5T_DIR_ASCEND);
                    Handle<Object> int64Buffer = node::Buffer::New(v8::Isolate::GetCurrent(), bufSize * theSize).ToLocalChecked();
                    H5LTread_dataset (args[0]->ToInt32()->Value(), *dset_name, native_type_id, (char*)node::Buffer::Data(int64Buffer) );

                    args.GetReturnValue().Set(int64Buffer);
                    H5Tclose(type_id);
                    H5Dclose(did);
                    return;

                }
                else if(class_id==H5T_INTEGER && bufSize==4)
                {
                    hid_t h=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
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
                    hid_t h=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
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
                    hid_t h=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
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

                err=H5LTread_dataset (idWrap->Value(), *dset_name, type_id, buffer->Buffer()->Externalize().Data() );
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
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                        break;
                    case 2:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                        if(rank>1)buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                        break;
                    case 1:
                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                    break;
                }
                if(args.Length() ==3){
                    v8::Persistent<v8::Function> callback;
                    const unsigned argc = 2;
                    callback.Reset(v8::Isolate::GetCurrent(), args[2].As<Function>());
                    v8::Local<v8::Object> options=v8::Object::New(v8::Isolate::GetCurrent());
                    options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rank"), Number::New(v8::Isolate::GetCurrent(), rank));
                    H5T_order_t order=H5Tget_order( type_id );
                    options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "endian"), Number::New(v8::Isolate::GetCurrent(), order));
                    //options->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "interlace"), String::NewFromUtf8(v8::Isolate::GetCurrent(), interlace));

                    v8::Local<v8::Value> argv[1] = { options };
                    v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), callback)->Call(v8::Isolate::GetCurrent()->GetCurrentContext()->Global(), argc, argv);
                }

        //Attributes
        uint32_t index=0;
        hsize_t idx=0;
        std::vector<std::string> holder;
//        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//        }, &index, &holder);
        H5Aiterate_by_name(idWrap->Value(), *dset_name, H5_INDEX_CRT_ORDER ,
        H5_ITER_INC , &idx, [] (hid_t location_id, const char* attr_name, const H5A_info_t *ainfo, void *operator_data){
                if(ainfo->data_size>0)((std::vector<std::string>*)operator_data)->push_back(std::string(attr_name));
                return (herr_t)((std::vector<std::string>*)operator_data)->size();
            }, (void*)&holder, H5P_DEFAULT);
        for(index=0;index<(uint32_t)holder.size();index++)
        {
            hsize_t values_dim[1] = {1};
            size_t bufSize = 0;
            H5T_class_t class_id;
            err = H5LTget_attribute_info(idWrap->Value(), *dset_name, holder[index].c_str(), values_dim, &class_id, &bufSize);
            if (err >=0) {
                switch(class_id)
                {
                    case H5T_INTEGER:
                        long long intValue;
                         H5LTget_attribute_int(idWrap->Value(), *dset_name, holder[index].c_str(), (int*)&intValue);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                        break;
                    case H5T_FLOAT:
                        double value;
                         H5LTget_attribute_double(idWrap->Value(), *dset_name, holder[index].c_str(), &value);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                        break;
                    case H5T_STRING:
                    {
                        std::string strValue(bufSize+1,'\0');
                         H5LTget_attribute_string(idWrap->Value(), *dset_name, holder[index].c_str(), (char *)strValue.c_str());
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
    if (args.Length() < 2 || args.Length() > 3 || !args[0]->IsObject() || !args[1]->IsString()) {

        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "expected id, name, buffer, [options]")));
        args.GetReturnValue().SetUndefined();
        return;

    }

    String::Utf8Value dset_name (args[1]->ToString());
    size_t bufSize = 0;
    H5T_class_t class_id;
    int rank;
    Int64* idWrap = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    herr_t err=H5LTget_dataset_ndims(idWrap->Value(), *dset_name, &rank);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset rank")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    std::unique_ptr<hsize_t[]> values_dim(new hsize_t[rank]);
    err=H5LTget_dataset_info(idWrap->Value(), *dset_name, values_dim.get(), &class_id, &bufSize);
    if(err<0)
    {
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to find dataset info")));
        args.GetReturnValue().SetUndefined();
        return;
    }
    hsize_t theSize=bufSize;
    bool subsetOn=false;
    std::unique_ptr<hsize_t[]> start(new hsize_t[rank]);
    std::unique_ptr<hsize_t[]> stride(new hsize_t[rank]);
    std::unique_ptr<hsize_t[]> count(new hsize_t[rank]);
    if(args.Length() == 3){
         Local<Array> names=args[2]->ToObject()->GetOwnPropertyNames();
         for(uint32_t index=0;index<names->Length();index++){
            String::Utf8Value _name (names->Get(index));
            std::string name(*_name);
            if(name.compare("start")==0){
                Local<Object> starts=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<starts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    start.get()[arrayIndex]=starts->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("stride")==0){
                Local<Object> strides=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<strides->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    stride.get()[arrayIndex]=strides->Get(arrayIndex)->Uint32Value();
                }
            }
            else if(name.compare("count")==0){
                 Local<Object> counts=args[2]->ToObject()->Get(names->Get(index))->ToObject();
                for(unsigned int arrayIndex=0;arrayIndex<counts->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "length"))->ToObject()->Uint32Value();arrayIndex++){
                    count.get()[arrayIndex]=counts->Get(arrayIndex)->Uint32Value();
                }
                subsetOn=true;
            }
         }
    }
          switch(rank)
          {
              case 3:
              theSize=values_dim.get()[0]*values_dim.get()[1]*values_dim.get()[2];
              break;
              case 2:
              theSize=values_dim.get()[0]*values_dim.get()[1];
              break;
              case 1:
                  theSize=values_dim.get()[0];
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
        switch(class_id)
        {
            case H5T_STRING:
            {
                std::string buffer(theSize+1, 0);
                err=H5LTread_dataset_string (idWrap->Value(), *dset_name,  (char*)buffer.c_str());
                if(err<0)
                {
                    v8::Isolate::GetCurrent()->ThrowException(v8::Exception::SyntaxError(String::NewFromUtf8(v8::Isolate::GetCurrent(), "failed to read dataset into string")));
                    args.GetReturnValue().SetUndefined();
                    return;
                }
//                args.GetReturnValue().Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), buffer.c_str(), String::kNormalString, theSize));
                args.GetReturnValue().Set(node::Buffer::New(v8::Isolate::GetCurrent(), (char*)buffer.c_str(), theSize).ToLocalChecked());
            }
                break;
            case H5T_INTEGER:
            case H5T_FLOAT:
            case H5T_BITFIELD:
            {

                hid_t did=H5Dopen(idWrap->Value(), *dset_name, H5P_DEFAULT );
                hid_t t=H5Dget_type(did);
                //hid_t type_id=H5Tget_native_type(t,H5T_DIR_ASCEND);
                hid_t dataspace_id=H5S_ALL;
                hid_t memspace_id=H5S_ALL;
                if(subsetOn){
                    const hsize_t maxsize = H5S_UNLIMITED;
                    memspace_id = H5Screate_simple (rank, count.get(), &maxsize);
                    dataspace_id = H5Dget_space (did);
                    herr_t  err = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, start.get(),
                                                  stride.get(), count.get(), NULL);
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
                    for(int rankIndex=0;rankIndex<rank;rankIndex++){
                        theSize*=count.get()[rankIndex];
                    }
                }
                v8::Local<v8::Object> buffer=node::Buffer::New(v8::Isolate::GetCurrent(),bufSize*theSize).ToLocalChecked();
                err = H5Dread(did, t, memspace_id, dataspace_id, H5P_DEFAULT, (char*)node::Buffer::Data(buffer));
                //err=H5LTread_dataset (idWrap->Value(), *dset_name, type_id, (char*)node::Buffer::Data(buffer) );
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
                bool hit=false;
                H5T etype=NODE_H5T_UNKNOWN;
                for(std::map<H5T, hid_t>::iterator it=toTypeMap.begin();!hit && it!=toTypeMap.end();it++){

                    if(H5Tequal(t, toTypeMap[(*it).first])){
                        etype=(*it).first;
                        hit=true;
                    }
                }

                buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "type"), Int32::New(v8::Isolate::GetCurrent(), etype));
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
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[2]));
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "sections"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                        break;
                    case 2:
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                        if(rank>1)buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "columns"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[1]));
                        break;
                    case 1:
                    buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), "rows"), Number::New(v8::Isolate::GetCurrent(), values_dim.get()[0]));
                    break;
                }

        //Attributes
        uint32_t index=0;
        hsize_t idx=0;
        std::vector<std::string> holder;
//        group->m_group.iterateAttrs([&](H5::H5Location &loc, H5std_string attr_name, void *operator_data){
//            ((std::vector<std::string>*)operator_data)->push_back(attr_name);
//        }, &index, &holder);
        H5Aiterate_by_name(idWrap->Value(), *dset_name, H5_INDEX_CRT_ORDER ,
        H5_ITER_INC , &idx, [] (hid_t location_id, const char* attr_name, const H5A_info_t *ainfo, void *operator_data){
                if(ainfo->data_size>0)((std::vector<std::string>*)operator_data)->push_back(std::string(attr_name));
                return (herr_t)((std::vector<std::string>*)operator_data)->size();
            }, (void*)&holder, H5P_DEFAULT);
        for(index=0;index<(uint32_t)holder.size();index++)
        {
            hsize_t values_dim[1] = {1};
            size_t bufSize = 0;
            H5T_class_t class_id;
            err = H5LTget_attribute_info(idWrap->Value(), *dset_name, holder[index].c_str(), values_dim, &class_id, &bufSize);
            if (err >=0) {
                switch(class_id)
                {
                    case H5T_INTEGER:
                        long long intValue;
                         H5LTget_attribute_int(idWrap->Value(), *dset_name, holder[index].c_str(), (int*)&intValue);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Int32::New(v8::Isolate::GetCurrent(), intValue));
                        break;
                    case H5T_FLOAT:
                        double value;
                         H5LTget_attribute_double(idWrap->Value(), *dset_name, holder[index].c_str(), &value);
                        buffer->Set(String::NewFromUtf8(v8::Isolate::GetCurrent(), holder[index].c_str()), Number::New(v8::Isolate::GetCurrent(), value));
                        break;
                    case H5T_STRING:
                    {
                        std::string strValue(bufSize+1,'\0');
                         H5LTget_attribute_string(idWrap->Value(), *dset_name, holder[index].c_str(), (char *)strValue.c_str());
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
