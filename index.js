
var hdf5=exports;

hdf5.hdf5 = require ("./build/Release/obj.target/hdf5");
hdf5.h5lt = require ("./build/Release/obj.target/h5lt");
hdf5.h5tb = require ("./build/Release/obj.target/h5tb");
hdf5.h5pt = require ("./build/Release/obj.target/h5pt");
hdf5.h5im = require ("./build/Release/obj.target/h5im");

console.dir("hdf5 exports");
module.exports=hdf5;