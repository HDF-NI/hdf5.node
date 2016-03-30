[![NPM Version][npm-image]][npm-url]
[![NPM Downloads][downloads-image]][npm-url]
[![Node.js Version][node-version-image]][node-version-url]


HDF5 is a node module for reading and writing the 
[HDF5](https://www.hdfgroup.org/HDF5/) file format.

## Documentation

API documentation is available at <http://hdf-ni.github.io/hdf5.node>

```bash
npm install hdf5 --fallback-to-build
```
See <http://hdf-ni.github.io/hdf5.node/doc/install-setup.html> for the native requirements and details.

```javascript
var hdf5 = require('hdf5').hdf5;

var Access = require('hdf5/lib/globals').Access;
var file = new hdf5.File('/tmp/foo.h5', Access.ACC_RDONLY);
var group = file.openGroup('pmc');
```

### Notes on Recent Releases

* Note: Release v0.1.0 was built with nodejs v4.2.1. If you want nodejs v0.12.x then
stay with  release v0.0.20. npm will continue with nodejs v4.x.x line and any
fixes or features needed by prior versions will be from github branches.

* Note: Release v0.0.20 is for prebuilts with hdf5-1.8.15-patch1. If you want
hdf5-1.8.14 stay with v0.0.19.

## Philosophy

Unlike other languages that wrap hdf5 API's this interface takes advantage of
the compatibility of V8 and HDF5. The result is a direct map to javascript
behavior with the least amount of data copying and coding tasks for the user.
Hopefully you won't need to write yet another layer in your code to accomplish
your goals.


## Other Feature Notes


The node::Buffer and streams are being investigated so native hdf5 data's only destination is client browser window or client in general.


### Dimension Scales

Mostly implemented (missing H5DSiterate_scales[ found a way to make callback functions from te native side and looking to finish this and use the technique for other h5 iterators])

### High-level Functions for Region References, Hyperslabs, and Bit-fields

Writing an interface based on the standard hdf5 library.  Currently you can write and read a subset from a two rank dataset. Other ranks may work yet are untested.
See tutorial <http://hdf-ni.github.io/hdf5.node/tut/subset_tutorial.html> for example applied to node Buffers.

### Filters and Compression

Testing filters and compression.  Have the gzip filter working. For some applications getting the uncompressed data from the h5 would reduce the number of 
compressions and decompressions.  For example an image could be sent to client before unzipping and rezipping on the server side.  

Third party filters can be used.  Those do take separate compiled libraries yet are independent. They get picked up by native 
hdf5 from the HDF5_PLUGIN_PATH.

### Koa Based Browser Interface

The koa based browser interface is a reference app for viewing, modifying and looking at h5 content from browsers. Eventually it will provide for editing, charting and performing statistics on h5 file data.  
The interface is now showing images, datasets, text, column tables and string based packet tables. Basic group operations are available with right-click on a node.  Hovering on a group or dataset shows attributes in a tooltip however still looking for 
a good mechanism to add, copy and edit attributes.  Images can be dropped on the main panel after selecting a group and will be stored at the equivalent place in the h5.  Learning how to accomplish more HDFView functionality in browser style.  
Experimenting with https://ethercalc.net/ as an editor; there is an upper limit to practical data going from h5 into a spreadsheet and other mechanism may need to be provided.  It has some charting yet that may need addressed with a d3 or threejs approach.

Testing addons for custom views/editing or multiviews of the same data.  Implementation of charting based on d3 working for tables. Next need to work on selection/patterns for which tables to map to which custom view.


## Experimental

The h5im namespace is being designed to meet the Image Spec 1.2 <http://www.hdfgroup.org/HDF5/doc/ADGuide/ImageSpec.html>.

Any ideas for the design of the API and interface are welcome.

[npm-image]: https://badge.fury.io/js/hdf5.svg
[npm-url]: https://www.npmjs.com/package/hdf5
[downloads-image]: https://img.shields.io/npm/dm/hdf5.svg
[node-version-image]: https://img.shields.io/node/v/hdf5.svg
[node-version-url]: https://nodejs.org/en/download/
