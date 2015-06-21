---
layout: page
title: "Writing & Reading Subsets"
category: tut
date: 2015-06-20 16:02:53
---

###Rank 2 Subsets

This nodejs interface follows the hdf5 example at https://www.hdfgroup.org/HDF5/Tutor/selectsimple.html

If you don't already have a dataset, start by creating one.

```javascript
var hdf5 = require('hdf5').hdf5;
var h5lt = require('hdf5').h5lt;

var Access = require('hdf5/lib/globals').Access;

var file = new hdf5.File('./roothaan.h5', Access.ACC_TRUNC);
var group=file.createGroup('pmcservices');
var buffer=new Buffer(8*10*8, "binary");
buffer.rank=2;
buffer.rows=8;
buffer.columns=10;
buffer.type=H5Type.H5T_NATIVE_DOUBLE;
for (j = 0; j < buffer.columns; j++) {
	for (i = 0; i < buffer.rows; i++){
            if (j< (buffer.columns/2))
                buffer.writeDoubleLE(1.0, 8*(i*buffer.columns+j));
            else
               buffer.writeDoubleLE(2.0, 8*(i*buffer.columns+j));
    }
}

h5lt.makeDataset(group.id, 'Waldo', buffer);
```

View your h5 file and there should be a dataset with data looking like:
<table style="border-collapse: separate; border-spacing: 7px;">
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
</table>

To write a subset use the options on the writeDataset method.

```javascript
var subsetBuffer=new Buffer(3*4*8, "binary");
subsetBuffer.rank=2;
subsetBuffer.rows=3;
subsetBuffer.columns=4;
subsetBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
for (j = 0; j < subsetBuffer.columns; j++) {
	for (i = 0; i < subsetBuffer.rows; i++){
                subsetBuffer.writeDoubleLE(5.0, 8*(i*subsetBuffer.columns+j));
    }
}

h5lt.writeDataset(group.id, 'Waldo', subsetBuffer, {start: [1,2], stride: [1,1], count: [3,4]});
```

The subset of 5.0's is down a row and over 2 columns and 4 wide and 3 in height.

<table style="border-collapse: separate; border-spacing: 7px;">
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td style="font-weight: bold; color: red">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
</table>

To read back a different subset use the readDatasetAsBuffer with the shape defined in the options:

```javascript
var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Waldo', {start: [3,4], stride: [1,1], count: [2,2]});
```

The returned nodejs Buffer subset is down 4 row and over 5 columns and 2 wide and 2 in height.  The values should be 5.0, 5.0, 1.0 and 2.0.

<table style="border-collapse: separate; border-spacing: 7px;">
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style=" color: red">5.0</td><td style="color: red">5.0</td><td style="color: red">5.0</td><td style="color: red">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style="color: red">5.0</td><td style="color: red">5.0</td><td style="color: red">5.0</td><td style="fcolor: red">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td style="color: red">5.0</td><td style="color: red">5.0</td><td style="font-weight: bold; color: green">5.0</td><td style="font-weight: bold; color: green">5.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td style="font-weight: bold; color: green">1.0</td><td style="font-weight: bold; color: green">2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
    <tr>
        <td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>1.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td><td>2.0</td>
    </tr>
</table>

There is currently only one operator that overwrites the subset data in h5. Remember to close your group to flush the memory to file.


