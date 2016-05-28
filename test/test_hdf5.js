'use strict';

require('co-mocha')(require('mocha'));
require('should');


const hdf5Lib = require('..');
const globs   = require('../lib/globals');

describe("testing c interface ", function() {

    describe("create an h5 and group ", function() {
        let file;
        before(function*() {
          file = new hdf5Lib.hdf5.File('./TRAAAAW128F429D538.h5', globs.Access.ACC_TRUNC);
        });

        it("should be >0 ", function*() {
            const group=file.createGroup('pmc');
            group.id.should.not.equal(-1);
            group.close();
        });

        it("reopen of pmc should be >0", function*() {
            const groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
            groupPmc.close();
        });

        it("should be >0 ", function*() {
            const xpathGroup=file.createGroup('pmc/Trajectories');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
        });

        it("initial should be >0 ", function*() {
            const xpathGroup=file.createGroup('pmc/Trajectories/0');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
        });

        it("move should be 1 ", function*() {
            const stemGroup=file.createGroup('pmc/Trajectories');
            stemGroup.move("0", stemGroup.id, "1");
            stemGroup.close();
        });

        it("move should be pmcservices ", function*() {
            file.move("pmc", file.id, "pmcservices");
        });

        it("should have one child of type group ", function*() {
            const group=file.openGroup('pmcservices');
            group.getNumObjs().should.equal(1);
            group.getChildType("Trajectories").should.equal(globs.H5OType.H5O_TYPE_GROUP);
            group.close();
        });

        it("should add an attribute to the file ", function*() {
            file.role="Target";
            file.flush();
        });

        after(function*() {
          file.close();
        });

    });


});

