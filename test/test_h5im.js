'use strict';

require('co-mocha')(require('mocha'));
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
        before(function*() {
            try {
            file  = new hdf5.File('./h5im.h5', Access.ACC_TRUNC);
            file2 = new hdf5.File('./test/examples/hdf5.h5', Access.ACC_RDONLY);
            image = h5im.readImage(file2.id, 'hdf_logo.jpg');
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
        });

        it("should be 1.10.0 ", function*() {
            var version=hdf5.getLibVersion();
            console.dir(version);
        });

        let group;
        it("should be >0 ", function*() {
            group = file.createGroup('pmc');
            group.id.should.not.equal(-1);
        });

        it("should be an image ", function*() {
            const res = h5im.isImage(file2.id, 'hdf_logo.jpg');
            res.should.equal(true);
        });

        it("image width should be 48 ", function*() {
            image.width.should.equal(48);
        });

        it("image height should be 45 ", function*() {
            image.height.should.equal(45);
        });

        it("image interlace should be INTERLACE_PIXEL ", function*() {
            image.interlace.should.equal('INTERLACE_PIXEL');
        });

        it("image planes should be 3 ", function*() {
            image.planes.should.equal(3);
        });

        it("image length should be 6480 ", function*() {
            image.length.should.equal(6480);
        });

        it("make image  ", function*() {
            h5im.makeImage(group.id, 'hdf_logo.jpg', image);
        });

        let imageAgain;
        it("again image width should be 48 ", function*() {
            imageAgain=h5im.readImage(group.id, 'hdf_logo.jpg');
            imageAgain.width.should.equal(48);
        });

        it("again make image  ", function*() {
            h5im.makeImage(group.id, 'repeat.jpg', imageAgain);
        });

        after(function*() {
            group.close();
            file.close();
            file2.close();
        });
    });
});
