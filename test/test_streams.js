var co = require('co');
var fs = require('fs');
var StreamTable = require('lib/stream_table.js');

var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = require('lib/globals.js').Access;
var CreationOrder = require('lib/globals.js').CreationOrder;
var H5OType = require('lib/globals.js').H5OType;

var MainView = require('views/MainView.js');

  describe("Stream tests", function() {
//    var content;

    before(function*() {
//      yield setup();
//        content = yield fs.createReadStream("./views/h5tree.js.plate", "utf8");
    });

    it("should be from memory buffer", function*() {
        try
        {
//            var jsStream=fs.createReadStream("./plates/h5tree.js");
//            var jsonStream=fs.createReadStream("./plates/h5tree.json");
//            jsonStream.pipe(es.wait(function (err, body) {
//            jsStream.pipe(es.replace('${treedata}', body)).pipe(es.wait(function (err, body) {
//                    console.dir(body);
//                }));
//            }));
//            var boiler=new StreamBoiler();
//            boiler.pipe(process.stdout);
//            console.dir(boiler);
           // boiler.resume();
//            readStream.on('readable', function(){
//                readStream.pipe(boiler);
//            });

//            var state=false;
//            boiler.on('header', function(chunk){
//            
//                console.dir('got %d bytes of data', chunk.length);
//            });
//            var state=false;
//            boiler.on('end', function() {
//              console.dir('Goodbye\n');
//              state=true;
//            });
//            boiler.write("say what?");
        }
        catch (err) {
        console.dir(err.message);
        }
    });

    it("should produce table", function*() {
        try
        {
//            var file= new hdf5.File('/home/roger/Downloads/header_sample_2015-01-19/series_1_master.h5', Access.ACC_RDONLY);
//            var parent=file.openGroup("entry/data");
//            var document=h5lt.readDataset(parent.id, "data_000001");
//            console.dir("r="+document.rank);
//            var streamTable=new StreamTable(document.rows, document.columns);
////            streamTable.pipe(process.stdout);
////            streamTable.resume();
//                        for(var i=0;i<document.rows*document.columns;i++)
//                        {
//                            //console.dir(document[i]);
//                                streamTable.write(document[i]);
//                        }
//            streamTable.end();

        }
        catch (err) {
        console.dir(err.message);
        }
    });

    it("should produce page", function*() {
        try
        {
//            var main=new MainView(this, "/home/roger/Downloads/header_sample_2015-01-19/series_1_master.h5");
//            main.pipe(process.stdout);

        }
        catch (err) {
        console.dir(err.message);
        }
    });

    after(function*() {
//      yield teardown();
    });
  });