A node module for reading/writing the HDF5 file format. The koa based browser interface is a reference app for viewing, modifying and looking at h5 content. Eventually it will provide for editing, charting and performing statistics on h5 file data.  
The interface is now showing images, datasets, column tables and string based packet tables. Basic group operations are becoming available with right-click on a node.  Hovering on a group shows attributes in a tooltip however stil looking for 
a good mechanism to add, copy and edit attributes.  Images can be dropped on the main panel after selecting a group and will be stored at the equivalent place in the h5.  Learning how to accomplish more HDFView functionality in browser style.  
Experimenting with https://ethercalc.net/ as an editor; there is an upper limit to practical data going from h5 into a spreadsheet and other mechanism may need to be provided.  It has some charting yet that may need addressed with a d3 or threejs approach.

Unlike other languages that wrap hdf5 API's this interface takes advantage of the compatibility of V8 and HDF5. The result 
is a direct map to javascript behavior with the least amount of data copying and coding tasks for the user. Hopefully you 
won't need to write yet another layer in your code to accomplish your goals.

The node::Buffer and streams are being investigated so native hdf5 data's only destination is client browser window or client in general.

API documentation is available at http://ryancole.github.io/hdf5.node
```javascript
var hdf5 = require('hdf5').hdf5;

var Access = require('hdf5/lib/globals').Access;
var file = new hdf5.File('/tmp/foo.h5', Access.ACC_RDONLY);
var group=file.openGroup('pmc');
```

### High-level datasets as javascript arrays
To create a new h5 and put data into it,

```javascript
var hdf5 = require('hdf5').hdf5;
var h5lt = require('hdf5').h5lt;

var Access = require('hdf5/lib/globals').Access;

var file = new hdf5.File('/tmp/foo.h5', Access.ACC_TRUNC);
    
var group=file.createGroup('pmc');
var buffer=new Float64Array(5);
buffer[0]=1.0;
buffer[1]=2.0;
buffer[2]=3.0;
buffer[3]=4.0;
buffer[4]=5.0;
h5lt.makeDataset(group.id, 'Refractive Index', buffer);
var readBuffer=h5lt.readDataset(group.id, 'Refractive Index');

//A rank two dataset with 3 columns of doubles
var frequency=new Float64Array(3*numberOfDataLines);
var groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
groupFrequencies.open('pmcservices/sodium-icosanoate/Frequency Data/Frequencies', file);
frequency.rank=2;
frequency.rows=numberOfDataLines;
frequency.columns=3;
h5lt.makeDataset(groupFrequencies.id, title, frequency);
```

The dataset members of a group can be retrieved in order of creation. Something I've needed more often than not.

```javascript
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

### High-level datasets as nodejs Buffer's
If a Buffer http://nodejs.org/docs/v0.12.0/api/buffer.html is filled with pure datatype(e.g. double) it can be written to h5 as a dataset.

```javascript
var H5Type = require('hdf5/lib/globals.js').H5Type;

var buffer=new Buffer(5*8, "binary");
buffer.type=H5Type.H5T_NATIVE_DOUBLE;
buffer.writeDoubleLE(1.0, 0);
buffer.writeDoubleLE(2.0, 8);
buffer.writeDoubleLE(3.0, 16);
buffer.writeDoubleLE(4.0, 24);
buffer.writeDoubleLE(5.0, 32);
h5lt.makeDataset(group.id, 'Dielectric Constant', buffer);
```

will assume the rank is one. Rank, rows, columns and sections can be set to shape the dataset.

```javascript
var H5Type = require('hdf5/lib/globals.js').H5Type;
var buffer=new Buffer(6*8, "binary");
buffer.type=H5Type.H5T_NATIVE_DOUBLE;
buffer.writeDoubleLE(1.0, 0);
buffer.writeDoubleLE(2.0, 8);
buffer.writeDoubleLE(3.0, 16);
buffer.writeDoubleLE(1.0, 24);
buffer.writeDoubleLE(2.0, 32);
buffer.writeDoubleLE(3.0, 40);
buffer.rank=2;
buffer.rows=3;
buffer.columns=2;
h5lt.makeDataset(group.id, 'Two Rank', buffer);
```

is 3 by 2 dataset.  To read from h5 the dataset can still transfer to a javascript array or with the method
readDatasetAsBuffer the return is a nodejs Buffer with the shape properties set.

```javascript
var readBuffer=h5lt.readDataset(group.id, 'Two Rank');
readBuffer.constructor.name.should.match('Float64Array');

