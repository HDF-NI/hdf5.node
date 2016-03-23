---
layout: default
title: "HDF5 Docs"
---

## Get Started

```bash
npm install hdf5 --fallback-to-build
```
 
Because this project has native code look at [Install & Setup]({{ site.baseurl }}/doc/install-setup.html) for the details.


To start a new h5 file and populate it with groups and data:

```bash
node --harmony  ./lib/application.js 3000 "`pwd`/new.h5"
```  

Browse <http://localhost:3000>. If the h5 already exists you can view and add 
to it.

On the tree view right click to use context menu.  Hover over a group to see 
its attributes. Select a group and then drop content on the editor panel
