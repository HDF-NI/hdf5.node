var unirest = require('unirest');
var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;

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
            var document=h5lt.readDataset(parent.id, leaf);
                if(document instanceof Float64Array || document instanceof Int32Array)
                {
                    if(document.rank<=2)
                    {
                        var table ="<table>\n";
                        var csvDoc="";
                        for(var i=0;i<document.rows;i++)
                        {
                            table +="<tr>";
                        for(var j=0;j<document.columns;j++)
                        {
                            table +="<td>"+document[document.columns*i+j]+"</td>";
                            csvDoc+=document[document.columns*i+j];
                            if(j<document.columns-1)csvDoc+=",";
                        }
                            table +="</tr>\n";
                            csvDoc+="\n";
                        }
                        table +="</table>\n";
                        this.body = "<div id=\"editor\"><form>"+table+"</form></div>";
unirest.post('http://roger-g74sx:8000/_/example2')
.headers({ 'Content-Type': 'text/csv' })
.send(csvDoc)
.end(function (response) {
  console.log(response.body);
});                            
                    }
                }
//                else if(document instanceof Int32Array)
//                {
//                    
//                }
                else
                {
                    this.body = "<div id=\"editor\"><form><textarea type=\"text\" name=\"username\" >"+document+"</textarea></form></div>";
                }
            break;
            default:
                this.body = "";
                console.dir(" h5 obj "+childType);
            break;
        }

};

