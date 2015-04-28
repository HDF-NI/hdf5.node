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

module.exports.load = function * load(path) {
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
    var attrIndex=leaf.lastIndexOf("#");
        if(attrIndex>0){
            var attrText="";
            leaf=leaf.substring(0, attrIndex);
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
                    break;
            }
            parent.close();
            file.close();
        }
        this.body = attrText;
        return;
    }
    if(stem && leaf){
    //console.dir(stem+" on to h5 "+leaf);
    var file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
    var parent=file.openGroup(stem);
        var childType = parent.getChildType(leaf);
        switch (childType)
        {
//            case H5OType.H5O_TYPE_GROUP:
//                break;
            case H5OType.H5O_TYPE_DATASET:
                var datasetType = parent.getDatasetType(leaf);
                switch(datasetType)
                {
                    case HLType.HL_TYPE_PACKET_TABLE:
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
                                for (var name in packetTable.record) {
                                    table +="<td>"+packetTable.record[name]+"</td>";
                                    csvDoc+=packetTable.record[name];
                                    if(j<columns-1)csvDoc+=",";
                                }
                                table +="</tr>\n";
                                csvDoc+="\n";
                            }
                            packetTable.close();
                            table +="</table>\n";
                            this.body = "<div id=\"editor\"><form>"+table+"</form></div>";
//                            unirest.post('http://localhost:8000/_/example2')
//                            .headers({ 'Content-Type': 'text/csv' })
//                            .send(csvDoc)
//                            .end(function (response) {
////                              console.log(response.body);
//                            });
                        break;
                    case HLType.HL_TYPE_TABLE:
                        var columnTable=h5tb.readTable(parent.id, leaf);
                            var table ="<table draggable=\"true\">\n";
                            var csvDoc="";
                                table +="<tr>";
                            for (var columnIndex=0;columnIndex<columnTable.length;columnIndex++) {
                                    table +="<th>"+columnTable[columnIndex].name+"</th>";
                                
                            }
                                table +="</tr>";
                            for(var rowIndex=0;rowIndex<columnTable[0].length;rowIndex++)
                            {
                                table +="<tr>";
                                for (var columnIndex=0;columnIndex<columnTable.length;columnIndex++) {
                                    table +="<td>"+columnTable[columnIndex][rowIndex]+"</td>";
                                    csvDoc+=columnTable[columnIndex][rowIndex];
                                    if(j<columnTable.length-1)csvDoc+=",";
                                }
                                table +="</tr>\n";
                                csvDoc+="\n";
                            }
                            table +="</table>\n";
                            this.body = "<div id=\"editor\"><form>"+table+"</form></div>";
                        break;
                    case HLType.HL_TYPE_IMAGE:
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
                        
                            this.body = "<div id=\"editor\"><canvas id=\"photoplate\" draggable=\"true\" width=\""+buffer.width+"\" height=\""+buffer.height+"\"></script><script type=\"text/javascript\" src=\"js/photoplate.js\"></script></canvas></div>";
                        break;
                    default:
                    var document=h5lt.readDataset(parent.id, leaf);
                    if(document instanceof Float64Array || document instanceof Int32Array || document instanceof Uint32Array)
                    {
                        switch(document.rank)
                        {
                            case 3:
                            var sections="";
                            var csvDoc="";
                            console.dir("sec "+document.sections+" "+document.rows+" "+document.columns);
                            this.body = "<div id=\"editor\"><form>";
    //                        for(var k=0;k<document.sections;k++)
                            {
                            var streamTable=new StreamTable(document.rows, document.columns);
                            this.body=streamTable;
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
                            this.body = "<div id=\"editor\"><form>"+table+"</form></div>";
                            unirest.post('http://localhost:8000/_/example2')
                            .headers({ 'Content-Type': 'text/csv' })
                            .send(csvDoc)
                            .end(function (response) {
                              console.log(response.body);
                            });
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
                        this.body = "<div id=\"editor\"><form><textarea type=\"text\" name=\"username\" id=\"editortextarea\" draggable=\"true\">"+document+"</textarea></form></div>";
                    }
                break;
            }
            break;
            default:
                this.body = "";
            break;
        }
        parent.close();
        file.close();

    }
    else this.body = "";
};

