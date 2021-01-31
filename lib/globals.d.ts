// Type definitions for hdf5 0.3
// Project: https://github.com/HDF-NI/hdf5.node#readme
// Definitions by: NINI1988 <https://github.com/NINI1988>

export declare enum Access {
    /** absence of rdwr => rd-only */
    ACC_RDONLY,
    /** open for read and write    */
    ACC_RDWR,
    /** overwrite existing files   */
    ACC_TRUNC,
    /** fail if file already exists*/
    ACC_EXCL,
    /** print debug info      */
    ACC_DEBUG,
    /** create non-existing files  */
    ACC_CREAT
    /** read/write in SWMR mode*/	
    ACC_SWMR_WRITE
    /** read in SWMR mode*/	
    ACC_SWMR_READ
}

export declare enum CreationOrder {
    H5P_CRT_ORDER_TRACKED,
    H5P_CRT_ORDER_INDEXED
}

export declare enum State {
    COUNT,
    TITLE,
    DATA
}

export declare enum H5SType {
    H5S_UNLIMITED = -1
}

export declare enum HLType {
    HL_TYPE_UNKNOWN, /* Unknown object type                      */
    HL_TYPE_LITE,  /* Lite dataset interface                        */
    HL_TYPE_IMAGE,  /* Image                      */
    HL_TYPE_TABLE,  /* Table   */
    HL_TYPE_PACKET_TABLE,  /* Packets       */
    HL_TYPE_TEXT,
    HL_TYPE_DIMENSION_SCALES,  /*        */
    HL_TYPE_OPTIMIZED_FUNCTIONS,  /*        */
    HL_TYPE_EXTENSIONS   /*        */
}

export declare enum H5OType {
    H5O_TYPE_UNKNOWN, /* Unknown object type                      */
    H5O_TYPE_GROUP,  /* Object is a group                        */
    H5O_TYPE_DATASET,  /* Object is a dataset                      */
    H5O_TYPE_NAMED_DATATYPE,  /* Object is a committed (named) datatype   */
    H5O_TYPE_NTYPES   /* Number of different object types (must   */
    /* be last!)                                */
}

export declare enum H5TOrder {
    H5T_ORDER_LE,  /* Little-endian byte order */
    H5T_ORDER_BE, /*  Big-endian byte order */
    H5T_ORDER_VAX, /* VAX mixed byte order  */
    H5T_ORDER_MIXED, /* Mixed byte order among members of a compound datatype (see below)  */
    H5T_ORDER_NONE, /* No particular order (fixed-length strings, object and region references) */
}

export declare enum H5ZType {
    H5Z_FILTER_ALL,       //Removes all filters from the filter pipeline.
    H5Z_FILTER_DEFLATE, //Data compression filter, employing the gzip algorithm
    H5Z_FILTER_SHUFFLE, //Data shuffling filter
    H5Z_FILTER_FLETCHER32, //Error detection filter, employing the Fletcher32 checksum algorithm
    H5Z_FILTER_SZIP, //Data compression filter, employing the SZIP algorithm
    H5Z_FILTER_NBIT, //Data compression filter, employing the N-Bit algorithm
    H5Z_FILTER_SCALEOFFSET  //Data compression filter, employing the scale-offset algorithm
}

export declare enum H5Type {
    H5T_STD_I8BE,
    H5T_STD_I8LE,
    H5T_STD_I16BE,
    H5T_STD_I16LE,
    H5T_STD_I32BE,
    H5T_STD_I32LE,
    H5T_STD_I64BE,
    H5T_STD_I64LE,
    H5T_STD_U8BE,
    H5T_STD_U8LE,
    H5T_STD_U16BE,
    H5T_STD_U16LE,
    H5T_STD_U32BE,
    H5T_STD_U32LE,
    H5T_STD_U64BE,
    H5T_STD_U64LE,
    H5T_NATIVE_CHAR,
    H5T_NATIVE_SCHAR,
    H5T_NATIVE_UCHAR,
    H5T_NATIVE_SHORT,
    H5T_NATIVE_USHORT,
    H5T_NATIVE_INT,
    H5T_NATIVE_UINT,
    H5T_NATIVE_LONG,
    H5T_NATIVE_ULONG,
    H5T_NATIVE_LLONG,
    H5T_NATIVE_ULLONG,
    H5T_IEEE_F32BE,
    H5T_IEEE_F32LE,
    H5T_IEEE_F64BE,
    H5T_IEEE_F64LE,
    H5T_NATIVE_FLOAT,
    H5T_NATIVE_DOUBLE,
    H5T_NATIVE_LDOUBLE,
    H5T_STRING,
    NODE_STRSIZE,
    NODE_STRPAD,
    NODE_CSET,
    NODE_CTYPE,
    H5T_VARIABLE,
    H5T_STR_NULLTERM,
    H5T_STR_NULLPAD,
    H5T_STR_SPACEPAD,
    H5T_CSET_ASCII,
    H5T_CSET_UTF8,
    H5T_C_S1,
    H5T_FORTRAN_S1,
    H5T_OPAQUE,
    NODE_OPQ_SIZE,
    NODE_OPQ_TAG,
    H5T_COMPOUND,
    H5T_ENUM,
    H5T_ARRAY,
    H5T_VLEN,
    NODE_STRING,
    NODE_NUMBER,
    H5T_NATIVE_INT8,
    H5T_NATIVE_UINT8,
}

export declare enum H5RType {
  H5R_BADTYPE         = -1,     /*invalid Reference Type                     */
  H5R_OBJECT          = 0,      /*Object reference                           */
  H5R_DATASET_REGION  = 1,      /*Dataset Region Reference                   */
  H5R_MAXTYPE         = 2       /*highest type (Invalid as true type)	     */
}


export declare enum Interlace {
    INTERLACE_PIXEL,
    INTERLACE_PLANE
}
