---
layout: page
title: "Making & Reading Datasets"
category: tut
date: 2015-05-02 20:14:07
---

###Datasets as javascript arrays
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

###Datasets as nodejs Buffer's
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
