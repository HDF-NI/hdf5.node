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
var Interlace = require('lib/globals').Interlace;
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
        stemGroup.move(argument,  stemGroup.id, leaf);
        stemGroup.close();
    }
    else{
    console.dir(leaf+" to2 "+argument);
        file.move(argument, file.id, leaf);
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
    var squareIndex=path.lastIndexOf("[");
    if(squareIndex<0){this.body="";return;}
    var index=path.lastIndexOf("/", squareIndex);
    var stem = "";
    var leaf = "";
    if(index>=0)
    {
        stem=path.substring(0, index);
        leaf=path.substring(index+1, squareIndex);
    }
    else
        leaf = path;
    var originalStem=path.substring(squareIndex+1, path.length-1);
    console.dir(stem);
    console.dir(leaf);
    console.dir(originalStem);
    
    try{
    var file = new hdf5.File(currentH5Path, Access.ACC_RDWR);
    if(stem && originalStem){
    console.dir(leaf+" to "+originalStem+" from "+stem);
        var stemGroup=file.openGroup(stem);
        var originalStemGroup=file.openGroup(originalStem);
        originalStemGroup.move(leaf,  stemGroup.id, leaf);
        stemGroup.close();
        originalStemGroup.close();
    }
    else if(originalStem){
    console.dir(leaf+" to2 "+originalStem);
        var originalStemGroup=file.openGroup(originalStem);
        originalStemGroup.move(leaf, file.id, leaf);
        originalStemGroup.close();
    }
    file.close();
    }
    catch(err){
        console.dir(err.message);
    }
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
        server.on('error',function (error ) {console.dir(error);});
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
                image.interlace=Interlace.INTERLACE_PIXEL;
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

module.exports.makeTable = function * makeTable(path) {
    if ('POST' != this.method) return yield next;
    console.dir("got to make table");
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
        var server = BinaryServer({port: 9001, path: '/binary-store-table'});
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
                var tableData = Buffer.concat(buffers);
                console.dir(meta.name+" "+meta.rows);
            var tableModel=new Array(meta.columns);
                    var lines = tableData.toString().split('\n');
                    var header = lines[0].split(",");
                    
                    var firstDataLine = lines[1].split(",");
                    
                                    for(var k = 0; k < header.length; k++){
                                        var columnName=header[k].replace("^\"|\"$/g", "");
                                        console.dir("columnName "+columnName.constructor);
                                        if(isNaN(firstDataLine[k])){
                                            tableModel[k]=new Array(meta.rows);
                                            tableModel[k].name=columnName;
                                            tableModel[k][0]=firstDataLine[k].replace("^\"|\"$/g", "");
                                        }
                                        else if(Number.isInteger(firstDataLine[k])){
                                            tableModel[k]=new Int32Array(meta.rows);
                                            tableModel[k].name=columnName.replace("^\"|\"$/g", "");
                                            tableModel[k][0]=firstDataLine[k];
                                        }
                                        else{
                                            tableModel[k]=new Float64Array(meta.rows);
                                            tableModel[k].name=columnName.replace("^\"|\"$/g", "");
                                            tableModel[k][0]=firstDataLine[k];
                                            
                                        }
                                    }
                    
                    for(var j = 2; j<lines.length; j++){
                            if(lines[j] != ""){
                                    var information = lines[j].split(",");
                                    for(var k = 0; k < information.length; k++){
                                        if(tableModel[k].constructor===Array){
                                        tableModel[k][j-1]=information[k].replace("^\"|\"$/g", "");
                                        }
                                        else{
                                        tableModel[k][j-1]=information[k];
                                        }
                                    }
                            }

                    }
                try{
                    h5tb.makeTable(group.id, meta.name, tableModel);
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
