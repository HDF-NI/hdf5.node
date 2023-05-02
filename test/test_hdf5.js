'use strict';

(require('mocha'));
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
            stemGroup.move("0", stemGroup.id, "1");
            stemGroup.close();
            done();
        });

        it("move should be pmcservices ", function (done) {
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

        it("catch on nonexistent group open try", function(done) {
          try{
            //const group=file.openGroup('pmcservices');
              const groupPmc=file.openGroup('pmc');
              groupPmc.id.should.equal(-1);
              groupPmc.close();
          }
            catch(error) {
            error.message.should.equal("Cannot read properties of undefined (reading 'id')");
          }
          var group;
          try{
            group=file.openGroup('pmcservices');
              const groupPmc=group.openGroup('polywog');
              groupPmc.id.should.equal(-1);
              groupPmc.close();
          }
            catch(error) {
              group.close();
                console.log(error.message);
            error.message.should.equal("Cannot read properties of undefined (reading 'id')");
          }
            done();
        });
        
        after(function(done) {
          file.close();
          done();
        });

    });

    describe("hdf5 check ", function() {
        it.skip("should be an hdf5", function (done) {
              hdf5Lib.hdf5.isHDF5('./TRAAAAW128F429D538.h5').should.equal(true);
            done();
        });

    });

    describe("handling errors ", function() {
        //let file;
        before(function(done) {
          //file = new hdf5Lib.hdf5.File('/home/roger/Downloads/sample.h5', Access.ACC_RDONLY);
          done();
        });

        it("file reaad/write when it doesn't exist", function(done) {
        try {
          const file = new hdf5Lib.hdf5.File('./record.h5', globs.Access.ACC_RDWR);
          const dims = file.getDatasetDimensions('infos');
          file.close();
          if (dims.length > 0) {
          }
        } catch (error) {
          error.message.should.equal("File ./record.h5 doesn\'t exist.");
        }
          done();
        });
        
        it("should stop on broken h5", function(done) {
            try{
              console.log("stop on broken");
              hdf5Lib.hdf5.isHDF5('./test/examples/broken.h5').should.equal(true);
              var file = new hdf5Lib.hdf5.File('./test/examples/broken.h5', globs.Access.ACC_RDONLY);
              file.close();
            }catch(error){
              //console.error(error);
              error.message.should.equal("Failed to open file, ./test/examples/broken.h5 and flags 0 with return: -1.");
            }
            done();
        });
        
        after(function(done) {
          //file.close();
          done();
        });
    });

});

