---
layout: page
title: "Metadata & Attributes"
category: tut
date: 2015-05-02 20:34:56
---

###Properties as h5 metadata attributes

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

Types are allowed to change to match the javascript side. If an attribute is already there it will get updated by h5's existence check, removed and then added back. 
When opening an h5 the group's attributes can be refreshed to the javascript in a reverse manner

```javascript
var groupTarget=file.openGroup('pmcservices/sodium-icosanoate');
groupTarget.refresh();
console.dir(groupTarget.[ 'Computed Heat Of Formation' ]);
console.dir(groupTarget.Information);
```

