var unirest = require('unirest');
var hdf5 = require('hdf5');
var h5im = require('h5im');
var h5lt = require('h5lt');
var h5tb = require('h5tb');
var h5pt = require('h5pt');

var co = require('co');
var BinaryServer = require('binaryjs').BinaryServer;
var WebSocketServer = require('ws').Server

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;
var HLType = require('lib/globals').HLType;
var StreamTable = require('lib/stream_table');
var BufferStream = require('lib/buffer_stream.js');

var requireDir = require('require-dir');
var tableAddons = requireDir("../addons/table");
//var textAddons = requireDir("../addons/text");

module.exports.datasetEditor = function * datasetEditor(path) {
    if ('GET' != this.method) return yield next;
    //console.dir("load editor"+path);
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
    if(stem && leaf){
        //console.dir(stem+" on to h5 "+leaf);
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var document=h5lt.readDataset(parent.id, leaf);
        if(document instanceof Float64Array || document instanceof Int32Array || document instanceof Uint32Array)
        {
            switch(document.rank)
            {
                case 3:
                var sections="";
                var csvDoc="";
                console.dir("sec "+document.sections+" "+document.rows+" "+document.columns);
                this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><form>";
//                        for(var k=0;k<document.sections;k++)
                {
                var streamTable=new StreamTable(document.rows, document.columns);
                this.body+=streamTable;
//            streamTable.resume();
                for(var i=0;i<document.rows*document.columns;i++)
                {
                    //console.dir(document[i]);
                        streamTable.write(document[i]);
                }
//                        var table ="<table>\n";
//                        for(var i=0;i<document.rows;i++)
//                        {
//                            table +="<tr>";
//                            var columns=document.columns;
//                            for(var j=0;j<columns;j++)
//                            {
//                                table +="<td>"+document[document.rows*document.columns*k+columns*i+j]+"</td>";
//                                csvDoc+=document[document.rows*document.columns*k+columns*i+j];
//                                if(j<columns-1)csvDoc+=",";
//                            }
//                            table +="</tr>\n";
//                            csvDoc+="\n";
//                        }
//                        table +="</table>\n";
//                        sections+=table;
                }
//                        sections
                this.body+="</form></div>";
                unirest.post('http://localhost:8000/_/example2')
                .headers({ 'Content-Type': 'text/csv' })
                .send(csvDoc)
                .end(function (response) {
                  console.log(response.body);
                });
                break
                case 2:
                case 1:
                var table ="<table>\n";
                var csvDoc="";
                for(var i=0;i<document.rows;i++)
                {
                    table +="<tr>";
                    var columns=1;
                    if(document.rank==2)columns=document.columns;
                    for(var j=0;j<columns;j++)
                    {
                        table +="<td>"+document[columns*i+j]+"</td>";
                        csvDoc+=document[columns*i+j];
                        if(j<columns-1)csvDoc+=",";
                    }
                    table +="</tr>\n";
                    csvDoc+="\n";
                }
                table +="</table>\n";
                this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><form>"+table+"</form></div>";
//                unirest.post('http://localhost:8000/_/example2')
//                .headers({ 'Content-Type': 'text/csv' })
//                .send(csvDoc)
//                .end(function (response) {
//                  console.log(response.body);
//                });
                break;
            default:
            break;
            }
        }
//                else if(document instanceof Int32Array)
//                {
//                    
//                }
        else
        {
            this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><form><textarea type=\"text\" name=\"username\" id=\"editortextarea\" draggable=\"true\">"+document+"</textarea></form></div>";
        }
        parent.close();
        file.close();

    }
    else this.body = "";
};


module.exports.imageEditor = function * imageEditor(path) {
    if ('GET' != this.method) return yield next;
    //console.dir("load editor"+path);
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
    if(stem && leaf){
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var buffer=h5im.readImage(parent.id, leaf);
        var channelSize = buffer.width * buffer.height;
        var size = channelSize * (buffer.planes);
        var redChannelEnd = channelSize * 1;
        var greenChannelEnd = channelSize * 2;
        var blueChannelEnd = channelSize * 3;
        var server = BinaryServer({port: 9000, path: '/binary-load-image'});

        server.on('connection', function(client){
            client.on('close', function(){
                server.close();
            });
            var bs=new BufferStream(buffer);
            var stream = client.createStream({planes: buffer.planes});
            bs.pipe(stream);
        });

            this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><canvas id=\"photoplate\" draggable=\"true\" width=\""+buffer.width+"\" height=\""+buffer.height+"\"></script><script type=\"text/javascript\" src=\"js/photoplate.js\"></script></canvas></div>";
        parent.close();
        file.close();

    }
    else this.body = "";
};

