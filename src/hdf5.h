#pragma once
#include <map>

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>

#include "H5Cpp.h"

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    
    class File : public ObjectWrap {
    protected:
            std::map<unsigned long, unsigned long> toAccessMap = {{0,H5F_ACC_RDONLY}, {1,H5F_ACC_RDWR}, {2,H5F_ACC_TRUNC}, {3,H5F_ACC_EXCL}, {4,H5F_ACC_DEBUG}, {5,H5F_ACC_CREAT}};

        public:
            static void Initialize (Handle<Object> target);
            H5::H5File* FileObject();
            
        private:
            H5::H5File* m_file;
            File(const char* path);
            File(const char* path, unsigned long access);
            ~File();
            static Persistent<FunctionTemplate> Constructor;
            static void New (const v8::FunctionCallbackInfo<Value>& args);
            static void CreateGroup (const v8::FunctionCallbackInfo<Value>& args);
            static void OpenGroup (const v8::FunctionCallbackInfo<Value>& args);
        
    };
    
    class Group : public ObjectWrap {
        
        private:
            H5::Group m_group;
            static Persistent<FunctionTemplate> Constructor;
            static void New (const v8::FunctionCallbackInfo<Value>& args);
            
        public:
            Group(H5::Group group);
            static void Initialize ();
            static Local<Object> Instantiate (Local<Object> file);
            static Local<Object> Instantiate (const char* path, Local<Object> file);
            static void Create (const v8::FunctionCallbackInfo<Value>& args);
            static void Open (const v8::FunctionCallbackInfo<Value>& args);
        
    };

};