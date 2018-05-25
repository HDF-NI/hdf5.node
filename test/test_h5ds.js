'use strict';

(require('mocha'));
require('should');

const hdf5Lib = require('..');
const globs   = require('../lib/globals');
const hdf5    = hdf5Lib.hdf5;
const h5lt    = hdf5Lib.h5lt;
const h5ds    = hdf5Lib.h5ds;
const H5Type  = globs.H5Type;

describe("testing dimension scale interface ", function() {

    describe("create an h5, group and some dimension scales ", function() {
        // open hdf file
        let file;
        before(function(done) {
          file = new hdf5.File('./h5ds.h5', globs.Access.ACC_TRUNC);
          done();
        });

        it("should set scale ", function(done) {
            const group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            const buffer=Buffer.alloc(4096*4, "\0", "binary");
            buffer.type=H5Type.H5T_NATIVE_UINT;
            buffer.rank=2;
            buffer.rows=64;
            buffer.columns=64;
            for(let index=0;index<4096;index++) {
                buffer.writeUInt32LE((index % 1000), index*4);
            }
            h5lt.makeDataset(group.id, 'Data', buffer);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Data');
            readAsBuffer.length.should.equal(16384);
            readAsBuffer.rows.should.equal(64);

            const scaleBuffer=Buffer.alloc(64*8, "\0", "binary");
            scaleBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for(let index=0;index<64;index++) {
                scaleBuffer.writeDoubleLE(index, index*8);
            }
            h5lt.makeDataset(group.id, 'X(dim)', scaleBuffer);
            h5ds.isScale(group.id, 'X(dim)').should.equal(false);
            h5ds.setScale(group.id, 'X(dim)', "The X Scale");
            h5ds.isScale(group.id, 'X(dim)').should.equal(true);

            h5ds.isAttached(group.id, 'Data', 'X(dim)', 0).should.equal(false);
            h5ds.attachScale(group.id, 'Data', 'X(dim)', 0);
            h5ds.isAttached(group.id, 'Data', 'X(dim)', 0).should.equal(true);

            const scaleYBuffer=Buffer.alloc(64*8, "\0", "binary");
            scaleYBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for(let index=0;index<64;index++) {
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

            try {
                h5ds.iterateScale(group.id, 'Data', 1, function(r, status) {
                    console.dir("status: "+status);
                });
            } catch (e) {

            }
            group.close();
            done();
        });

        after(function(done) {
            file.close();
            done();
        });
    });

    describe("should read dimension scales ", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./h5ds.h5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be dimension scales info ", function(done) {
            const group  = file.openGroup('pmc/refinement');
            let attrs    = group.getDatasetAttributes("Data");
            let attrText = "";
            Object.getOwnPropertyNames(attrs).forEach(function(val) {
                if (val!=="id") {
                    if(attrs[val].constructor.name===Array) {
                        attrText+=val+" :  ";
                        for(let mIndex=0;mIndex<attrs[val].Length();mIndex++) {
                            attrText+=attrs[val][mIndex];
                            if(mIndex<attrs[val].Length()-1) {attrText+=",";}
                        }
                    }
                    else{
                        attrText+=val+" :  "+attrs[val]+"\n";
                    }
                }
            });
            attrText.should.equal('DIMENSION_LIST :  ->/pmc/refinement/X(dim),->/pmc/refinement/Y(dim)\nDIMENSION_LABELS :  ,y\n');
            attrs = group.getDatasetAttributes("X(dim)");
            attrText="";
            Object.getOwnPropertyNames(attrs).forEach(function(val) {
                if (val !== "id") {
                    if(attrs[val].constructor.name===Array) {
                        attrText+=val+" :  ";
                        for(let mIndex=0;mIndex<attrs[val].Length();mIndex++) {
                            attrText+=attrs[val][mIndex];
                            if(mIndex<attrs[val].Length()-1) {attrText+=",";}
                        }
                    }
                    else{
                        attrText+=val+" :  "+attrs[val]+"\n";
                    }
                }
            });
            attrText.should.equal('CLASS :  DIMENSION_SCALE\nNAME :  The X Scale\nREFERENCE_LIST :  ->/pmc/refinement/Data,0\n');
            group.close();
            done();
        });

        after(function(done) {
            file.close();
            done();
        });
    });

});

