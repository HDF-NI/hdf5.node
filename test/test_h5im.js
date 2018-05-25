'use strict';

(require('mocha'));
require("should");

const hdf5Lib = require('..');
const Access  = require('../lib/globals.js').Access;

describe("testing images ",function() {
    const hdf5   = hdf5Lib.hdf5;
    const h5im   = hdf5Lib.h5im;

    describe("read image and transfer",function() {
        // open hdf file
        let file;
        let file2;
        let image;
        before(function(done) {
            try {
            file  = new hdf5.File('./h5im.h5', Access.ACC_TRUNC);
            file2 = new hdf5.File('./test/examples/hdf5.h5', Access.ACC_RDONLY);
            image = h5im.readImage(file2.id, 'hdf_logo.jpg');
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            done();
        });

        it("should be 1.10.2 ", function(done) {
            var version=hdf5.getLibVersion();
            version.should.startWith('1.');
            done();
        });

        let group;
        it("should be >0 ", function(done) {
            group = file.createGroup('pmc');
            group.id.should.not.equal(-1);
            done();
        });

        it("should be an image ", function(done) {
            const res = h5im.isImage(file2.id, 'hdf_logo.jpg');
            res.should.equal(true);
            done();
        });

        it("image width should be 48 ", function(done) {
            image.width.should.equal(48);
            done();
        });

        it("image height should be 45 ", function(done) {
            image.height.should.equal(45);
            done();
        });

        it("image interlace should be INTERLACE_PIXEL ", function(done) {
            image.interlace.should.equal('INTERLACE_PIXEL');
            done();
        });

        it("image planes should be 3 ", function(done) {
            image.planes.should.equal(3);
            done();
        });

        it("image length should be 6480 ", function(done) {
            image.length.should.equal(6480);
            done();
        });

        it("make image  ", function(done) {
            h5im.makeImage(group.id, 'hdf_logo.jpg', image);
            done();
        });

        let imageAgain;
        it("again image width should be 48 ", function(done) {
            imageAgain=h5im.readImage(group.id, 'hdf_logo.jpg');
            imageAgain.width.should.equal(48);
            done();
        });

        it("again make image  ", function(done) {
            h5im.makeImage(group.id, 'repeat.jpg', imageAgain, {width: imageAgain.width, height: imageAgain.height, planes: imageAgain.planes});
            done();
        });

        it("repeat image options.width should be 48 ", function(done) {
            let imageRepeat=h5im.readImage(group.id, 'repeat.jpg', (options) =>{
                options.width.should.equal(48);
                options.height.should.equal(45);
                options.planes.should.equal(3);
                options.interlace.should.equal('INTERLACE_PIXEL');
            });
            done();
        });

        after(function(done) {
            group.close();
            file.close();
            file2.close();
            done();
        });
    });
});
