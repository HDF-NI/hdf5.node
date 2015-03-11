var Duplex = require('stream').Duplex;
var inherits = require('util').inherits;
var co = require('co');
var fs = require('fs');
var StreamBoiler = require('lib/stream_boiler.js');
var BufferStream = require('lib/buffer_stream.js');
var H5Tree = require('lib/h5tree.js');

module.exports = MainView

inherits(MainView, Duplex);

function MainView(context, filePath1) {
  Duplex.call(this, {});

  // render the view on a different loop
  co.call(this, this.render).catch(context.onerror);
  this.filePath=filePath1;
}

MainView.prototype._read = function () {};
MainView.prototype._write = function (chunk, encoding, callback) {
    this.push(chunk);
    callback();
    };

MainView.prototype._flush = function (callback) {
    this.push(null);
    callback();
    };

MainView.prototype.render = function* () {
    this.streamBoiler=new StreamBoiler("${h5tree}");
            var appStream=fs.createReadStream("./plates/application.html");
            var _this = this;
//                
            this.streamBoiler.on('inject', function (arg) {
                appStream.pause();
            treeBoiler=new StreamBoiler("${treedata}");
            var treeStream=fs.createReadStream("./plates/h5tree.js");
            treeBoiler.on('inject', function (arg) {
                    treeStream.pause();
                var treeDataBoiler=new StreamBoiler("${h5treedata}");
                var treeDataStream=fs.createReadStream("./plates/h5tree.json");
                treeDataBoiler.on('inject', function (arg) {
                    treeDataStream.pause();
                    co(function* () {
                        var theTree = new H5Tree(_this.filePath);
                        var h5Metadata = yield theTree.loadH5Metadata();
                        return h5Metadata;
                    }).then(function (value) {
//                        console.dir(_this.check);
                        var tBuf=new Buffer(value.toString());
                        var bs=new BufferStream(tBuf);
                        bs.on('end', function() {
                        treeDataStream.resume();
                        treeDataBoiler.end();
                        });
                        bs.pipe(_this);
                        
                      }, function (err) {
                        console.error("co1: "+err.stack);
                    });
                });
                treeDataBoiler.on('finish', function() {
                    treeDataStream.resume();
                    treeBoiler.end();
                    
                });
                treeDataStream.pipe(treeDataBoiler).pipe(_this);
            });
            treeBoiler.on('finish', function() {
                _this.streamBoiler.resume();
                _this.streamBoiler.end();
                _this.push( null );

            });
            treeStream.pipe(treeBoiler).pipe(_this);
              });
            appStream.pipe(this.streamBoiler).pipe(this);

};
