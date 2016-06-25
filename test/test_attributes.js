'use strict';

require('co-mocha')(require('mocha'));
require('should');

const hdf5Lib = require('..');
const globs   = require('../lib/globals');

describe("testing atrribute interface ",function(){

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
            group.close();
        });

        after(function*(){
            file.close();
        });
    });

});

