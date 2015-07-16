
var hdf5=exports;

hdf5.hdf5 = require ("./build/Release/hdf5");
hdf5.h5lt = require ("./build/Release/h5lt");
hdf5.h5tb = require ("./build/Release/h5tb");
hdf5.h5pt = require ("./build/Release/h5pt");
hdf5.h5im = require ("./build/Release/h5im");
hdf5.h5ds = require ("./build/Release/h5ds");

module.exports=hdf5;