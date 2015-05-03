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
* name &rarr; The packet name
* table &rarr; The packet table with its first packet

h5pt.**readTable**(id, name)

* id &rarr; {{ site.hdf5_id }}
* name &rarr; The packet name
* return &larr; The packet table loaded with first record

h5pt.PacketTable.**record**{{: #packet-record}}


h5pt.PacketTable.prototype.**append**()
> Appends a packet and increments to be ready for appending the next one. If only some attributes
>  are changing the others go along and stay valid.  Update the [h5pt.PacketTable.**record**](#packet-record) between
> appends.

h5pt.PacketTable.prototype.**next**()
> Reads a packet and increments to be ready for the next one. Read attribute values of interest from
>  the h5pt.PacketTable.**record**
>
> *  return &larr; true if there is another record loaded or false if at end of table


h5pt.PacketTable.prototype.**close**()
> Closes the table and the id is no longer vaild Important for eliminating resource leaks. No
> javascript garbage collection has worked safely so this is necessary.

