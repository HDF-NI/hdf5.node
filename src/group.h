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
    

    class Group : public Methods {
        friend class File;
        using Attributes::name;
        using Attributes::id;
        using Attributes::gcpl_id;
        using Attributes::Refresh;
        using Attributes::Flush;
    protected:
        std::vector<hid_t> hidPath;

        private:
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
            static void Copy (const v8::FunctionCallbackInfo<Value>& args);
            static void Move (const v8::FunctionCallbackInfo<Value>& args);
            static void Link (const v8::FunctionCallbackInfo<Value>& args);
            static void Delete (const v8::FunctionCallbackInfo<Value>& args);
            static void Close (const v8::FunctionCallbackInfo<Value>& args);
            
        protected:

    };

};