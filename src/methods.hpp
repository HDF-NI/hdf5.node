#pragma once

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <memory>
#include "H5Tpublic.h"

namespace NodeHDF5 {

  enum HLType {
    HL_TYPE_UNKNOWN             = -1, /* Unknown object type                      */
    HL_TYPE_LITE                = 0,  /* Lite dataset interface                        */
    HL_TYPE_IMAGE               = 1,  /* Image                      */
    HL_TYPE_TABLE               = 2,  /* Table   */
    HL_TYPE_PACKET_TABLE        = 3,  /* Packets       */
    HL_TYPE_TEXT                = 4,
    HL_TYPE_DIMENSION_SCALES    = 5, /*        */
    HL_TYPE_OPTIMIZED_FUNCTIONS = 6, /*        */
    HL_TYPE_EXTENSIONS          = 7  /*        */
  };

  enum H5T {
    JS_H5T_UNKNOWN        = 0,
    JS_H5T_STD_I8BE       = 258,
    JS_H5T_STD_I8LE       = 259,
    JS_H5T_STD_I16BE      = 260,
    JS_H5T_STD_I16LE      = 261,
    JS_H5T_STD_I32BE      = 262,
    JS_H5T_STD_I32LE      = 263,
    JS_H5T_STD_I64BE      = 264,
    JS_H5T_STD_I64LE      = 265,
    JS_H5T_STD_U8BE       = 266,
    JS_H5T_STD_U8LE       = 267,
    JS_H5T_STD_U16BE      = 268,
    JS_H5T_STD_U16LE      = 269,
    JS_H5T_STD_U32BE      = 270,
    JS_H5T_STD_U32LE      = 271,
    JS_H5T_STD_U64BE      = 272,
    JS_H5T_STD_U64LE      = 273,
    JS_H5T_NATIVE_CHAR    = 274,
    JS_H5T_NATIVE_SCHAR   = 275,
    JS_H5T_NATIVE_UCHAR   = 276,
    JS_H5T_NATIVE_SHORT   = 277,
    JS_H5T_NATIVE_USHORT  = 278,
    JS_H5T_NATIVE_INT     = 279,
    JS_H5T_NATIVE_UINT    = 280,
    JS_H5T_NATIVE_LONG    = 281,
    JS_H5T_NATIVE_ULONG   = 282,
    JS_H5T_NATIVE_LLONG   = 283,
    JS_H5T_NATIVE_ULLONG  = 284,
    JS_H5T_IEEE_F32BE     = 285,
    JS_H5T_IEEE_F32LE     = 286,
    JS_H5T_IEEE_F64BE     = 287,
    JS_H5T_IEEE_F64LE     = 288,
    JS_H5T_NATIVE_FLOAT   = 289,
    JS_H5T_NATIVE_DOUBLE  = 290,
    JS_H5T_NATIVE_LDOUBLE = 291,
    JS_H5T_STRING         = 292,
    NODE_STRSIZE            = 293,
    NODE_STRPAD             = 294,
    NODE_CSET               = 295,
    NODE_CTYPE              = 296,
    JS_H5T_VARIABLE       = 297,
    JS_H5T_STR_NULLTERM   = 298,
    JS_H5T_STR_NULLPAD    = 299,
    JS_H5T_STR_SPACEPAD   = 300,
    JS_H5T_CSET_ASCII     = 301,
    JS_H5T_CSET_UTF8      = 302,
    JS_H5T_C_S1           = 303,
    JS_H5T_FORTRAN_S1     = 304,
    JS_H5T_OPAQUE         = 305,
    NODE_OPQ_SIZE           = 306,
    NODE_OPQ_TAG            = 307,
    JS_H5T_COMPOUND       = 308,
    JS_H5T_ENUM           = 309,
    JS_H5T_ARRAY          = 310,
    JS_H5T_VLEN           = 311,
    NODE_STRING             = 312,
    NODE_NUMBER             = 313,
    JS_H5T_NATIVE_INT8    = 314,
    JS_H5T_NATIVE_UINT8   = 315
  };

