'use strict';

(require('mocha'));
require('should');

const hdf5Lib = require('..');
const globs   = require('../lib/globals');
const H5RType        = globs.H5RType;
const hdf5          = hdf5Lib.hdf5;
const h5lt          = hdf5Lib.h5lt;

describe("testing attribute interface",function(){

    describe("create an h5, group and some attributes ", function() {
        // open hdf file
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./attributes.h5', globs.Access.ACC_TRUNC);
          done();
        });

        it("should set filter ", function(done) {
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
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe("should read attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./attributes.h5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be attribute info ", function(done) {
            const group   = file.openGroup('pmc/refinement');
            group.refresh();
            group.name.should.equal("3FVA");
            group.name.type.should.equal("variable-length");
            group.size.constructor.name.should.match('Float64Array');
             console.dir(group.notes);
            group.notes.constructor.name.should.match('Array');
            group.close();
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe.skip("should read biom attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./long_attributes.biom', globs.Access.ACC_TRUNC);
          done();
        });

        it("write attribute info", function(done) {
            file.nnz=new hdf5Lib.hdf5.Int64("15");
            file.unnz=new hdf5Lib.hdf5.Uint64("40");
            var fv=new Array(2);
            fv[0]=new hdf5Lib.hdf5.Int64("2");
            fv[1]=new hdf5Lib.hdf5.Int64("1");
            file["formart-version"]=fv;
            var ufv=new Array(2);
            ufv[0]=new hdf5Lib.hdf5.Uint64("20");
            ufv[1]=new hdf5Lib.hdf5.Uint64("10");
            file["uformart-version"]=ufv;
            file.flush();
            done();
        });
        
        it("should be attribute info ", function(done) {
            console.log(file);
            console.log(file.getNumAttrs());
            file.refresh();
            for (var property in file) {
                if (file.hasOwnProperty(property)) {
                    console.log(property+": "+file[property]);
                }
            }
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe.skip("read variable string attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/sample.h5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be variable string info", function(done) {
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
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe.skip("read two attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/HAA-NL-OPA-picnictd-BE-341.hdf5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be variable string info", function(done) {
            const group   = file.openGroup('LEG_L_KBA_FLUX');
            group.refresh();
            var attrs = group.getDatasetAttributes("y");
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
            var buffer=h5lt.readDataset(group.id, 'y');
            attrText = "";
            Object.getOwnPropertyNames(buffer).forEach(function(val, idx, array) {
//              if (val !=  'id') {
                if (buffer[val].constructor.name === Array) {
                  attrText += val + ' :  ';
                  for (var mIndex = 0; mIndex < buffer[val].Length(); mIndex++) {
                    attrText += buffer[val][mIndex];
                    if (mIndex < buffer[val].Length() - 1) {
                      attrText += ',';
                    }
                  }
                }
                else{
                  attrText += val + ' :  ' + buffer[val] + '\n';
                }
//              }
            });
            console.dir(attrText);
            
            group.close();
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe.skip("read reference attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/test.h5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be reference to clock signal", function(done) {
          try{
            const group   = file.openGroup('Group_1');
            const readBuffer=h5lt.readDataset(group.id, 'Dataset_1', function(options){
                options.rank.should.equal(2);
                options.rows.should.equal(10);
                options.columns.should.equal(1);
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(10);
            var refName=readBuffer.bases.getName(file.id, H5RType.H5R_OBJECT);
            console.log(refName);
            const refBuffer=h5lt.readDataset(file.id, refName, function(options){
                options.rank.should.equal(2);
                options.rows.should.equal(10);
                options.columns.should.equal(1);
            });
            
            group.close();
          }
          catch (e) {
            console.log("\t\t"+e.message);
            e.message.should.equal("unsupported data type");
          }
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

});

