'use strict';
require("should");

const hdf5Lib       = require('..');
const globs         = require('../lib/globals');

const hdf5          = hdf5Lib.hdf5;
const h5tb          = hdf5Lib.h5tb;
const Access        = globs.Access;
const CreationOrder = globs.CreationOrder;
const HLType        = globs.HLType;

describe("testing table interface ",function(){

    describe("create an h5, group and some tables ",function(){
        // open hdf file
        let file;
        let group;
        before(function*() {
            file  = new hdf5.File('./h5tb.h5', Access.ACC_TRUNC);
            group = file.createGroup('pmc/refinement');
        });

        it("should be Table io ", function*() {
            group.id.should.not.equal(-1);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(5);
            fieldArray1.name="Index";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(5);
            fieldArray2.name="Count Up";
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            fieldArray2[2]=3.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=5.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(5);
            fieldArray3.name="Count Down";
            fieldArray3[0]=5.0;
            fieldArray3[1]=4.0;
            fieldArray3[2]=3.0;
            fieldArray3[3]=2.0;
            fieldArray3[4]=1.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(5);
            fieldArray4.name="Residues";
            fieldArray4[0]="ALA";
            fieldArray4[1]="VAL";
            fieldArray4[2]="HIS";
            fieldArray4[3]="LEU";
            fieldArray4[4]="HOH";
            tableModel[3]=fieldArray4;
            h5tb.makeTable(group.id, 'Reflections', tableModel);
        });

        it("should append to Table ", function*() {
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=5;
            fieldArray1[1]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=6.0;
            fieldArray2[1]=7.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=7.0;
            fieldArray3[1]=6.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ARG";
            fieldArray4[1]="GLY";
            tableModel[3]=fieldArray4;
            h5tb.appendRecords(group.id, 'Reflections', tableModel);
            const info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            const fieldInfo=h5tb.getFieldInfo(group.id, 'Reflections');
            fieldInfo.length.should.equal(4);
            fieldInfo[3].should.equal("Residues");
        });

        it("should overwrite records in Table ", function*() {
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=3;
            fieldArray1[1]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=8.0;
            fieldArray2[1]=9.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=9.0;
            fieldArray3[1]=7.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ASN";
            fieldArray4[1]="GLN";
            tableModel[3]=fieldArray4;
            h5tb.writeRecords(group.id, 'Reflections', 3, tableModel);
            const info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
        });

        it("should close pmc/refinement ", function*() {
            group.close();
        });
    });

    describe("should read table", function() {
        let file;
        before(function*() {
          file = new hdf5.File('./h5tb.h5', Access.ACC_RDONLY);
        });

        it("should be Table input ", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            let table=h5tb.readTable(groupTarget.id, "Reflections");
            table.length.should.equal(4);
            table[0].length.should.equal(7);
            table=h5tb.readRecords(groupTarget.id, "Reflections", 3, 2);
            table.length.should.equal(4);
            table[0].length.should.equal(2);
            groupTarget.close();
        });

        it("should delete records from Table ", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            h5tb.deleteRecord(groupTarget.id, "Reflections", 3, 2);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(5);
            groupTarget.close();
        });

        it("should insert records into Table ", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=3;
            fieldArray1[1]=4;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=8.0;
            fieldArray2[1]=9.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=9.0;
            fieldArray3[1]=7.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ASN";
            fieldArray4[1]="GLN";
            tableModel[3]=fieldArray4;
            h5tb.insertRecord(groupTarget.id, "Reflections", 3, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should overwrite fields in Table ", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(2);
            const fieldArray1=new Uint32Array(7);
            fieldArray1.name="Index";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(7);
            fieldArray2.name="Count Down";
            fieldArray2[0]=7.0;
            fieldArray2[1]=6.0;
            fieldArray2[2]=5.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=3.0;
            fieldArray2[5]=2.0;
            fieldArray2[6]=1.0;
            tableModel[1]=fieldArray2;
            h5tb.writeFieldsName(groupTarget.id, "Reflections", 0, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should overwrite fields in Table by index ", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(2);
            const fieldArray1=new Uint32Array(7);
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(7);
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            fieldArray2[2]=3.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=5.0;
            fieldArray2[5]=6.0;
            fieldArray2[6]=7.0;
            tableModel[1]=fieldArray2;
            h5tb.writeFieldsIndex(groupTarget.id, "Reflections", 0, tableModel, [0, 1]);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should read fields in Table", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const table=h5tb.readFieldsName(groupTarget.id, "Reflections", 1, 5, ["Index", "Residues"]);
            table[1][0].should.equal("VAL");
            table[1][1].should.equal("HIS");
            table[1][2].should.equal("ASN");
            table[1][3].should.equal("GLN");
            table[1][4].should.equal("ARG");
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should read fields in Table by index", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const table=h5tb.readFieldsIndex(groupTarget.id, "Reflections", 0, 7, [0, 2]);
            table[1][0].should.equal(7.0);
            table[1][1].should.equal(6.0);
            table[1][2].should.equal(5.0);
            table[1][3].should.equal(4.0);
            table[1][4].should.equal(3.0);
            table[1][5].should.equal(2.0);
            table[1][6].should.equal(1.0);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should delete field in Table", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            h5tb.deleteField(groupTarget.id, "Reflections", "Index");
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(3);
            info.nrecords.should.equal(7);
            groupTarget.close();
        });

        it("should insert field in Table", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(1);
            const fieldArray1=new Uint32Array(7);
            fieldArray1.name="Indices";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            fieldArray1[5]=5;
            fieldArray1[6]=6;
            tableModel[0]=fieldArray1;
            h5tb.insertField(groupTarget.id, "Reflections", 0, tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            groupTarget.flush();
            groupTarget.close();
        });

        it("should add records from Table", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const tableModel=new Array(4);
            const fieldArray1=new Uint32Array(2);
            fieldArray1.name="Indices";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            tableModel[0]=fieldArray1;
            const fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            tableModel[1]=fieldArray2;
            const fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=5.0;
            fieldArray3[1]=4.0;
            tableModel[2]=fieldArray3;
            const fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ALA";
            fieldArray4[1]="VAL";
            tableModel[3]=fieldArray4;
            h5tb.makeTable(groupTarget.id, 'More Reflections', tableModel);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            h5tb.addRecordsFrom(groupTarget.id, "Reflections", 0, info.nrecords, "More Reflections", 2);
            const info2=h5tb.getTableInfo(groupTarget.id, 'More Reflections');
            info2.nfields.should.equal(4);
            info2.nrecords.should.equal(9);
            groupTarget.close();
        });

        it("should combine Tables", function*() {
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
            const info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            h5tb.combineTables(groupTarget.id, "Reflections", groupTarget.id, "More Reflections", "Stack");
            const info2=h5tb.getTableInfo(groupTarget.id, 'Stack');
            info2.nfields.should.equal(4);
            info2.nrecords.should.equal(16);
            groupTarget.close();
        });

        after(function*() {
            file.close();
        });
    });

});