  static std::map<H5T, hid_t> toTypeMap{{JS_H5T_STD_I8BE, H5T_STD_I8BE},
                                        {JS_H5T_STD_I8LE, H5T_STD_I8LE},
                                        {JS_H5T_STD_I16BE, H5T_STD_I16BE},
                                        {JS_H5T_STD_I16LE, H5T_STD_I16LE},
                                        {JS_H5T_STD_I32BE, H5T_STD_I32BE},
                                        {JS_H5T_STD_I32LE, H5T_STD_I32LE},
                                        {JS_H5T_STD_I64BE, H5T_STD_I64BE},
                                        {JS_H5T_STD_I64LE, H5T_STD_I64LE},
                                        {JS_H5T_STD_U8BE, H5T_STD_U8BE},
                                        {JS_H5T_STD_U8LE, H5T_STD_U8LE},
                                        {JS_H5T_STD_U16BE, H5T_STD_U16BE},
                                        {JS_H5T_STD_U16LE, H5T_STD_U16LE},
                                        {JS_H5T_STD_U32BE, H5T_STD_U32BE},
                                        {JS_H5T_STD_U32LE, H5T_STD_U32LE},
                                        {JS_H5T_STD_U64BE, H5T_STD_U64BE},
                                        {JS_H5T_STD_U64LE, H5T_STD_U64LE},
                                        {JS_H5T_NATIVE_CHAR, H5T_NATIVE_CHAR},
                                        {JS_H5T_NATIVE_SCHAR, H5T_NATIVE_SCHAR},
                                        {JS_H5T_NATIVE_UCHAR, H5T_NATIVE_UCHAR},
                                        {JS_H5T_NATIVE_SHORT, H5T_NATIVE_SHORT},
                                        {JS_H5T_NATIVE_USHORT, H5T_NATIVE_USHORT},
                                        {JS_H5T_NATIVE_INT, H5T_NATIVE_INT},
                                        {JS_H5T_NATIVE_UINT, H5T_NATIVE_UINT},
                                        {JS_H5T_NATIVE_LONG, H5T_NATIVE_LONG},
                                        {JS_H5T_NATIVE_ULONG, H5T_NATIVE_ULONG},
                                        {JS_H5T_NATIVE_LLONG, H5T_NATIVE_LLONG},
                                        {JS_H5T_NATIVE_ULLONG, H5T_NATIVE_ULLONG},
                                        {JS_H5T_IEEE_F32BE, H5T_IEEE_F32BE},
                                        {JS_H5T_IEEE_F32LE, H5T_IEEE_F32LE},
                                        {JS_H5T_IEEE_F64BE, H5T_IEEE_F64BE},
                                        {JS_H5T_IEEE_F64LE, H5T_IEEE_F64LE},
                                        {JS_H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT},
                                        {JS_H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE},
                                        {JS_H5T_NATIVE_LDOUBLE, H5T_NATIVE_LDOUBLE},
                                        {JS_H5T_STRING, H5T_STRING},
                                        //{NODE_STRSIZE,STRSIZE},
                                        //{NODE_STRPAD,STRPAD},
                                        //{NODE_CSET,CSET},
                                        //{NODE_CTYPE,CTYPE},
                                        {JS_H5T_VARIABLE, H5T_VARIABLE},
                                        {JS_H5T_STR_NULLTERM, H5T_STR_NULLTERM},
                                        {JS_H5T_STR_NULLPAD, H5T_STR_NULLPAD},
                                        {JS_H5T_STR_SPACEPAD, H5T_STR_SPACEPAD},
                                        {JS_H5T_CSET_ASCII, H5T_CSET_ASCII},
                                        {JS_H5T_CSET_UTF8, H5T_CSET_UTF8},
                                        {JS_H5T_C_S1, H5T_C_S1},
                                        {JS_H5T_FORTRAN_S1, H5T_FORTRAN_S1},
                                        {JS_H5T_OPAQUE, H5T_OPAQUE},
                                        //{NODE_OPQ_SIZE,OPQ_SIZE},
                                        //{NODE_OPQ_TAG,OPQ_TAG},
                                        {JS_H5T_COMPOUND, H5T_COMPOUND},
                                        {JS_H5T_ENUM, H5T_ENUM},
                                        {JS_H5T_ARRAY, H5T_ARRAY},
                                        {JS_H5T_VLEN, H5T_VLEN},
                                        //{NODE_STRING,STRING},
                                        //{NODE_NUMBER,NUMBER},
                                        {JS_H5T_NATIVE_INT8, H5T_NATIVE_INT8},
                                        {JS_H5T_NATIVE_UINT8, H5T_NATIVE_UINT8}};