var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Two Rank');
```

### High-level Images

Currently the image subclass is "IMAGE_TRUECOLOR".  The rest of the spec[http://www.hdfgroup.org/HDF5/doc/ADGuide/ImageSpec.html is coming.  Need test examples.
Image data sent to and from html5's canvas 2d context putImageData and getImageData are compatible with IMAGE_TRUECOLOR" with 4 planes and pixel interlacing.
At this point I experimented with binaryjs for data transfer to html5 canvas yet underlying mechanism can do binary.  However this provided streams that worked 
and handles node::Buffer's. Although eventually streaming from the native side may replace this.  The image data is handled without any particular image processing 
library server side so you can choose which one you prefer depending on application. For the html5 interface the image format is handled by the canvas and 
window.URL.createObjectURL(file) in the drop zone.

```javascript
var buffer=h5im.readImage(parent.id, name);
buffer.width, buffer.height, buffer.planes

var image = Buffer.concat(buffers);
image.interlace="INTERLACE_PIXEL" or "INTERLACE_PLANE";
image.planes=4;
image.width=meta.width;
image.height=meta.height;
h5im.makeImage(group.id, name, image);
```


### High-level Tables

```javascript
var table=new Array(4);
var fieldArray1=new Uint32Array(5);
fieldArray1.name="Index";
fieldArray1[0]=0;
fieldArray1[1]=1;
fieldArray1[2]=2;
fieldArray1[3]=3;
fieldArray1[4]=4;
table[0]=fieldArray1;
var fieldArray2=new Float64Array(5);
fieldArray2.name="Count Up";
fieldArray2[0]=1.0;
fieldArray2[1]=2.0;
fieldArray2[2]=3.0;
fieldArray2[3]=4.0;
fieldArray2[4]=5.0;
table[1]=fieldArray2;
var fieldArray3=new Float64Array(5);
fieldArray3.name="Count Down";
fieldArray3[0]=5.0;
fieldArray3[1]=4.0;
fieldArray3[2]=3.0;
fieldArray3[3]=2.0;
fieldArray3[4]=1.0;
table[2]=fieldArray3;
var fieldArray4=new Array(5);
fieldArray4.name="Residues";
fieldArray4[0]="ALA";
fieldArray4[1]="VAL";
fieldArray4[2]="HIS";
fieldArray4[3]="LEU";
fieldArray4[4]="HOH";
table[3]=fieldArray4;
h5tb.makeTable(group.id, 'Reflections', table);
var readTable=h5tb.readTable(group.id, "Reflections");
```

A column of strings is set fixed width to the widest in the set(working on other possible solutions). The return table is equivalent

            h5tb.makeTable(id, name, table model)
            h5tb.readTable(id, name)
            h5tb.appendRecords(id, name, table model)
            h5tb.writeRecords(id, name, start, table model)
            h5tb.readRecords(id, name, start, nrecords)
            h5tb.deleteRecord(id, name, start, nrecords)
            h5tb.insertRecord(id, name, start, table model)
            h5tb.writeFieldsName(id, name, start, table model);
            h5tb.writeFieldsIndex(id, name, 0, table model, column index array)
            h5tb.readFieldsName(id, name, start, nrecords, column name array)
            h5tb.readFieldsIndex(id, name, start, nrecords, column index array)
            h5tb.getTableInfo(id, 'Reflections')
            h5tb.getFieldInfo(id, 'Reflections')
            h5tb.deleteField(id, name, field name)
            h5tb.insertField(id, name, start, table model)
            h5tb.addRecordsFrom(id, source name, start1, nrecords, destination name, start2)
            h5tb.combineTables(id1, name1, id2, name2, destination name)

### High-level Packet Tables

This one is experimental and can only do variable length strings today(and I had a need for it)

```javascript
var table=new h5pt.PacketTable(0, 5);
table.record=new Object();
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:10:44 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.4";
table.record[ "Units" ]="Celcius";
h5pt.makeTable(group.id, 'Events', table);
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:20:45 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.3";
table.record[ "Units" ]="Celcius";
table.append();
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:20:46 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.5";
table.record[ "Units" ]="Celcius";
table.append();
table.close();
```

after the initial make the representative table object can do further appends.  The close is important for the h5.
To read, the next method refills the record with the current packet's data and is ready for the subsequent packet until it returns a false.

```javascript
var table=h5pt.readTable(groupTarget.id, "Events");
for (var name in table.record) {
    console.dir(name);
}
while(table.next()){
    console.dir(table.record[ "Set" ]+" "+table.record[ "Date Time" ]+" "+table.record[ "Name" ]+" "+table.record[ "Value" ]+" "+table.record[ "Units" ]);
}
table.close();
```

### Dimension Scales

to be implemented

### High-level Functions for Region References, Hyperslabs, and Bit-fields

Looking at this.  It does require a separate download and install because it is not a part of the standard hdf5 distribution.


### Properties as h5 metadata attributes

Attributes can be attached to Groups by flush after the properties are added to javascript group instance.  Prototype properties also get flushed.
Because javascript has the ability to have property names with spaces via [ '' ] these readily map to h5's similar nature.

```javascript
var groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
groupTargets.Status=256;
groupTargets.Information="\"There are no solutions; there are only trade-offs.\" -- Thomas Sowell";
groupTargets.flush();
```

Types are allowed to change to match the javascript side. If an attribute is already there it will get updated by h5's existence check, remove and then added back. 
When opening an h5 the group's attributes can be refreshed to the javascript in a reverse manner

```javascript
var groupTarget=file.openGroup('pmcservices/sodium-icosanoate');
groupTarget.refresh();
console.dir(groupTarget.[ 'Computed Heat Of Formation' ]);
console.dir(groupTarget.Information);
```

## Status

Currently testing with node v0.12.0 and V8 3.28.73.  Builds on Ubuntu 14_04, CentOS 6 and MacOSX 10.7.  Working on getting access to a 10.10 version to solve later build issues. And setting up a windows work environment for VS2013 [any knowledge about node-gyp on these platforms would be much appreciated] 
Resources leaks are being found when the h5 file is closed.  When found they are being eliminated.  Error handling component is being investigated; how to best leverage V8 and node from the native side.

A legacy development for node v0.10.31 and V8 3.14.5.9 resides in ./legacy/node-v0.10.31. Further development of legacy is suspended since nodejs v0.12.0 has been released.

## Dependencies

+ [HDF5 C Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.14
        (Prior v5-1.8.x's untested yet should work)

The `binding.gyp` expects the HDF5_HOME environment variable set to your install.


## Compiling

The code requires a gcc compiler supporting C++11 for linux. MacOSX build target has been added.  Windows build target is coming. The binding.gyp defines the cflags with -std=c++11.  There isn't any cxxflags that I know of but cflags in node-gyp does 
effect g++.

### In a working copy of git

```bash
export HDF5_HOME=/home/roger/NodeProjects/hdf5
export NODE_PATH=/home/roger/NodeProjects/hdf5.node/build/Release/obj.target:$NODE_PATH
node-gyp configure build
```

NODE_PATH is still used for the mocha tests.

### Including as a node module

The HDF5_HOME needs to be set. NODE_PATH should not. Then an 'npm install hdf5' will pull the version and build it for you
in node_modules/hdf5. There is no need for the node-gyp step.


## Environment Variables

The path to the HDF5 shared objects must be added to the runtime library search path. 

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./hdf5.node/../hdf5/lib
```

## Running Test

The tests are based on co-mocha

```bash
mocha
```
or
```bash
mocha --require should  --require co-mocha
```

To launch the view:
```bash
node --harmony  ./lib/application.js 3000 "./roothaan.h5"
```
will serve the interface to the h5 on port 3000. If the h5 doesn't exist it will be created and the interface
 can then add groups and content/images can be dragged and dropped while being displayed and stored back in the h5.

## Experimental

The h5im is being designed to meet the Image Spec 1.2 http://www.hdfgroup.org/HDF5/doc/ADGuide/ImageSpec.html


Any ideas for the design of the API and interface are welcome.
