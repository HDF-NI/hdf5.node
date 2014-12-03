
var hdf5 = require('hdf5');

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var H5OType = require('lib/globals.js').H5OType;

var CreationOrder = {
    H5P_CRT_ORDER_TRACKED : 1,
    H5P_CRT_ORDER_INDEXED : 2
};

describe("testing c++ interface ",function(){

    describe("create an h5 and group ",function(){
        // open hdf file
        var file;
        before(function*(){
          file = new hdf5.File('./TRAAAAW128F429D538.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be -1 yet", function*(){
            group=file.createGroup();
            group.id.should.equal(-1);
        });
        it("should be >0 ", function*(){
            group.create('pmc', file);
            group.id.should.not.equal(-1);
        });
        var groupPmc;
        it("reopen of pmc should be >0", function*(){
            groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
        });
        var xpathGroup;
        it("should be >0 ", function*(){
            xpathGroup=file.createGroup();
            xpathGroup.create('pmc/Trajectories', file);
            xpathGroup.id.should.not.equal(-1);
        });
        it("should have one child of type group ", function*(){
            var group=file.openGroup('pmc');
            group.getNumObjs().should.equal(1);
            group.getChildType("Trajectories").should.equal(H5OType.H5O_TYPE_GROUP);
        });
    });

});
