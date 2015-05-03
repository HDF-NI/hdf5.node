---
layout: page
title: "Events in Packets"
category: tut
date: 2015-05-02 20:29:43
---
{% include anchors.html %}
###Packet Tables

This one is experimental and can only do variable length strings today(and I had a need for it)

```javascript
var table=new h5pt.PacketTable(0, 5);
table.record=new Object();
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:10:44 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.4";
table.record[ "Units" ]="Celcius";
h5pt.makeTable(group.id, 'Events', table);
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:20:45 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.3";
table.record[ "Units" ]="Celcius";
table.append();
table.record[ "Set" ]="Single Point";
table.record[ "Date Time" ]="Mon Nov 24 13:20:46 2014";
table.record[ "Name" ]="Temperature";
table.record[ "Value" ]="37.5";
table.record[ "Units" ]="Celcius";
table.append();
table.close();
```

after the initial make the representative table object can do further appends.  The close is important for the h5.
To read, the next method refills the record with the current packet's data and is ready for the subsequent packet until it returns a false.

```javascript
var table=h5pt.readTable(groupTarget.id, "Events");
for (var name in table.record) {
    console.dir(name);
}
while(table.next()){
    console.dir(table.record[ "Set" ]+" "+table.record[ "Date Time" ]+" "+table.record[ "Name" ]+" "+table.record[ "Value" ]+" "+table.record[ "Units" ]);
}
table.close();
```


