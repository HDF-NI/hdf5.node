
var hdf5 = require('hdf5');

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var H5OType = require('lib/globals.js').H5OType;

describe("testing c++ interface ",function(){

    describe("create an h5 and group ",function(){
        // open hdf file
        var file;
        before(function*(){
          file = new hdf5.File('./TRAAAAW128F429D538.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be >0 ", function*(){
            group=file.createGroup('pmc');
            group.id.should.not.equal(-1);
        });
        var groupPmc;
        it("reopen of pmc should be >0", function*(){
            groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
        });
        var xpathGroup;
        it("should be >0 ", function*(){
            xpathGroup=file.createGroup('pmc/Trajectories');
            xpathGroup.id.should.not.equal(-1);
        });
        it("should have one child of type group ", function*(){
            var group=file.openGroup('pmc');
            group.getNumObjs().should.equal(1);
            group.getChildType("Trajectories").should.equal(H5OType.H5O_TYPE_GROUP);
        });
        it("should add an attribute to the file ", function*(){
            file.role="Target";
            file.flush();
            
        });
    });

});
