---
layout: page
title: "References"
category: ref
date: 2018-07-21 15:26:30
---

```javascript
var hdf5 = require('hdf5').hdf5;
var h5lt = require('hdf5').h5lt;
```

h5lt.Reference.id, xpath, type);

> Constructs a reference purely on the javascript side from it's native registration.
>
> *  id &rarr; file or group id.
> *  xpath &rarr; xpath like path and name
> *  type &rarr; a type from the H5RType enumeration.  Currently [H5RType]({{ site.baseurl }}/ref/globals.html#h5rtype).H5R_OBJECT.
> *  return &larr; a constructed Reference function

Reference.prototype.**getName**(id, type)

> *  id &rarr; file or group id.
> *  type &rarr; a type from the H5RType enumeration.  Currently [H5RType]({{ site.baseurl }}/ref/globals.html#h5rtype).H5R_OBJECT.
> *  return &larr; xpath like path and name which can be used to open the referernced dataset
