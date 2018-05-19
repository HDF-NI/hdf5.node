---
layout: page
title: "Working with Images"
category: tut
date: 2015-05-02 20:18:51
---

### Images (1.2)

Currently the image subclass is "IMAGE_TRUECOLOR".  The rest of the spec[http://www.hdfgroup.org/HDF5/doc/ADGuide/ImageSpec.html is coming.  Need test examples.
Image data sent to and from html5's canvas 2d context putImageData and getImageData are compatible with IMAGE_TRUECOLOR" with 4 planes and pixel interlacing.
At this point I experimented with binaryjs for data transfer to html5 canvas yet underlying mechanism can do binary.  However this provided streams that worked 
and handles node::Buffer's. Although eventually streaming from the native side may replace this.  The image data is handled without any particular image processing 
library server side so you can choose which one you prefer depending on application. For the html5 interface the image format is handled by the canvas and 
window.URL.createObjectURL(file) in the drop zone.

```javascript
var h5im = require('hdf5').h5im;
var Interlace = require('hdf5/lib/globals').Interlace;

var buffer=h5im.readImage(parent.id, name);

//buffer.width, buffer.height, buffer.planes

var image = Buffer.concat(buffers);
h5im.makeImage(group.id, name, image, {interlace: Interlace.INTERLACE_PIXEL, planes: 4, width: meta.width, height: meta.height});
```



