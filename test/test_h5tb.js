var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('../index.js').hdf5;
var h5tb = require('../index.js').h5tb;

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var State = require('lib/globals.js').State;
var H5OType = require('lib/globals.js').H5OType;
var HLType = require('lib/globals').HLType;


describe("testing table interface ",function(){

    describe("create an h5, group and some tables ",function(){
        // open hdf file
        var file;
        before(function*(){
          file = new hdf5.File('./h5tb.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be Table io ", function*(){
            try
            {
            group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            var tableModel=new Array(4);
            var fieldArray1=new Uint32Array(5);
            fieldArray1.name="Index";
            fieldArray1[0]=0;
            fieldArray1[1]=1;
            fieldArray1[2]=2;
            fieldArray1[3]=3;
            fieldArray1[4]=4;
            tableModel[0]=fieldArray1;
            var fieldArray2=new Float64Array(5);
            fieldArray2.name="Count Up";
            fieldArray2[0]=1.0;
            fieldArray2[1]=2.0;
            fieldArray2[2]=3.0;
            fieldArray2[3]=4.0;
            fieldArray2[4]=5.0;
            tableModel[1]=fieldArray2;
            var fieldArray3=new Float64Array(5);
            fieldArray3.name="Count Down";
            fieldArray3[0]=5.0;
            fieldArray3[1]=4.0;
            fieldArray3[2]=3.0;
            fieldArray3[3]=2.0;
            fieldArray3[4]=1.0;
            tableModel[2]=fieldArray3;
            var fieldArray4=new Array(5);
            fieldArray4.name="Residues";
            fieldArray4[0]="ALA";
            fieldArray4[1]="VAL";
            fieldArray4[2]="HIS";
            fieldArray4[3]="LEU";
            fieldArray4[4]="HOH";
            tableModel[3]=fieldArray4;
            h5tb.makeTable(group.id, 'Reflections', tableModel);
            
            }
            catch (err) {
            console.dir(err.message);
            }
        });
        it("should append to Table ", function*(){
            try
            {
            var tableModel=new Array(4);
            var fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=5;
            fieldArray1[1]=6;
            tableModel[0]=fieldArray1;
            var fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=6.0;
            fieldArray2[1]=7.0;
            tableModel[1]=fieldArray2;
            var fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=7.0;
            fieldArray3[1]=6.0;
            tableModel[2]=fieldArray3;
            var fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ARG";
            fieldArray4[1]="GLY";
            tableModel[3]=fieldArray4;
            h5tb.appendRecords(group.id, 'Reflections', tableModel);
            var info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            var fieldInfo=h5tb.getFieldInfo(group.id, 'Reflections');
            fieldInfo.length.should.equal(4);
            fieldInfo[3].should.equal("Residues");
            
            }
            catch (err) {
            console.dir(err.message);
            }
        });
        it("should overwrite records in Table ", function*(){
            try
            {
            var tableModel=new Array(4);
            var fieldArray1=new Uint32Array(2);
            fieldArray1.name="Index";
            fieldArray1[0]=3;
            fieldArray1[1]=4;
            tableModel[0]=fieldArray1;
            var fieldArray2=new Float64Array(2);
            fieldArray2.name="Count Up";
            fieldArray2[0]=8.0;
            fieldArray2[1]=9.0;
            tableModel[1]=fieldArray2;
            var fieldArray3=new Float64Array(2);
            fieldArray3.name="Count Down";
            fieldArray3[0]=9.0;
            fieldArray3[1]=7.0;
            tableModel[2]=fieldArray3;
            var fieldArray4=new Array(2);
            fieldArray4.name="Residues";
            fieldArray4[0]="ASN";
            fieldArray4[1]="GLN";
            tableModel[3]=fieldArray4;
            h5tb.writeRecords(group.id, 'Reflections', 3, tableModel);
            var info=h5tb.getTableInfo(group.id, 'Reflections');
            info.nfields.should.equal(4);
            info.nrecords.should.equal(7);
            
            }
            catch (err) {
            console.dir(err.message);
            }
        });
        it("should close pmc/refinement ", function*(){
            group.close();
        });
    });
    
    describe("should read table", function() {
        var file;
        before(function*(){
          file = new hdf5.File('./h5tb.h5', Access.ACC_RDONLY);
        });
        
        it("should be Table input ", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var table=h5tb.readTable(groupTarget.id, "Reflections");
                table.length.should.equal(4);
                table[0].length.should.equal(7);
                console.dir(table);
                table=h5tb.readRecords(groupTarget.id, "Reflections", 3, 2);
                table.length.should.equal(4);
                table[0].length.should.equal(2);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should delete records from Table ", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                console.dir(groupTarget.id);
                h5tb.deleteRecord(groupTarget.id, "Reflections", 3, 2);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(5);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });

        it("should insert records into Table ", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var tableModel=new Array(4);
                var fieldArray1=new Uint32Array(2);
                fieldArray1.name="Index";
                fieldArray1[0]=3;
                fieldArray1[1]=4;
                tableModel[0]=fieldArray1;
                var fieldArray2=new Float64Array(2);
                fieldArray2.name="Count Up";
                fieldArray2[0]=8.0;
                fieldArray2[1]=9.0;
                tableModel[1]=fieldArray2;
                var fieldArray3=new Float64Array(2);
                fieldArray3.name="Count Down";
                fieldArray3[0]=9.0;
                fieldArray3[1]=7.0;
                tableModel[2]=fieldArray3;
                var fieldArray4=new Array(2);
                fieldArray4.name="Residues";
                fieldArray4[0]="ASN";
                fieldArray4[1]="GLN";
                tableModel[3]=fieldArray4;
                h5tb.insertRecord(groupTarget.id, "Reflections", 3, tableModel);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should overwrite fields in Table ", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var tableModel=new Array(2);
                var fieldArray1=new Uint32Array(7);
                fieldArray1.name="Index";
                fieldArray1[0]=0;
                fieldArray1[1]=1;
                fieldArray1[2]=2;
                fieldArray1[3]=3;
                fieldArray1[4]=4;
                fieldArray1[5]=5;
                fieldArray1[6]=6;
                tableModel[0]=fieldArray1;
                var fieldArray2=new Float64Array(7);
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
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should overwrite fields in Table by index ", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var tableModel=new Array(2);
                var fieldArray1=new Uint32Array(7);
                fieldArray1[0]=0;
                fieldArray1[1]=1;
                fieldArray1[2]=2;
                fieldArray1[3]=3;
                fieldArray1[4]=4;
                fieldArray1[5]=5;
                fieldArray1[6]=6;
                tableModel[0]=fieldArray1;
                var fieldArray2=new Float64Array(7);
                fieldArray2[0]=1.0;
                fieldArray2[1]=2.0;
                fieldArray2[2]=3.0;
                fieldArray2[3]=4.0;
                fieldArray2[4]=5.0;
                fieldArray2[5]=6.0;
                fieldArray2[6]=7.0;
                tableModel[1]=fieldArray2;
                h5tb.writeFieldsIndex(groupTarget.id, "Reflections", 0, tableModel, [0, 1]);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should read fields in Table", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var table=h5tb.readFieldsName(groupTarget.id, "Reflections", 1, 5, ["Index", "Residues"]);
                table[1][0].should.equal("VAL");
                table[1][1].should.equal("HIS");
                table[1][2].should.equal("ASN");
                table[1][3].should.equal("GLN");
                table[1][4].should.equal("ARG");
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should read fields in Table by index", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var table=h5tb.readFieldsIndex(groupTarget.id, "Reflections", 0, 7, [0, 2]);
                table[1][0].should.equal(7.0);
                table[1][1].should.equal(6.0);
                table[1][2].should.equal(5.0);
                table[1][3].should.equal(4.0);
                table[1][4].should.equal(3.0);
                table[1][5].should.equal(2.0);
                table[1][6].should.equal(1.0);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should delete field in Table", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                h5tb.deleteField(groupTarget.id, "Reflections", "Index");
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(3);
                info.nrecords.should.equal(7);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should insert field in Table", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var tableModel=new Array(1);
                var fieldArray1=new Uint32Array(7);
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
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                groupTarget.flush();
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should add records from Table", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var tableModel=new Array(4);
                var fieldArray1=new Uint32Array(2);
                fieldArray1.name="Indices";
                fieldArray1[0]=0;
                fieldArray1[1]=1;
                tableModel[0]=fieldArray1;
                var fieldArray2=new Float64Array(2);
                fieldArray2.name="Count Up";
                fieldArray2[0]=1.0;
                fieldArray2[1]=2.0;
                tableModel[1]=fieldArray2;
                var fieldArray3=new Float64Array(2);
                fieldArray3.name="Count Down";
                fieldArray3[0]=5.0;
                fieldArray3[1]=4.0;
                tableModel[2]=fieldArray3;
                var fieldArray4=new Array(2);
                fieldArray4.name="Residues";
                fieldArray4[0]="ALA";
                fieldArray4[1]="VAL";
                tableModel[3]=fieldArray4;
                h5tb.makeTable(groupTarget.id, 'More Reflections', tableModel);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                h5tb.addRecordsFrom(groupTarget.id, "Reflections", 0, info.nrecords, "More Reflections", 2);
                var info2=h5tb.getTableInfo(groupTarget.id, 'More Reflections');
                info2.nfields.should.equal(4);
                info2.nrecords.should.equal(9);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        it("should combine Tables", function*(){
            try
            {
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Reflections").should.equal(HLType.HL_TYPE_TABLE);
                var info=h5tb.getTableInfo(groupTarget.id, 'Reflections');
                info.nfields.should.equal(4);
                info.nrecords.should.equal(7);
                h5tb.combineTables(groupTarget.id, "Reflections", groupTarget.id, "More Reflections", "Stack");
                var info2=h5tb.getTableInfo(groupTarget.id, 'Stack');
                info2.nfields.should.equal(4);
                info2.nrecords.should.equal(16);
                groupTarget.close();
            }
            catch (err) {
            console.dir(err.message);
            }
            
        });
        after(function*(){
            file.close();
        });
    });

});

