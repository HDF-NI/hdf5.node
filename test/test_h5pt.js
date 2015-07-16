var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('../index.js').hdf5;
var h5pt = require('../index.js').h5pt;

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
          file = new hdf5.File('./h5pt.h5', Access.ACC_TRUNC);
        });
        it("should be Table io ", function*(){
            try
            {
            var group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            var table=new h5pt.PacketTable(0, 5);
            table.record=new Object();
            table.record[ "Set" ]="Single Point";
            table.record[ "Date Time" ]="Mon Nov 24 13:10:44 2014";
            table.record[ "Name" ]="Temperature";
            table.record[ "Value" ]="37.4";
            table.record[ "Units" ]="Celcius";
            h5pt.makeTable(group.id, 'Events', table);
            table.record[ "Set" ]="Single Point";
            table.record[ "Date Time" ]="Mon Nov 24 13:20:44 2014";
            table.record[ "Name" ]="Temperature";
            table.record[ "Value" ]="37.3";
            table.record[ "Units" ]="Celcius";
            table.append();
            table.record[ "Set" ]="Single Point";
            table.record[ "Date Time" ]="Mon Nov 24 13:20:44 2014";
            table.record[ "Name" ]="Temperature";
            table.record[ "Value" ]="37.5";
            table.record[ "Units" ]="Celcius";
            table.append();
            table.close();
            group.close();
            }
            catch (err) {
            console.dir(err.message);
            }
        });
        it("should close pmc/refinement ", function*(){
            file.close();
        });
    });
    
    describe("should read table", function() {
        var file;
        before(function*(){
//          file = new hdf5.File('/home/roger/testing-grounds/Oleg/3FVA/roothaan.h5', Access.ACC_RDONLY);
          file = new hdf5.File('./h5pt.h5', Access.ACC_RDONLY);
        });
        
        it("should be Table input ", function*(){
            try
            {
//                var groupTarget=file.openGroup('DivCon/3FVA', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                var groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
                groupTarget.getDatasetType("Events").should.equal(HLType.HL_TYPE_PACKET_TABLE);
                console.dir("Events "+groupTarget.getDatasetType("Events"));
                var table=h5pt.readTable(groupTarget.id, "Events");
                for (var name in table.record) {
                    console.dir(name);
                }
                while(table.next()){
                    console.dir(table.record[ "Set" ]+" "+table.record[ "Date Time" ]+" "+table.record[ "Name" ]+" "+table.record[ "Value" ]+" "+table.record[ "Units" ]);
                }
                table.close();
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

