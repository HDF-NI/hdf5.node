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

The dataset members of a group can be retieved in order of creation. Something I've need more often than not.
```
            var groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
            var frequencyNames=groupFrequencies.getMemberNamesByCreationOrder();
            for (var frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
            {
                xmolDocument+=elements.length+'\n';
                xmolDocument+=frequencyNames[frequencyIndex]+'\n';
                var frequency=h5lt.readDataset(groupFrequencies.id, frequencyNames[frequencyIndex]);
                for (var index = 0; index < elements.length; index++)
                {
                    xmolDocument+=elements[index]+' '+lastTrajectory[3*index]+' '+lastTrajectory[3*index+1]+' '+lastTrajectory[3*index+2]+' '+frequency[3*index]+' '+frequency[3*index+1]+' '+frequency[3*index+2]+'\n';
                }
            }

```

Attributes can be attached to Groups by flush after the properties are added to javascript group instance.  Prototype properties also get flushed.
Because javscript has the ability to have property names with spaces are similar characters via [ '' ] this is matched to the h5's similar nature.
```
            var groupTargets=file.createGroup();
            groupTargets.create('pmcservices/sodium-icosanoate', file);
            groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
            groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
            groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
            groupTargets.Status=256;
            groupTargets.Information="\"There are no solutions; there are only trade-offs.\" -- Thomas Sowell";
            groupTargets.flush();
```
If an attribute is already there it will get updated by h5 exists check, remove and then added back. Types are allowed to change to match the javascript side.
When opening an h5 the group's attributes can be refreshed to the javascript in reverse manner
```
            var groupTarget=file.openGroup('pmcservices/sodium-icosanoate');
            groupTarget.refresh();
            console.dir(groupTarget.[ 'Computed Heat Of Formation' ]);
            console.dir(groupTarget.Information);
```

Currently testing with node v0.11.13-pre and V8 3.25.30

And a legacy development for node v0.10.31 and V8 3.14.5.9 resides in ./legacy/node-v0.10.31. This probably will work a number of v0.10.x's.
Go into the ./legacy/node-v0.10.31 folder and with npm, node-gyp and node pointing to compatible version, compile the same as below. Then go back two folders and test using the same javascript code.  Make sure your NODEPATH has your obj.target.
For example:
```
export NODE_PATH=/home/roger/NodeProjects/hdf5.node/build/Release/obj.target:$NODE_PATH

```

## Dependencies

+ [HDF5 C++ Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.13
        (untested yet should work numerous v5-1.8.x's)

## Compiling

```
node-gyp configure build
```

When compiling the HDF5 C++ library, be sure to use the `--enable-cxx` flag. I have installed HDF5 to sibling folder to `./hdf5.node/../hdf5`, so modify `binding.gyp` if yours is different.

## Environment Variables

The path to the HDF5 shared objects must be added to the runtime library search path. To do this, `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./hdf5.node/../hdf5/lib`.

## Running Test

The tests are based on mocha
```
mocha
```
or
```
mocha --require should
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
Attributes are refreshed from and flushed to Groups with the properties on javascript objects. More attributes will soon be possible on the file object itself and on datasets.

The HDF5 C++ Library is being used for the file and group objects and metadata since some persistence on the native side is needed.  Although the c interface
is appealing for enabling creation tracking and ordering of members of a group. Currently this property list is default for groups.

Any ideas for the design of the API and interface are welcome.
