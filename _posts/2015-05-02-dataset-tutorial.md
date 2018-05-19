---
layout: page
title: "Making & Reading Datasets"
category: tut
date: 2015-05-02 20:14:07
---

### Datasets as javascript arrays
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
h5lt.makeDataset(groupFrequencies.id, title, frequency, {rank: 2, rows: numberOfDataLines, columns: 3});
```

The dataset members of a group can be retrieved in order of creation. Something
I've needed more often than not.

```javascript
var groupFrequencies = file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
var frequencyNames = groupFrequencies.getMemberNamesByCreationOrder();
for (var frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
{
  xmolDocument += elements.length+'\n';
  xmolDocument += frequencyNames[frequencyIndex]+'\n';
  var frequency = h5lt.readDataset(groupFrequencies.id, frequencyNames[frequencyIndex]);
  for (var index = 0; index < elements.length; index++) {
    xmolDocument += elements[index] + ' ' + lastTrajectory[3 * index] + ' ' + lastTrajectory[3*index+1] + ' ' + lastTrajectory[3*index+2] + ' ' + frequency[3 * index] + ' ' + frequency[3 * index + 1] + ' ' + frequency[3 * index + 2] + '\n';
  }
}
```

### Array of variable length strings

To store variable length strings, put your javascript strings into an Array and
pass as third argument of makeDataset. To retrieve use readDataset to get an
equivalent Array filled with the strings.

```javascript
try
{
    var file = new hdf5.File('./roothaan.h5', Access.ACC_TRUNC);
    var group=file.createGroup('pmcservices/Quotes');
    var quotes=new Array(7);
    quotes[0]="Never put off till tomorrow what may be done day after tomorrow just as well.\0";
    quotes[1]="I have never let my schooling interfere with my education";
    quotes[2]="Reader, suppose you were an idiot. And suppose you were a member of Congress. But I repeat myself.";
    quotes[3]="Substitute 'damn' every time you're inclined to write 'very;' your editor will delete it and the writing will be just as it should be.";
    quotes[4]="Don’t go around saying the world owes you a living. The world owes you nothing. It was here first.";
    quotes[5]="Loyalty to country ALWAYS. Loyalty to government, when it deserves it.";
    quotes[6]="What would men be without women? Scarce, sir...mighty scarce.";
    h5lt.makeDataset(group.id, "Mark Twain", quotes);
    group.close();
    file.close();
    file = new hdf5.File('./roothaan.h5', Access.ACC_RDWR);
    group=file.openGroup('pmcservices/Quotes');
    var array=h5lt.readDataset(group.id, 'Mark Twain');
    console.dir(array.length);
    if(array.constructor.name==='Array'){
        for(var mIndex=0;mIndex<array.length;mIndex++){
            console.dir(array[mIndex]);
        }
    }
    group.close();
    file.close();
}
catch(err) {
    console.dir(err.message);
}
```
Currently this interface supports only the custom variable length string
datatype in the h5 array class.

### Datasets as nodejs Buffers

If a [Buffer](https://nodejs.org/api/buffer.html) is filled with pure datatype
(e.g. double) it can be written to h5 as a dataset.

```javascript
var H5Type = require('hdf5/lib/globals.js').H5Type;

var buffer=Buffer.alloc(5*8, "\0", "binary");
buffer.type=H5Type.H5T_NATIVE_DOUBLE;
buffer.writeDoubleLE(1.0, 0);
buffer.writeDoubleLE(2.0, 8);
buffer.writeDoubleLE(3.0, 16);
buffer.writeDoubleLE(4.0, 24);
buffer.writeDoubleLE(5.0, 32);
h5lt.makeDataset(group.id, 'Dielectric Constant', buffer);
```

will assume the rank is one. Rank, rows, columns, sections and files can be set to
shape the dataset. The files property is the fourth dimension.

```javascript
var H5Type = require('hdf5/lib/globals.js').H5Type;
var buffer=Buffer.alloc(6*8, "\0", "binary");
buffer.writeDoubleLE(1.0, 0);
buffer.writeDoubleLE(2.0, 8);
buffer.writeDoubleLE(3.0, 16);
buffer.writeDoubleLE(1.0, 24);
buffer.writeDoubleLE(2.0, 32);
buffer.writeDoubleLE(3.0, 40);
h5lt.makeDataset(group.id, 'Two Rank', buffer, {type: H5Type.H5T_NATIVE_DOUBLE, rank: 2, rows: 3, columns: 2});
```

is 3 by 2 dataset.  To read from h5 the dataset can still transfer to a
javascript array or with the method readDatasetAsBuffer the return is a nodejs
Buffer with the shape properties set.

```javascript
var readBuffer = h5lt.readDataset(group.id, 'Two Rank', function(options){
                options.rank; //a synchronous return of the options properties
                options.rows;
            });
readBuffer.constructor.name.should.match('Float64Array');

var readAsBuffer = h5lt.readDatasetAsBuffer(group.id, 'Two Rank');
```
