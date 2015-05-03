---
layout: page
title: "Populate Tables"
category: tut
date: 2015-05-02 20:25:37
---

### Column Tables

```javascript
var table=new Array(4);
var fieldArray1=new Uint32Array(5);
fieldArray1.name="Index";
fieldArray1[0]=0;
fieldArray1[1]=1;
fieldArray1[2]=2;
fieldArray1[3]=3;
fieldArray1[4]=4;
table[0]=fieldArray1;
var fieldArray2=new Float64Array(5);
fieldArray2.name="Count Up";
fieldArray2[0]=1.0;
fieldArray2[1]=2.0;
fieldArray2[2]=3.0;
fieldArray2[3]=4.0;
fieldArray2[4]=5.0;
table[1]=fieldArray2;
var fieldArray3=new Float64Array(5);
fieldArray3.name="Count Down";
fieldArray3[0]=5.0;
fieldArray3[1]=4.0;
fieldArray3[2]=3.0;
fieldArray3[3]=2.0;
fieldArray3[4]=1.0;
table[2]=fieldArray3;
var fieldArray4=new Array(5);
fieldArray4.name="Residues";
fieldArray4[0]="ALA";
fieldArray4[1]="VAL";
fieldArray4[2]="HIS";
fieldArray4[3]="LEU";
fieldArray4[4]="HOH";
table[3]=fieldArray4;
h5tb.makeTable(group.id, 'Reflections', table);
var readTable=h5tb.readTable(group.id, "Reflections");
```

A column of strings is set fixed width to the widest in the set(working on other possible solutions). The return table is equivalent

