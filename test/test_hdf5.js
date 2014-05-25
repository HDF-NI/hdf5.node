
var hdf5 = require('hdf5');

var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

var CreationOrder = {
    H5P_CRT_ORDER_TRACKED : 1,
    H5P_CRT_ORDER_INDEXED : 2
};

describe("testing c++ interface ",function(){

    describe("create an h5 and group ",function(){
        // open hdf file
        var file;
        before(function(){
          file = new hdf5.File('./TRAAAAW128F429D538.h5', Access.ACC_TRUNC);
        });
        var group;
        it("should be -1 yet", function(){
            group=file.createGroup();
            group.id.should.equal(-1);
        });
        it("should be >0 ", function(){
            group.create('pmc', file);
            group.id.should.not.equal(-1);
        });
        var groupPmc;
        it("reopen of pmc should be >0", function(){
            groupPmc=file.openGroup('pmc');
            groupPmc.id.should.not.equal(-1);
        });
        var xpathGroup;
        it("should be >0 ", function(){
            xpathGroup=file.createGroup();
            xpathGroup.create('pmc/Trajectories', file);
            xpathGroup.id.should.not.equal(-1);
        });
    });
    describe("read an h5, examine group metadata and list members ",function(){
        // open hdf file
        var file;
        before(function(){
          file = new hdf5.File('/home/roger/testing-grounds/sodium eicosanoate/roothaan.h5', Access.ACC_RDONLY);
        });
        var groupTarget;
        it("open of target should be >0", function(){
            groupTarget=file.openGroup('DivCon/sodium-icosanoate', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
        });
        it("getNumAttrs of groupTarget should be 3", function(){
            groupTarget.getNumAttrs().should.equal(3);
        });
        it("getAttributeNames of groupTarget should be 3 names", function(){
            var array=groupTarget.getAttributeNames();
            array.length.should.equal(3);
        });
        it("readAttribute Computed Heat Of Formation should be -221.78436098572274", function(){
            var attrValue=groupTarget.readAttribute("Computed Heat Of Formation");
            attrValue.should.equal(-221.78436098572274);
        });
        var groupGeometries;
        it("open of Geometries should be >0", function(){
            groupGeometries=file.openGroup('DivCon/sodium-icosanoate/Trajectories/Geometries');
            groupGeometries.id.should.not.equal(-1);
        });
        it("getNumAttrs of Geometries should be 0", function(){
            groupGeometries.getNumAttrs().should.equal(0);
        });
        it("getNumObjs of Geometries should be 240", function(){
            groupGeometries.getNumObjs().should.equal(240);
        });
        it("getMemberNames of Geometries should be 240 names in creation order", function(){
            var array=groupGeometries.getMemberNamesByCreationOrder();
            array[239].should.equal("239");
        });
    
   });
});
