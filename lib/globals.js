
module.exports.Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

module.exports.CreationOrder = {
    H5P_CRT_ORDER_TRACKED : 1,
    H5P_CRT_ORDER_INDEXED : 2
};

module.exports.State = {
  COUNT : 0,
  TITLE : 1,
  DATA : 2
};

module.exports.HLType = {
    HL_TYPE_UNKNOWN : -1,      /* Unknown object type                      */
    HL_TYPE_LITE : 0,             /* Lite dataset interface                        */
    HL_TYPE_IMAGE : 1,           /* Image                      */
    HL_TYPE_TABLE : 2,    /* Table   */
    HL_TYPE_PACKET_TABLE : 3,             /* Packets       */
    HL_TYPE_DIMENSION_SCALES : 4,             /*        */
    HL_TYPE_OPTIMIZED_FUNCTIONS : 5,             /*        */
    HL_TYPE_EXTENSIONS : 6             /*        */
}

module.exports.H5OType = {
    H5O_TYPE_UNKNOWN : -1,      /* Unknown object type                      */
    H5O_TYPE_GROUP : 0,             /* Object is a group                        */
    H5O_TYPE_DATASET : 1,           /* Object is a dataset                      */
    H5O_TYPE_NAMED_DATATYPE : 2,    /* Object is a committed (named) datatype   */
    H5O_TYPE_NTYPES : 3             /* Number of different object types (must   */
                                /* be last!)                                */
}
module.exports.yytokentype = {
     H5T_STD_I8BE_TOKEN : 258,
     H5T_STD_I8LE_TOKEN : 259,
     H5T_STD_I16BE_TOKEN : 260,
     H5T_STD_I16LE_TOKEN : 261,
     H5T_STD_I32BE_TOKEN : 262,
     H5T_STD_I32LE_TOKEN : 263,
     H5T_STD_I64BE_TOKEN : 264,
     H5T_STD_I64LE_TOKEN : 265,
     H5T_STD_U8BE_TOKEN : 266,
     H5T_STD_U8LE_TOKEN : 267,
     H5T_STD_U16BE_TOKEN : 268,
     H5T_STD_U16LE_TOKEN : 269,
     H5T_STD_U32BE_TOKEN : 270,
     H5T_STD_U32LE_TOKEN : 271,
     H5T_STD_U64BE_TOKEN : 272,
     H5T_STD_U64LE_TOKEN : 273,
     H5T_NATIVE_CHAR_TOKEN : 274,
     H5T_NATIVE_SCHAR_TOKEN : 275,
     H5T_NATIVE_UCHAR_TOKEN : 276,
     H5T_NATIVE_SHORT_TOKEN : 277,
     H5T_NATIVE_USHORT_TOKEN : 278,
     H5T_NATIVE_INT_TOKEN : 279,
     H5T_NATIVE_UINT_TOKEN : 280,
     H5T_NATIVE_LONG_TOKEN : 281,
     H5T_NATIVE_ULONG_TOKEN : 282,
     H5T_NATIVE_LLONG_TOKEN : 283,
     H5T_NATIVE_ULLONG_TOKEN : 284,
     H5T_IEEE_F32BE_TOKEN : 285,
     H5T_IEEE_F32LE_TOKEN : 286,
     H5T_IEEE_F64BE_TOKEN : 287,
     H5T_IEEE_F64LE_TOKEN : 288,
     H5T_NATIVE_FLOAT_TOKEN : 289,
     H5T_NATIVE_DOUBLE_TOKEN : 290,
     H5T_NATIVE_LDOUBLE_TOKEN : 291,
     H5T_STRING_TOKEN : 292,
     STRSIZE_TOKEN : 293,
     STRPAD_TOKEN : 294,
     CSET_TOKEN : 295,
     CTYPE_TOKEN : 296,
     H5T_VARIABLE_TOKEN : 297,
     H5T_STR_NULLTERM_TOKEN : 298,
     H5T_STR_NULLPAD_TOKEN : 299,
     H5T_STR_SPACEPAD_TOKEN : 300,
     H5T_CSET_ASCII_TOKEN : 301,
     H5T_CSET_UTF8_TOKEN : 302,
     H5T_C_S1_TOKEN : 303,
     H5T_FORTRAN_S1_TOKEN : 304,
     H5T_OPAQUE_TOKEN : 305,
     OPQ_SIZE_TOKEN : 306,
     OPQ_TAG_TOKEN : 307,
     H5T_COMPOUND_TOKEN : 308,
     H5T_ENUM_TOKEN : 309,
     H5T_ARRAY_TOKEN : 310,
     H5T_VLEN_TOKEN : 311,
     STRING : 312,
     NUMBER : 313
     }