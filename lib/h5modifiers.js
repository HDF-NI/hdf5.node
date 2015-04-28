var unirest = require('unirest');
var hdf5 = require('hdf5');
var h5im = require('h5im');
var h5lt = require('h5lt');
var h5tb = require('h5tb');
var h5pt = require('h5pt');

var co = require('co');
var BinaryServer = require('binaryjs').BinaryServer;

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;
var HLType = require('lib/globals').HLType;
var StreamTable = require('lib/stream_table');
var BufferStream = require('lib/buffer_stream.js');

module.exports.create = function * create(path) {
    if ('POST' != this.method) return yield next;
    var index=path.lastIndexOf("/");
    var stem = "";
    var leaf = "";
    if(index>=0)
    {
        stem=path.substring(0, index);
        leaf=path.substring(index+1, path.length);
    }
    else
        leaf = path;
    console.dir(stem);
    console.dir(leaf);
    var file = new hdf5.File(currentH5Path, Access.ACC_RDWR);
    console.dir(stem+"/"+leaf);
    if(stem){
        var newGroup=file.createGroup(stem+"/"+leaf);
        newGroup.flush();
        newGroup.close();
    }
    else{
        var newGroup=file.createGroup("/"+leaf);
        newGroup.flush();
        newGroup.close();
    }
    file.close();
    this.body = "";
    return;
};

module.exports.rename = function * rename(path) {
    if ('POST' != this.method) return yield next;
    var index=path.lastIndexOf("/");
    var stem = "";
    var leaf = "";
    if(index>=0)
    {
        stem=path.substring(0, index);
        leaf=path.substring(index+1, path.length);
    }
    else
        leaf = path;
    console.dir(stem);
    console.dir(leaf);
    var rightBracketIndex=leaf.indexOf("[");
    var argument="";
    if(rightBracketIndex>0){
        argument=leaf.substring(rightBracketIndex+1, leaf.length-1);
        leaf=leaf.substring(0, rightBracketIndex);
    }
    try{
    var file = new hdf5.File(currentH5Path, Access.ACC_RDWR);
    if(stem){
    console.dir(leaf+" to "+argument+" on "+stem);
        var stemGroup=file.openGroup(stem);
        var originalGroup=file.openGroup(stem+"/"+argument);
        var newGroup=file.createGroup(stem+"/"+leaf);
        stemGroup.move(originalGroup, newGroup);
        newGroup.flush();
        originalGroup.delete();
        newGroup.close();
        stemGroup.close();
    }
    else{
    console.dir(leaf+" to2 "+argument);
        var originalGroup=file.openGroup("/"+leaf);
        var newGroup=file.createGroup("/"+argument);
        file.move(originalGroup, newGroup);
        newGroup.flush();
        originalGroup.delete();
        newGroup.close();
    }
    file.close();
    }
    catch(err){
        console.dir(err.message);
    }
    this.body = "";
    return;
};

module.exports.move = function * move(path) {
    if ('POST' != this.method) return yield next;
    console.dir("got to move");
        this.body = "";
        return;
};

module.exports.copy = function * copy(path) {
    if ('POST' != this.method) return yield next;
    console.dir("got to copy");
        this.body = "";
        return;
};

module.exports.paste = function * paste(path) {
    if ('POST' != this.method) return yield next;
    console.dir("got to paste");
        this.body = "";
        return;
};

module.exports.deleteit = function * deleteit(path) {
    if ('POST' != this.method) return yield next;
    var index=path.lastIndexOf("/");
    var stem = "";
    var leaf = "";
    if(index>=0)
    {
        stem=path.substring(0, index);
        leaf=path.substring(index+1, path.length);
    }
    else
        leaf = path;
    console.dir(stem);
    console.dir(leaf);
    var file = new hdf5.File(currentH5Path, Access.ACC_RDWR);
    console.dir("opened h5");
    if(stem){
        var group=file.openGroup(stem);
        group.delete(leaf);
        group.close();
    }
    else{
        file.delete(leaf);

    }
    file.close();
    this.body = "";
    return;
};

module.exports.makeImage = function * makeImage(path) {
    if ('POST' != this.method) return yield next;
    console.dir("got to make image");
    var index=path.lastIndexOf("/");
    var stem = "";
    var leaf = "";
    if(index>=0)
    {
        stem=path.substring(0, index);
        leaf=path.substring(index+1, path.length);
    }
    else
        leaf = path;
    console.dir(stem);
    console.dir(leaf);
        var server = BinaryServer({port: 9001, path: '/binary-store-image'});
        // Wait for new user connections
        server.on('connection', function(client){
          // Incoming stream from browsers
          console.dir("connected");
          client.on('stream', function(stream, meta){
          console.dir("streaming");
          try{
            var buffers = [];
            stream.on('data', function(buffer) {
          console.dir("streaming data");
              buffers.push(buffer);
            });
            stream.on('end', function() {
          console.dir("streaming end "+currentH5Path);
                var file = new hdf5.File(currentH5Path, Access.ACC_RDWR);
          console.dir("open "+stem);
                var group=file.openGroup(stem);
          console.dir("buffer concat");
                var image = Buffer.concat(buffers);
                image.interlace=0;
                image.planes=4;
                image.width=meta.width;
                image.height=meta.height;
                console.dir(meta.name+" "+meta.width);
          console.dir("buffer concat");
                try{
                h5im.makeImage(group.id, meta.name, image);
                }
                catch(err){
                    console.dir(err.message);
                }
                group.close();
                file.close();
                server.close();
            });
        }catch(err){
            console.dir(err);
        }
          });
        });
        this.body = "";
        return;
};

