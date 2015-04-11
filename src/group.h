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

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    

    class Group : public ObjectWrap {
        
        private:
            std::string name;
            hid_t id;
            hid_t gcpl_id;
            static Persistent<FunctionTemplate> Constructor;
            static void New (const v8::FunctionCallbackInfo<Value>& args);
            
        public:
            Group(hid_t id);
            static void Initialize ();
            static Local<Object> Instantiate (Local<Object> file);
            static Local<Object> Instantiate (const char* path, Local<Object> file, unsigned long creationOrder = 0);
            static void Create (const v8::FunctionCallbackInfo<Value>& args);
            static void Open (const v8::FunctionCallbackInfo<Value>& args);
            static void OpenGroup (const v8::FunctionCallbackInfo<Value>& args);
            static void Refresh (const v8::FunctionCallbackInfo<Value>& args);
            static void Flush (const v8::FunctionCallbackInfo<Value>& args);
            static void Close (const v8::FunctionCallbackInfo<Value>& args);
            static void GetNumAttrs (const v8::FunctionCallbackInfo<Value>& args);
            static void GetAttributeNames (const v8::FunctionCallbackInfo<Value>& args);
            static void ReadAttribute (const v8::FunctionCallbackInfo<Value>& args);
            static void GetNumObjs (const v8::FunctionCallbackInfo<Value>& args);
            static void GetMemberNames (const v8::FunctionCallbackInfo<Value>& args);
            static void GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args);
            static void GetChildType (const v8::FunctionCallbackInfo<Value>& args);
            static void getDatasetType (const v8::FunctionCallbackInfo<Value>& args);
            
        protected:
            hsize_t getNumObjs();
            int getNumAttrs();
            H5O_type_t childObjType(const char* objname);
            std::string getObjnameByIdx(hsize_t idx);
            
    };

};