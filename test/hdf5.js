
var hdf5 = require('../build/Release/hdf5');

// open hdf file
var file = new hdf5.File('/home/ryan/data/MillionSongSubset/data/A/A/A/TRAAAAW128F429D538.h5');

// open group
file.group('metadata', function (err, group) {
    
    console.log(group);
    
});