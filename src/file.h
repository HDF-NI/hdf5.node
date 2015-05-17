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

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    
enum HLType {
    HL_TYPE_UNKNOWN = -1,      /* Unknown object type                      */
    HL_TYPE_LITE = 0,             /* Lite dataset interface                        */
    HL_TYPE_IMAGE = 1,           /* Image                      */
    HL_TYPE_TABLE = 2,    /* Table   */
    HL_TYPE_PACKET_TABLE = 3,             /* Packets       */
    HL_TYPE_TEXT = 4,
    HL_TYPE_DIMENSION_SCALES = 5,             /*        */
    HL_TYPE_OPTIMIZED_FUNCTIONS = 6,             /*        */
    HL_TYPE_EXTENSIONS = 7             /*        */
};

enum H5T {
     NODE_H5T_STD_I8BE = 258,
     NODE_H5T_STD_I8LE = 259,
     NODE_H5T_STD_I16BE = 260,
     NODE_H5T_STD_I16LE = 261,
     NODE_H5T_STD_I32BE = 262,
     NODE_H5T_STD_I32LE = 263,
     NODE_H5T_STD_I64BE = 264,
     NODE_H5T_STD_I64LE = 265,
     NODE_H5T_STD_U8BE = 266,
     NODE_H5T_STD_U8LE = 267,
     NODE_H5T_STD_U16BE = 268,
     NODE_H5T_STD_U16LE = 269,
     NODE_H5T_STD_U32BE = 270,
     NODE_H5T_STD_U32LE = 271,
     NODE_H5T_STD_U64BE = 272,
     NODE_H5T_STD_U64LE = 273,
     NODE_H5T_NATIVE_CHAR = 274,
     NODE_H5T_NATIVE_SCHAR = 275,
     NODE_H5T_NATIVE_UCHAR = 276,
     NODE_H5T_NATIVE_SHORT = 277,
     NODE_H5T_NATIVE_USHORT = 278,
     NODE_H5T_NATIVE_INT = 279,
     NODE_H5T_NATIVE_UINT = 280,
     NODE_H5T_NATIVE_LONG = 281,
     NODE_H5T_NATIVE_ULONG = 282,
     NODE_H5T_NATIVE_LLONG = 283,
     NODE_H5T_NATIVE_ULLONG = 284,
     NODE_H5T_IEEE_F32BE = 285,
     NODE_H5T_IEEE_F32LE = 286,
     NODE_H5T_IEEE_F64BE = 287,
     NODE_H5T_IEEE_F64LE = 288,
     NODE_H5T_NATIVE_FLOAT = 289,
     NODE_H5T_NATIVE_DOUBLE = 290,
     NODE_H5T_NATIVE_LDOUBLE = 291,
     NODE_H5T_STRING = 292,
     NODE_STRSIZE = 293,
     NODE_STRPAD = 294,
     NODE_CSET = 295,
     NODE_CTYPE = 296,
     NODE_H5T_VARIABLE = 297,
     NODE_H5T_STR_NULLTERM = 298,
     NODE_H5T_STR_NULLPAD = 299,
     NODE_H5T_STR_SPACEPAD = 300,
     NODE_H5T_CSET_ASCII = 301,
     NODE_H5T_CSET_UTF8 = 302,
     NODE_H5T_C_S1 = 303,
     NODE_H5T_FORTRAN_S1 = 304,
     NODE_H5T_OPAQUE = 305,
     NODE_OPQ_SIZE = 306,
     NODE_OPQ_TAG = 307,
     NODE_H5T_COMPOUND = 308,
     NODE_H5T_ENUM = 309,
     NODE_H5T_ARRAY = 310,
     NODE_H5T_VLEN = 311,
     NODE_STRING = 312,
     NODE_NUMBER = 313,
     NODE_H5T_NATIVE_INT8 = 314,
     NODE_H5T_NATIVE_UINT8 = 315
};

    class File : public Attributes {
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
//            static void Refresh (const v8::FunctionCallbackInfo<Value>& args);
//            static void Flush (const v8::FunctionCallbackInfo<Value>& args);
            static void Move (const v8::FunctionCallbackInfo<Value>& args);
            static void Delete (const v8::FunctionCallbackInfo<Value>& args);
            static void Close (const v8::FunctionCallbackInfo<Value>& args);
            static void GetNumAttrs (const v8::FunctionCallbackInfo<Value>& args);
            static void GetMemberNamesByCreationOrder (const v8::FunctionCallbackInfo<Value>& args);
            static void GetChildType (const v8::FunctionCallbackInfo<Value>& args);
        
        protected:
            hsize_t getNumObjs();
            int getNumAttrs();
            H5O_type_t childObjType(const char* objname);
            std::string getObjnameByIdx(hsize_t idx);
    };
    

};