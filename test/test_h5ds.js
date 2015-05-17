var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('../index.js').hdf5;
var h5lt = require('../index.js').h5lt;
var h5ds = require('../index.js').h5ds;

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var State = require('lib/globals.js').State;
var H5OType = require('lib/globals.js').H5OType;
var HLType = require('lib/globals').HLType;
var H5Type = require('lib/globals.js').H5Type;

describe("testing dimension scale interface ",function(){

    describe("create an h5, group and some dimension scales ",function(){
        // open hdf file
        var file;
        before(function*(){
          file = new hdf5.File('./h5ds.h5', Access.ACC_TRUNC);
        });
        it("should set scale ", function (){
            try
            {
            var group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            var buffer=new Buffer(4096*4, "binary");
            buffer.type=H5Type.H5T_NATIVE_UINT;
            buffer.rank=2;
            buffer.rows=64;
            buffer.columns=64;
            for(var index=0;index<4096;index++){
                buffer.writeUInt32LE((index % 1000), index*4);
            }
            h5lt.makeDataset(group.id, 'Data', buffer);
            
            var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Data');
            console.dir("buf length "+readAsBuffer.length);
            readAsBuffer.length.should.equal(16384);
            readAsBuffer.rows.should.equal(64);
                
            var scaleBuffer=new Buffer(64*8, "binary");
            scaleBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for(var index=0;index<64;index++){
                scaleBuffer.writeDoubleLE(index, index*8);
            }
            h5lt.makeDataset(group.id, 'X(dim)', scaleBuffer);
            h5ds.isScale(group.id, 'X(dim)').should.equal(false);
            h5ds.setScale(group.id, 'X(dim)', "The X Scale");
            h5ds.isScale(group.id, 'X(dim)').should.equal(true);
            
            h5ds.isAttached(group.id, 'Data', 'X(dim)', 0).should.equal(false);
            h5ds.attachScale(group.id, 'Data', 'X(dim)', 0);
            h5ds.isAttached(group.id, 'Data', 'X(dim)', 0).should.equal(true);
            
            var scaleYBuffer=new Buffer(64*8, "binary");
            scaleYBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for(var index=0;index<64;index++){
                scaleYBuffer.writeDoubleLE(index-31, index*8);
            }
            h5lt.makeDataset(group.id, 'Y(dim)', scaleYBuffer);
            h5ds.isScale(group.id, 'Y(dim)').should.equal(false);
            h5ds.setScale(group.id, 'Y(dim)', "The Y Scale");
            h5ds.isScale(group.id, 'Y(dim)').should.equal(true);
            
            h5ds.isAttached(group.id, 'Data', 'Y(dim)', 1).should.equal(false);
            h5ds.attachScale(group.id, 'Data', 'Y(dim)', 1);
            h5ds.isAttached(group.id, 'Data', 'Y(dim)', 1).should.equal(true);
            h5ds.getNumberOfScales(group.id, 'Data', 1).should.equal(1);
            h5ds.getScaleName(group.id, 'Y(dim)').should.equal("The Y Scale");
            h5ds.getLabel(group.id, 'Data', 1).should.equal("");
            h5ds.setLabel(group.id, 'Data', 1, "y");
            h5ds.getLabel(group.id, 'Data', 1).should.equal("y");
            group.close();
            }
            catch (err) {
            console.dir(err.message);
            }
        });
        after(function*(){
            file.close();
        });
    });
    
    describe("should read dimension scales ", function() {
        var file;
        before(function*(){
//          file = new hdf5.File('/home/roger/testing-grounds/Oleg/3FVA/roothaan.h5', Access.ACC_RDONLY);
          file = new hdf5.File('./h5ds.h5', Access.ACC_RDONLY);
        });
        
        it("should be dimension scales info ", function (){
            try
            {
                var group=file.openGroup('pmc/refinement');
                group.close();
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

