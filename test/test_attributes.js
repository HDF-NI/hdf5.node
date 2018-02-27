'use strict';

require('co-mocha')(require('mocha'));
require('should');

const hdf5Lib = require('..');
const globs   = require('../lib/globals');

describe("testing attribute interface ",function(){

    describe("create an h5, group and some attributes ", function() {
        // open hdf file
        let file;
        before(function*() {
          file = new hdf5Lib.hdf5.File('./attributes.h5', globs.Access.ACC_TRUNC);
        });

        it("should set filter ", function*() {
            const group = file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            var name=new String("3FVA");
            name.type="variable-length";
            group.name=name;
            group.size=new Float64Array([0.1, 0.1, 0.1]);
            const notes=new Array(3);
            notes[0]="Pick up dry cleaning";
            notes[1]="Prefry the refried beans";
            notes[2]="Remember Mother's Day";
            group.notes=notes;
            group.flush();
            group.close();
        });

        after(function*(){
            file.close();
        });
    });

    describe("should read attributes", function() {
        let file;
        before(function*() {
          file = new hdf5Lib.hdf5.File('./attributes.h5', globs.Access.ACC_RDONLY);
        });

        it("should be attribute info ", function*() {
            const group   = file.openGroup('pmc/refinement');
            group.refresh();
            group.name.should.equal("3FVA");
            group.name.type.should.equal("variable-length");
            group.size.constructor.name.should.match('Float64Array');
            group.notes.constructor.name.should.match('Array');
            // console.dir(group.notes);
            group.close();
        });

        after(function*(){
            file.close();
        });
    });

    describe("should read biom attributes", function() {
        let file;
        before(function*() {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/rich_sparse_otu_table_hdf5.biom', globs.Access.ACC_RDONLY);
        });

        it("should be attribute info ", function*() {
            console.log(file);
            console.log(file.getNumAttrs());
            file.refresh();
            for (var property in file) {
                if (file.hasOwnProperty(property)) {
                    console.log(property+": "+file[property]);
                }
            }            
        });

        after(function*(){
            file.close();
        });
    });

    describe.skip("read variable string attributes", function() {
        let file;
        before(function*() {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/sample.h5', globs.Access.ACC_RDONLY);
        });

        it("should be variable string info", function*() {
            const group   = file.openGroup('scada');
            group.refresh();
            var attrs = group.getDatasetAttributes("active_power");
            var attrText = '';
            Object.getOwnPropertyNames(attrs).forEach(function(val, idx, array) {
//              if (val !=  'id') {
                if (attrs[val].constructor.name === Array) {
                  attrText += val + ' :  ';
                  for (var mIndex = 0; mIndex < attrs[val].Length(); mIndex++) {
                    attrText += attrs[val][mIndex];
                    if (mIndex < attrs[val].Length() - 1) {
                      attrText += ',';
                    }
                  }
                }
                else{
                  attrText += val + ' :  ' + attrs[val] + '\n';
                  console.dir("directly a string ");
                }
//              }
            });
            console.dir(attrText);
            group.close();
        });

        after(function*(){
            file.close();
        });
    });

});

