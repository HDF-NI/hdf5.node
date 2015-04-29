
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
        it("should be >0 ", function*(){
            var group=file.createGroup('pmc');
            group.id.should.not.equal(-1);
            group.close();
        });
        it("reopen of pmc should be >0", function*(){
            var groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
            groupPmc.close();
        });
        it("should be >0 ", function*(){
            var xpathGroup=file.createGroup('pmc/Trajectories');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
        });
        it("initial should be >0 ", function*(){
            var xpathGroup=file.createGroup('pmc/Trajectories/0');
            xpathGroup.id.should.not.equal(-1);
            xpathGroup.close();
        });
        it("move should be 1 ", function*(){
            var stemGroup=file.createGroup('pmc/Trajectories');
            stemGroup.move("0", stemGroup,id, "1");
            stemGroup.close();
        });
        it("move should be 1 ", function*(){
            file.move("pmc", file.id, "pmcservices");
        });
        it("should have one child of type group ", function*(){
            var group=file.openGroup('pmcservices');
            group.getNumObjs().should.equal(1);
            group.getChildType("Trajectories").should.equal(H5OType.H5O_TYPE_GROUP);
            group.close();
        });
        it("should add an attribute to the file ", function*(){
            file.role="Target";
            file.flush();

        });
        after(function*(){
          file.close();
        });
    });

});
