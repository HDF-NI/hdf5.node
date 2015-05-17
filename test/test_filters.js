var assert = require("assert");
//var should = require("should");
var fs = require('fs');
var parseString = require('xml2js').parseString;
var util = require('util');

var hdf5 = require('../index.js').hdf5;
var h5lt = require('../index.js').h5lt;

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var State = require('lib/globals.js').State;
var H5OType = require('lib/globals.js').H5OType;
var HLType = require('lib/globals').HLType;
var H5Type = require('lib/globals.js').H5Type;
var H5ZType = require('lib/globals.js').H5ZType;

describe("testing fiter interface ",function(){

    describe("create an h5, group and some compressed datasets ",function(){
        // open hdf file
        var file;
        before(function*(){
          file = new hdf5.File('./filters.h5', Access.ACC_TRUNC);
        });
        it("should set filter ", function (){
            try
            {
            var group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            var buffer=new Buffer(5000*8, "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for(var index=0;index<5000;index++){
                buffer.writeDoubleLE((index % 1000), index*8);
            }
            h5lt.makeDataset(group.id, 'Data', buffer, {compression: 7});
            
                var filters=group.getFilters('Data');
                console.dir(filters.isAvailable(H5ZType.H5Z_FILTER_DEFLATE));
                console.dir("32004 "+ filters.isAvailable(32004));
                console.dir("n filters "+filters.getNFilters());
                var readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Data');
                console.dir("buf length "+readAsBuffer.length);
                readAsBuffer.length.should.equal(40000);
                readAsBuffer.rows.should.equal(5000);
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
    
    describe("should read compressed dataset", function() {
        var file;
        before(function*(){
//          file = new hdf5.File('/home/roger/testing-grounds/Oleg/3FVA/roothaan.h5', Access.ACC_RDONLY);
          file = new hdf5.File('./filters.h5', Access.ACC_RDONLY);
        });
        
        it("should be compression filter info ", function (){
            try
            {
                var group=file.openGroup('pmc/refinement');
                var filters=group.getFilters('Data');
                console.dir(filters.isAvailable(H5ZType.H5Z_FILTER_DEFLATE));
                console.dir("n filters "+filters.getNFilters());
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

