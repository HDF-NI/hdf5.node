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
        before(function(){
          file = new hdf5.File('./h5tb.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be Table io ", function(done){
            group=file.createGroup();
            group.id.should.equal(-1);
            group.create('/pmc/refinement', file);
            group.id.should.not.equal(-1);
            var tableModel="";
            h5tb.makeTable(group.id, 'Reflections', tableModel);
            done();
        });
        it("should close pmc/refinement ", function(done){
            group.close();
            done();
        });
    });
    
});

