var unirest = require('unirest');
var hdf5 = require('hdf5');
var h5lt = require('h5lt');
var h5tb = require('h5tb');
var h5pt = require('h5pt');

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;
var HLType = require('lib/globals').HLType;
var StreamTable = require('lib/stream_table');

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
                            var table ="<table>\n";
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
                            var table ="<table>\n";
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
                        this.body = "<div id=\"editor\"><form><textarea type=\"text\" name=\"username\" id=\"editortextarea\">"+document+"</textarea></form></div>";
                    }
                break;
            }
            break;
            default:
                this.body = "";
                console.dir(" h5 obj "+childType);
            break;
        }

};

