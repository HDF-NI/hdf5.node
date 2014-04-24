var assert = require("assert");
//var should = require("should");

var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = {
  ACC_RDONLY :	0,	/*absence of rdwr => rd-only */
  ACC_RDWR :	1,	/*open for read and write    */
  ACC_TRUNC :	2,	/*overwrite existing files   */
  ACC_EXCL :	3,	/*fail if file already exists*/
  ACC_DEBUG :	4,	/*print debug info	     */
  ACC_CREAT :	5	/*create non-existing files  */
};

describe("testing lite interface ",function(){

    describe("create an h5, group and some datasets ",function(){
        // open hdf file
        var file;
        before(function(){
          file = new hdf5.File('./h5lt.h5', Access.ACC_TRUNC);
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
        it("should be Float64Array io ", function(){
            var buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Refractive Index', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index');
            readBuffer.should.match(buffer);
        });
        it("should be Float32Array io ", function(){
            var buffer=new Float32Array(5);
            buffer[0]=5.0;
            buffer[1]=4.0;
            buffer[2]=3.0;
            buffer[3]=2.0;
            buffer[4]=1.0;
            h5lt.makeDataset(group.id, 'Refractive Index f', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index f');
            readBuffer.should.match(buffer);
        });
        it("should be Int32Array io ", function(){
            var buffer=new Int32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index l', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index l');
            readBuffer.should.match(buffer);
        });
        it("should be Uint32Array io ", function(){
            var buffer=new Uint32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index ui', buffer);
            var readBuffer=h5lt.readDataset(group.id, 'Refractive Index ui');
            readBuffer.should.match(buffer);
        });
    });
});
