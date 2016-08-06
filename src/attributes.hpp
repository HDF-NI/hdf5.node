#pragma once

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include "H5Tpublic.h"

namespace NodeHDF5{
    class Attributes : public node::ObjectWrap {
    protected:
            std::string name;
            hid_t id;
            hid_t gcpl_id=0;
    public:
        Attributes() {};
        Attributes(hid_t id) : id (id) {};
        Attributes(const Attributes& orig) = delete;
        virtual ~Attributes(){};
        
        static void make_attribute_from_typed_array(const int32_t &group_id, const char *attribute_name, v8::Handle<v8::TypedArray> buffer, hid_t type_id);
        static void make_attribute_from_array(const int32_t &group_id, const char *attribute_name, v8::Handle<v8::Array> array);
        static void Refresh (const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Flush (const v8::FunctionCallbackInfo<v8::Value>& args);

    protected:
        virtual int getNumAttrs() = 0;
    };
}