module.exports.tableEditor = function * tableEditor(path) {
    if ('GET' != this.method) return yield next;
    //console.dir("load editor"+path);
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
    if(stem && leaf){
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var columnTable=h5tb.readTable(parent.id, leaf);
        var table ="<table id=\"h5-table\" draggable=\"true\">\n";
//        var csvDoc="";
        table +="<thead><tr>";
        for (var columnIndex=0;columnIndex<columnTable.length;columnIndex++) {
                table +="<th>"+columnTable[columnIndex].name+"</th>";

        }
        table +="</tr></thead><tbody>";
        for(var rowIndex=0;rowIndex<columnTable[0].length;rowIndex++)
        {
            table +="<tr>";
            for (var columnIndex=0;columnIndex<columnTable.length;columnIndex++) {
                table +="<td>"+columnTable[columnIndex][rowIndex]+"</td>";
//                csvDoc+=columnTable[columnIndex][rowIndex];
//                if(columnIndex<columnTable.length-1)csvDoc+=",";
            }
            table +="</tr>\n";
//            csvDoc+="\n";
        }
        table +="</tbody></table>\n";
        console.dir(tableAddons);
        if(Object.keys(tableAddons).length>0){
            for (var addon in tableAddons){
                var te=yield tableAddons[ addon ].load(columnTable);
                if(te.multiview){
                    this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\">" +
                            "<article class=\"tabs\">	<section id=\"tab1\">" +
                            "<h2><a href=\"#tab1\">Table</a></h2><div id=\"editor-table\"><form>"+table+"</form></div></section>" +
                            "<section id=\"tab2\">" +
                            "<h2><a href=\"#tab2\">Chart</a></h2>"+te.body+"</section></article></div>";
                }
                else{
                    this.body=te.body;
                }
            }
        }
        else{
            this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\">" +
                    "<div id=\"editor-table\"><form>"+table+"</form></div></section>";

        }
        parent.close();
        file.close();

    }
    else this.body = "";
};

module.exports.packetsEditor = function * packetsEditor(path) {
    if ('GET' != this.method) return yield next;
    //console.dir("load editor"+path);
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
    if(stem && leaf){
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var packetTable=h5pt.readTable(parent.id, leaf);
            var table ="<table draggable=\"true\">\n";
            var csvDoc="";
                table +="<tr>";
                for (var name in packetTable.record) {
                    table +="<th>"+name+"</th>";
                }
                table +="</tr>";
                while(packetTable.next()){
                table +="<tr>";
                var columns=Object.keys(packetTable.record).length;
                var j=0;
                for (var name in packetTable.record) {
                    table +="<td>"+packetTable.record[name]+"</td>";
                    csvDoc+=packetTable.record[name];
                    if(j<columns-1)csvDoc+=",";
                    j++;
                }
                table +="</tr>\n";
                csvDoc+="\n";
            }
            packetTable.close();
            table +="</table>\n";
            this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><form>"+table+"</form></div>";
    //                            unirest.post('http://localhost:8000/_/example2')
    //                            .headers({ 'Content-Type': 'text/csv' })
    //                            .send(csvDoc)
    //                            .end(function (response) {
    ////                              console.log(response.body);
    //                            });
        parent.close();
        file.close();

    }
    else this.body = "";
};

module.exports.textEditor = function * textEditor(path) {
    if ('GET' != this.method) return yield next;
    console.dir("load text editor"+path);
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
    if(stem && leaf){
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var document=h5lt.readDataset(parent.id, leaf);
            this.body = "<div id=\"editor\" dropzone=\"copy file:image/png file:image/gif file:image/jpeg file:text/csv\" ondragstart=\"dragStartHandler(event, this)\" ondrag=\"dragHandler(event, this)\" ondragend=\"dragEndHandler(event, this)\" ondrop=\"dropHandler(event, this)\"><form><textarea type=\"text\" name=\"username\" id=\"editortextarea\" draggable=\"true\">"+document+"</textarea></form></div>";
        parent.close();
        file.close();

    }
    else this.body = "";
};

module.exports.attributesEditor = function * attributesEditor(path) {
    if ('GET' != this.method) return yield next;
    //console.dir("load editor"+path);
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
    var attrText="";
    if(stem && leaf){
        var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
        var parent=file.openGroup(stem);
        var childType = parent.getChildType(leaf);
        switch (childType)
        {
            case H5OType.H5O_TYPE_GROUP:
                var child=parent.openGroup(leaf);
                child.refresh();
                Object.getOwnPropertyNames(child).forEach(function(val, idx, array) {
                    if(val!="id")attrText+=val+" :  "+child[val]+"\n";
                });
                child.close();
                break;
            case H5OType.H5O_TYPE_DATASET:
                var attrs=parent.getDatasetAttributes(leaf);
                Object.getOwnPropertyNames(attrs).forEach(function(val, idx, array) {
                    if(val!="id"){
                        if(attrs[val].constructor.name===Array){
                            attrText+=val+" :  ";
                            for(var mIndex=0;mIndex<attrs[val].Length();mIndex++){
                                attrText+=attrs[val][mIndex];
                                if(mIndex<attrs[val].Length()-1)attrText+=",";
                            }
                        }
                        else{
                            attrText+=val+" :  "+attrs[val]+"\n";
                        }
                    }
                });
                break;
        }
        parent.close();
        file.close();
    }
    this.body = attrText;
};


