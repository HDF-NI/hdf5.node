A node module for reading/writing the HDF5 file format. The koa based browser interface is a reference app for viewing, modifying and looking at h5 content. Eventually it will provide for editing, charting and performing statistics on h5 file data.  
The interface is now showing images, datasets, column tables and string based packet tables. Basic group operations are becoming available with right-click on a node.  Hovering on a group shows attributes in a tooltip however stil looking for 
a good mechanism to add, copy and edit attributes.  Images can be dropped on the main panel after selecting a group and will be stored at the equivalent place in the h5.  Learning how to accomplish more HDFView functionality in browser style.  
Experimenting with https://ethercalc.net/ as an editor; there is an upper limit to practical data going from h5 into a spreadsheet and other mechanism may need to be provided.  It has some charting yet that may need addressed with a d3 or threejs approach.

Unlike other languages that wrap hdf5 API's this interface takes advantage of the compatibility of V8 and HDF5. The result 
is a direct map to javascript behavior with the least amount of data copying and coding tasks for the user. Hopefully you 
won't need to write yet another layer in your code to accomplish your goals.

The node::Buffer and streams are being investigated so native hdf5 data's only destination is client browser window or client in general.

API documentation is available at http://ryancole.github.io/hdf5.node

```bash
npm install hdf5
```
http://ryancole.github.io/hdf5.node/doc/install-setup.html for te native requirements and details.

```javascript
var hdf5 = require('hdf5').hdf5;

var Access = require('hdf5/lib/globals').Access;
var file = new hdf5.File('/tmp/foo.h5', Access.ACC_RDONLY);
var group=file.openGroup('pmc');
```


### Dimension Scales

to be implemented

### High-level Functions for Region References, Hyperslabs, and Bit-fields

Looking at this.  It does require a separate download and install because it is not a part of the standard hdf5 distribution.


## Experimental

The h5im namespace is being designed to meet the Image Spec 1.2 http://www.hdfgroup.org/HDF5/doc/ADGuide/ImageSpec.html


Any ideas for the design of the API and interface are welcome.
