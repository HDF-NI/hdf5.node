#pragma once
#include <map>

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <memory>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "attributes.hpp"
#include "methods.hpp"

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;

    class File : public Methods {
        using Attributes::name;
        using Attributes::id;
        using Attributes::gcpl_id;
        using Attributes::Refresh;
        using Attributes::Flush;
    protected:
            //std::map<unsigned long, unsigned long> toAccessMap = {{0,H5F_ACC_RDONLY}, {1,H5F_ACC_RDWR}, {2,H5F_ACC_TRUNC}, {3,H5F_ACC_EXCL}, {4,H5F_ACC_DEBUG}, {5,H5F_ACC_CREAT}};
            hid_t plist_id, gcpl, dtpl_id, dapl_id, dcpl_id;

            unsigned int compression = 0;
            bool error=false;
            bool is_open = false;

        public:
            static void Initialize (Handle<Object> target);
//            H5::H5File* FileObject();
            hid_t getId(){return id;};
            hid_t getGcpl(){return gcpl;};
            std::string getFileName(){return name;};

        private:
//            H5::H5File* m_file;
            File(const char* path);
            File(const char* path, unsigned int access);
            ~File();
            static Persistent<FunctionTemplate> Constructor;
            static void New (const v8::FunctionCallbackInfo<Value>& args);
            static void CreateGroup (const v8::FunctionCallbackInfo<Value>& args);
            static void OpenGroup (const v8::FunctionCallbackInfo<Value>& args);
            static void Move (const v8::FunctionCallbackInfo<Value>& args);
            static void Delete (const v8::FunctionCallbackInfo<Value>& args);
            static void Close (const v8::FunctionCallbackInfo<Value>& args);
            static void GetNumAttrs (const v8::FunctionCallbackInfo<Value>& args);
            static void GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args);
            static void GetChildType (const v8::FunctionCallbackInfo<Value>& args);

        protected:
    };


};
