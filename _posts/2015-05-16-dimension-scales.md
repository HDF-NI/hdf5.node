---
layout: page
title: "Dimension Scales"
category: ref
date: 2015-05-16 18:10:49
---


```javascript
var h5ds = require('hdf5').h5ds;
```

#### Functions

h5ds.**setScale**(id, name, dimname)  

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  name &rarr; dataset name - the one to become a dimension scale
> *  dimname &rarr; a dimension scale name.


h5ds.**isScale**(id, name)  

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  name &rarr; dataset name - the one to become a dimension scale
> *  return &larr; true if it is a dimension scale.

h5ds.**attachScale**(id, dataset name, name, index)  

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  dataset name &rarr; dataset name to attach this scale.
> *  name &rarr; dataset name - the dataset name of the dimension scale
> *  index &rarr; the selected axis or rank.

h5ds.**isAttached**(id, dataset name, name, index)  

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  dataset name &rarr; dataset name to check for attachement.
> *  name &rarr; dataset name - the dataset name of the dimension scale
> *  index &rarr; the selected axis or rank for checking.
> *  return &larr; true if it is attached to dataset.


h5ds.**getNumberOfScales**(id, dataset name, index)

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  dataset name &rarr; dataset name to check.
> *  index &rarr; the selected axis or rank for checking.
> *  return &larr; # of scales on particular axis or rank.


h5ds.**getScaleName**(id, name)

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  name &rarr; dataset name.
> *  return &larr; dimension scale name.

h5ds.**getLabel**(id, name, index)

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  name &rarr; dataset name.
> *  index &rarr; the selected axis or rank.
> *  return &larr; the label.

h5ds.**setLabel**(id, name, index, label);

> 
> *  id &rarr; {{ site.hdf5_id }}
> *  name &rarr; dataset name.
> *  index &rarr; the selected axis or rank.
> *  label &rarr; what to call it.
