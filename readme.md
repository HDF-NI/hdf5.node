A node module for reading the HDF5 file format.

```javascript
    var hdf5 = require('hdf5');
    var file = new hdf5.File('/tmp/foo.h5');
    
    file.group('group1', function (err, group) {
        console.log(group);
    });
```

## Dependencies

+ [HDF5 C++ Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.8

## Compiling

```
node-gyp configure build
```

When compiling the HDF5 C++ library, be sure to use the `--enable-cxx` flag. I have installed HDF5 into `/usr/local/hdf5`, so modify `binding.gyp` if yours is different.

## Environment Variables

The path to the HDF5 shared objects must be added to the runtime library search path. To do this, `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/hdf5/lib`.

## Experimental

This module is unfinished and is not in a useable state. It does not implement much of anything. I started writing it as a refresher on C++ and to learn some v8. This module demonstrates usage of gyp for node.js, as well as a few common v8 usages.

I would have continued but I have no use for HDF5.