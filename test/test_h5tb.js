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
            group=file.createGroup();
            group.id.should.equal(-1);
            group.create('/pmc/refinement', file);
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
            console.dir("open target...");
            var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            console.dir("read Reflections table...");
            var table=h5tb.readTable(groupTarget.id, "Reflections");
            console.dir(table);
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