  static std::map<hid_t, H5T> toEnumMap{{H5T_STD_I8BE, JS_H5T_STD_I8BE},
                                        {H5T_STD_I8LE, JS_H5T_STD_I8LE},
                                        {H5T_STD_I16BE, JS_H5T_STD_I16BE},
                                        {H5T_STD_I16LE, JS_H5T_STD_I16LE},
                                        {H5T_STD_I32BE, JS_H5T_STD_I32BE},
                                        {H5T_STD_I32LE, JS_H5T_STD_I32LE},
                                        {H5T_STD_I64BE, JS_H5T_STD_I64BE},
                                        {H5T_STD_I64LE, JS_H5T_STD_I64LE},
                                        {H5T_STD_U8BE, JS_H5T_STD_U8BE},
                                        {H5T_STD_U8LE, JS_H5T_STD_U8LE},
                                        {H5T_STD_U16BE, JS_H5T_STD_U16BE},
                                        {H5T_STD_U16LE, JS_H5T_STD_U16LE},
                                        {H5T_STD_U32BE, JS_H5T_STD_U32BE},
                                        {H5T_STD_U32LE, JS_H5T_STD_U32LE},
                                        {H5T_STD_U64BE, JS_H5T_STD_U64BE},
                                        {H5T_STD_U64LE, JS_H5T_STD_U64LE},
                                        {H5T_NATIVE_CHAR, JS_H5T_NATIVE_CHAR},
                                        {H5T_NATIVE_SCHAR, JS_H5T_NATIVE_SCHAR},
                                        {H5T_NATIVE_UCHAR, JS_H5T_NATIVE_UCHAR},
                                        {H5T_NATIVE_SHORT, JS_H5T_NATIVE_SHORT},
                                        {H5T_NATIVE_USHORT, JS_H5T_NATIVE_USHORT},
                                        {H5T_NATIVE_INT, JS_H5T_NATIVE_INT},
                                        {H5T_NATIVE_UINT, JS_H5T_NATIVE_UINT},
                                        {H5T_NATIVE_LONG, JS_H5T_NATIVE_LONG},
                                        {H5T_NATIVE_ULONG, JS_H5T_NATIVE_ULONG},
                                        {H5T_NATIVE_LLONG, JS_H5T_NATIVE_LLONG},
                                        {H5T_NATIVE_ULLONG, JS_H5T_NATIVE_ULLONG},
                                        {H5T_IEEE_F32BE, JS_H5T_IEEE_F32BE},
                                        {H5T_IEEE_F32LE, JS_H5T_IEEE_F32LE},
                                        {H5T_IEEE_F64BE, JS_H5T_IEEE_F64BE},
                                        {H5T_IEEE_F64LE, JS_H5T_IEEE_F64LE},
                                        {H5T_NATIVE_FLOAT, JS_H5T_NATIVE_FLOAT},
                                        {H5T_NATIVE_DOUBLE, JS_H5T_NATIVE_DOUBLE},
                                        {H5T_NATIVE_LDOUBLE, JS_H5T_NATIVE_LDOUBLE},
                                        {H5T_STRING, JS_H5T_STRING},
                                        //{STRSIZE,NODE_STRSIZE},
                                        //{STRPAD,NODE_STRPAD},
                                        //{CSET,NODE_CSET},
                                        //{CTYPE,NODE_CTYPE},
                                        {H5T_VARIABLE, JS_H5T_VARIABLE},
                                        {H5T_STR_NULLTERM, JS_H5T_STR_NULLTERM},
                                        {H5T_STR_NULLPAD, JS_H5T_STR_NULLPAD},
                                        {H5T_STR_SPACEPAD, JS_H5T_STR_SPACEPAD},
                                        {H5T_CSET_ASCII, JS_H5T_CSET_ASCII},
                                        {H5T_CSET_UTF8, JS_H5T_CSET_UTF8},
                                        {H5T_C_S1, JS_H5T_C_S1},
                                        {H5T_FORTRAN_S1, JS_H5T_FORTRAN_S1},
                                        {H5T_OPAQUE, JS_H5T_OPAQUE},
                                        //{NODE_OPQ_SIZE,OPQ_SIZE},
                                        //{NODE_OPQ_TAG,OPQ_TAG},
                                        {H5T_COMPOUND, JS_H5T_COMPOUND},
                                        {H5T_ENUM, JS_H5T_ENUM},
                                        {H5T_ARRAY, JS_H5T_ARRAY},
                                        {H5T_VLEN, JS_H5T_VLEN},
                                        //{STRING,NODE_STRING},
                                        //{NUMBER,NODE_NUMBER},
                                        {H5T_NATIVE_INT8, JS_H5T_NATIVE_INT8},
                                        {H5T_NATIVE_UINT8, JS_H5T_NATIVE_UINT8}};

  class Methods : public Attributes {
  protected:
  public:
    Methods(){};
    Methods(hid_t id)
        : Attributes(id){};
    Methods(const Methods& orig) = delete;
    virtual ~Methods(){};

    // static void Initialize (Handle<Object> target);

    static void QueryCallbackDelete(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info);
    static void GetNumAttrs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getAttributeNames(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void readAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void deleteAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetNumObjs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetMemberNames(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetMemberNamesByCreationOrder(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetChildType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDatasetType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDatasetDimensions(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDataType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDatasetAttributes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDatasetAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getByteOrder(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getFilters(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void iterate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void visit(const v8::FunctionCallbackInfo<v8::Value>& args);

  protected:
    int        getNumAttrs();
    hsize_t    getNumObjs();
    H5O_type_t childObjType(const char* objname);
    std::unique_ptr<char[]> getObjnameByIdx(hsize_t idx);
  };
}
