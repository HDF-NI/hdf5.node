
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

module.exports.H5OType = {
    H5O_TYPE_UNKNOWN : -1,      /* Unknown object type                      */
    H5O_TYPE_GROUP : 0,             /* Object is a group                        */
    H5O_TYPE_DATASET : 1,           /* Object is a dataset                      */
    H5O_TYPE_NAMED_DATATYPE : 2,    /* Object is a committed (named) datatype   */
    H5O_TYPE_NTYPES : 3             /* Number of different object types (must   */
                                /* be last!)                                */
}
