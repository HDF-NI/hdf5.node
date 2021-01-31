'use strict';

(require('mocha'));
require('should');

const util = require('util');
const hdf5Lib = require('..');

const hdf5          = hdf5Lib.hdf5;
const h5lt          = hdf5Lib.h5lt;

const globs   = require('../lib/globals');

/*test inspired/brought from https://github.com/KirmTwinty work*/
describe("testing hdf5 dataset reading in SWMR mode ", function(){
    describe("opening hdf5 dataset ", function(){
	let file;
	before(function(done){
	    file = new hdf5Lib.hdf5.File('./test/examples/swmr-1.h5', globs.Access.ACC_RDONLY | globs.Access.ACC_SWMR_READ);
	    done();
	});

	it('should be > 0, opening from file ', function(done){
                file.id.should.not.equal(-1);
            const readBuffer=h5lt.readDataset(file.id, 'group1/dataset-int', function(options){
                options.rank.should.equal(2);
                options.rows.should.equal(1);
            });
	    done();
	});

	it('should be > 0, opening from group ', function(done){
	    var group = file.openGroup('group1');
                group.id.should.not.equal(-1);
            const readBuffer=h5lt.readDataset(group.id, 'dataset-int', function(options){
                options.rank.should.equal(2);
                options.rows.should.equal(1);
            });
	    group.close();
	    done();
	});

	it("catch on nonexistent dataset open try on file", function(done) {
	    try{
                const readBuffer=h5lt.readDataset(file.id, 'nonexistingdataset', function(options){
            });
            }
            catch(error) {
		error.message.should.equal("failed to find dataset rank");
            }
	    done();
	});

	it("catch on nonexistent dataset open try on group", function(done) {
	    try{
		var group = file.openGroup('group1');
                const readBuffer=h5lt.readDataset(group.id, 'nonexistingdataset', function(options){});
            }
            catch(error) {
		error.message.should.equal("failed to find dataset rank");
		group.close()
            }
	    done();
	});
        
	after(function(done){
	    file.close();
            done();
	});
    });

});

