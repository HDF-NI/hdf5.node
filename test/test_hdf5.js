'use strict';

require('co-mocha')(require('mocha'));
require('should');


const hdf5Lib = require('..');
const globs   = require('../lib/globals');

describe("testing c interface ", function() {

    describe("create an h5 and group ", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('./TRAAAAW128F429D538.h5', globs.Access.ACC_TRUNC);
          done();
        });

        it("should be >0 ", function (done) {
            const group=file.createGroup('/pmc/x-ray/refinement');
            group.id.should.not.equal(-1);
            group.close();
            done();
        });

        it("reopen of pmc should be >0", function(done) {
            const groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
            groupPmc.close();
            done();
        });

        it("should be >0 ", function (done) {
            const xpathGroup=file.createGroup('pmc/Trajectories');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
            done();
        });

        it("initial should be >0 ", function (done) {
            const xpathGroup=file.createGroup('pmc/Trajectories/0');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
            done();
        });

        it("move should be 1 ", function (done) {
            const stemGroup=file.openGroup('pmc/Trajectories');
            console.dir("move 0 to 1 "+stemGroup.id);
            stemGroup.move("0", stemGroup.id, "1");
            stemGroup.close();
            done();
        });

        it("move should be pmcservices ", function (done) {
            console.dir("move pmc to pmcservices "+file.id);
            file.move("pmc", file.id, "pmcservices");
            done();
        });

        it("should have one child of type group ", function (done) {
            const group=file.openGroup('pmcservices');
            group.getNumObjs().should.equal(2);
            group.getChildType("Trajectories").should.equal(globs.H5OType.H5O_TYPE_GROUP);
            group.close();
            done();
        });

        it("should get member names ", function (done) {
            const names=file.getMemberNames();
            names.length.should.equal(1);
            names[0].should.equal('pmcservices');
            done();
        });

        it("should add an attribute to the file ", function (done) {
            file.role="Target";
            file.flush();
            done();
        });

        after(function(done) {
          file.close();
          done();
        });

    });


});

