
// import module
var hdf5 = require('../build/Release/hdf5');

// instantiate h5file
var h5file = new hdf5.H5File('/home/ryan/data/MillionSongSubset/data/A/A/A/TRAAAAW128F429D538.h5');

console.log(h5file);
console.log(h5file.filename());
console.log(h5file.filesize());

// close the file
h5file.close();