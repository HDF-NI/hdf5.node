
var hdf5 = require('hdf5');

var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
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
    });
});
