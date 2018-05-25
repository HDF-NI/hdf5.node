'use strict';

(require('mocha'));
require('should');

const hdf5Lib = require('..');
const globs   = require('../lib/globals');

describe("testing fiter interface ",function(){

    describe("create an h5, group and some compressed datasets ", function() {
        // open hdf file
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./filters.h5', globs.Access.ACC_TRUNC);
          done();
        });

        it("should set filter ", function(done) {
            const group = file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            const buffer = Buffer.alloc(5000*8, "\0", "binary");

            buffer.type = globs.H5Type.H5T_NATIVE_DOUBLE;
            for (let index = 0; index < 5000;index++) {
                buffer.writeDoubleLE((index % 1000), index*8);
            }
            hdf5Lib.h5lt.makeDataset(group.id, 'Data', buffer, {compression: 7});

            const filters = group.getFilters('Data');
            filters.isAvailable(globs.H5ZType.H5Z_FILTER_DEFLATE).should.be.true;
            filters.isAvailable(32004).should.be.false;
            filters.getNFilters().should.equal(1);

            const readAsBuffer = hdf5Lib.h5lt.readDatasetAsBuffer(group.id, 'Data');
            readAsBuffer.length.should.equal(40000);
            readAsBuffer.rows.should.equal(5000);
            group.close();
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe("should read compressed dataset", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./filters.h5', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be compression filter info ", function(done) {
            const group   = file.openGroup('pmc/refinement');
            const filters = group.getFilters('Data');
            filters.isAvailable(globs.H5ZType.H5Z_FILTER_DEFLATE).should.be.true;
            filters.getNFilters().should.equal(1);
            group.close();
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

});

