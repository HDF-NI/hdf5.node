
module.exports.Access = {
  ACC_RDONLY : 0, /*absence of rdwr => rd-only */
  ACC_RDWR   : 1, /*open for read and write    */
  ACC_TRUNC  : 2, /*overwrite existing files   */
  ACC_EXCL   : 4, /*fail if file already exists*/
  ACC_DEBUG  : 8, /*print debug info      */
  ACC_CREAT  : 10, /*create non-existing files  */
  ACC_SWMR_WRITE : 32,  /*indicate that this file is
                            * open for writing in a
                            * single-writer/multi-reader (SWMR)
                            * scenario.  Note that the
                            * process(es) opening the file
                            * for reading must open the file
                            * with RDONLY access, and use
                            * the special "SWMR_READ" access
                            * flag. */
  ACC_SWMR_READ : 64    /*indicate that this file is
                            * open for reading in a
                            * single-writer/multi-reader (SWMR)
                            * scenario.  Note that the
                            * process(es) opening the file
                            * for SWMR reading must also
                            * open the file with the RDONLY
                            * flag.  */
};

module.exports.CreationOrder = {
  H5P_CRT_ORDER_TRACKED : 1,
  H5P_CRT_ORDER_INDEXED : 2
};

module.exports.State = {
  COUNT : 0,
  TITLE : 1,
  DATA  : 2
};

module.exports.H5SType = {
  H5S_UNLIMITED : -1
}

module.exports.HLType = {
    HL_TYPE_UNKNOWN             : -1, /* Unknown object type                      */
    HL_TYPE_LITE                : 0,  /* Lite dataset interface                        */
    HL_TYPE_IMAGE               : 1,  /* Image                      */
    HL_TYPE_TABLE               : 2,  /* Table   */
    HL_TYPE_PACKET_TABLE        : 3,  /* Packets       */
    HL_TYPE_TEXT                : 4,
    HL_TYPE_DIMENSION_SCALES    : 5,  /*        */
    HL_TYPE_OPTIMIZED_FUNCTIONS : 6,  /*        */
    HL_TYPE_EXTENSIONS          : 7   /*        */
}

module.exports.H5OType = {
  H5O_TYPE_UNKNOWN        : -1, /* Unknown object type                      */
  H5O_TYPE_GROUP          : 0,  /* Object is a group                        */
  H5O_TYPE_DATASET        : 1,  /* Object is a dataset                      */
  H5O_TYPE_NAMED_DATATYPE : 2,  /* Object is a committed (named) datatype   */
  H5O_TYPE_NTYPES         : 3   /* Number of different object types (must   */
                                /* be last!)                                */
}

module.exports.H5TOrder = {
  H5T_ORDER_LE    :0,  /* Little-endian byte order */
  H5T_ORDER_BE    :1, /*  Big-endian byte order */
  H5T_ORDER_VAX   :2, /* VAX mixed byte order  */
  H5T_ORDER_MIXED :3, /* Mixed byte order among members of a compound datatype (see below)  */
  H5T_ORDER_NONE  :4, /* No particular order (fixed-length strings, object and region references) */ 
}

module.exports.H5ZType = {
  H5Z_FILTER_ALL         : 0,       //Removes all filters from the filter pipeline.
  H5Z_FILTER_DEFLATE     : 1, //Data compression filter, employing the gzip algorithm
  H5Z_FILTER_SHUFFLE     : 2, //Data shuffling filter
  H5Z_FILTER_FLETCHER32  : 3, //Error detection filter, employing the Fletcher32 checksum algorithm
  H5Z_FILTER_SZIP        : 4, //Data compression filter, employing the SZIP algorithm
  H5Z_FILTER_NBIT        : 5, //Data compression filter, employing the N-Bit algorithm
  H5Z_FILTER_SCALEOFFSET : 6  //Data compression filter, employing the scale-offset algorithm
}

module.exports.H5Type = {
  H5T_STD_I8BE       : 258,
  H5T_STD_I8LE       : 259,
  H5T_STD_I16BE      : 260,
  H5T_STD_I16LE      : 261,
  H5T_STD_I32BE      : 262,
  H5T_STD_I32LE      : 263,
  H5T_STD_I64BE      : 264,
  H5T_STD_I64LE      : 265,
  H5T_STD_U8BE       : 266,
  H5T_STD_U8LE       : 267,
  H5T_STD_U16BE      : 268,
  H5T_STD_U16LE      : 269,
  H5T_STD_U32BE      : 270,
  H5T_STD_U32LE      : 271,
  H5T_STD_U64BE      : 272,
  H5T_STD_U64LE      : 273,
  H5T_NATIVE_CHAR    : 274,
  H5T_NATIVE_SCHAR   : 275,
  H5T_NATIVE_UCHAR   : 276,
  H5T_NATIVE_SHORT   : 277,
  H5T_NATIVE_USHORT  : 278,
  H5T_NATIVE_INT     : 279,
  H5T_NATIVE_UINT    : 280,
  H5T_NATIVE_LONG    : 281,
  H5T_NATIVE_ULONG   : 282,
  H5T_NATIVE_LLONG   : 283,
  H5T_NATIVE_ULLONG  : 284,
  H5T_IEEE_F32BE     : 285,
  H5T_IEEE_F32LE     : 286,
  H5T_IEEE_F64BE     : 287,
  H5T_IEEE_F64LE     : 288,
  H5T_NATIVE_FLOAT   : 289,
  H5T_NATIVE_DOUBLE  : 290,
  H5T_NATIVE_LDOUBLE : 291,
  H5T_STRING         : 292,
  NODE_STRSIZE       : 293,
  NODE_STRPAD        : 294,
  NODE_CSET          : 295,
  NODE_CTYPE         : 296,
  H5T_VARIABLE       : 297,
  H5T_STR_NULLTERM   : 0,
  H5T_STR_NULLPAD    : 1,
  H5T_STR_SPACEPAD   : 2,
  H5T_CSET_ASCII     : 301,
  H5T_CSET_UTF8      : 302,
  H5T_C_S1           : 303,
  H5T_FORTRAN_S1     : 304,
  H5T_OPAQUE         : 305,
  NODE_OPQ_SIZE      : 306,
  NODE_OPQ_TAG       : 307,
  H5T_COMPOUND       : 308,
  H5T_ENUM           : 309,
  H5T_ARRAY          : 310,
  H5T_VLEN           : 311,
  NODE_STRING        : 312,
  NODE_NUMBER        : 313,
  H5T_NATIVE_INT8    : 314,
  H5T_NATIVE_UINT8   : 315
}

module.exports.H5RType ={
  H5R_BADTYPE         : (-1),   /*invalid Reference Type                     */
  H5R_OBJECT          : 0,      /*Object reference                           */
  H5R_DATASET_REGION  : 1,      /*Dataset Region Reference                   */
  H5R_MAXTYPE         : 2       /*highest type (Invalid as true type)	     */
}

module.exports.Interlace = {
  INTERLACE_PIXEL : "INTERLACE_PIXEL",
  INTERLACE_PLANE : "INTERLACE_PLANE"
}