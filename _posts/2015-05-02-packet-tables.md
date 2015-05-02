---
layout: page
title: "Packet Tables"
category: ref
date: 2015-05-02 12:02:10
---
```javascript
var h5pt = require('hdf5').h5pt;
```
h5pt.PacketTable(id, nrecords)

*  id &rarr; the packet tables id; when creating a new packet table this is set to zero and it will get updated automatically
*  nrecords &rarr; # of records per packet
*  return &larr; a constructed PacketTable

h5pt.**makeTable**(id, name, table)

* id &rarr; {{ site.hdf5_id }}
* name &rarr; The palette name

h5pt.**readTable**(id, name)

* id &rarr; {{ site.hdf5_id }}
* name &rarr; The palette name
* return &larr; The packet table loaded with first record

h5pt.PacketTable.**record**


h5pt.PacketTable.prototype.**append**()

h5pt.PacketTable.prototype.**next**()

*  return &larr; true if there is another record loaded or false if at end of table


h5pt.PacketTable.prototype.**close**()

