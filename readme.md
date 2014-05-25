A node module for reading/writing the HDF5 file format.

```javascript
    var hdf5 = require('hdf5');

//This will be refactored to match the native h5 for combining
var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

    var file = new hdf5.File('/tmp/foo.h5', Access.ACC_RDONLY);
    
    var group=file.openGroup('pmc');
```
To create a new h5 and put data into it,
```javascript
    var hdf5 = require('hdf5');
    var h5lt = require('h5lt');

//This will be refactored to match the native h5 for combining
var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

    var file = new hdf5.File('/tmp/foo.h5', Access.ACC_TRUNC);
    
    var group=file.createGroup();
    group.create('pmc', file);
    var buffer=new Float64Array(5);
    buffer[0]=1.0;
    buffer[1]=2.0;
    buffer[2]=3.0;
    buffer[3]=4.0;
    buffer[4]=5.0;
    h5lt.makeDataset(group.id, 'Refractive Index', buffer);
    var readBuffer=h5lt.readDataset(group.id, 'Refractive Index');

```
Currently testing with node v0.11.13-pre and V8 3.25.30

## Dependencies

+ [HDF5 C++ Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.13

## Compiling

```
node-gyp configure build
```

When compiling the HDF5 C++ library, be sure to use the `--enable-cxx` flag. I have installed HDF5 into `/usr/local/hdf5`, so modify `binding.gyp` if yours is different.

## Environment Variables

The path to the HDF5 shared objects must be added to the runtime library search path. To do this, `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/hdf5/lib`.

## Running Test

The tests are based on mocha
```
mocha
```

## Experimental

While still very experimental, the compatibility of node/v8 is now being explored for the [High-level HDF5 Interfaces](http://www.hdfgroup.org/HDF5/doc/HL/).  Since the javascript objects have
dimensions and type available the argument list is short to the native side. For example:
```javascript
h5lt.makeDataset();
```
takes three arguments; the id of the group or file, the dataset name and the javascript array object with the data. Reading
a dataset only needs the id of the group or file and the dataset name.  It returns a javascript array compatible with the h5 dataset properties.

Currently rank 1 and 2 datasets are made or read but higher rank is being investigated.  First, all the builtin types will be supported and eventually custom data types
will be attempted.  Particularly I have a need for complex numbers yet I'm on a learning curve in javascript.

The H5IM is now mostly implemented.  The palette portion remains to be implemented. 

The HDF5 C++ Library is being used for the file and group objects and metadata since some persistence on the native side is needed.  Although the c interface
is appealing for enabling creation tracking and ordering of members of a group. Currently this property list is default for groups.

Any ideas for the design of the API and interface are welcome.
