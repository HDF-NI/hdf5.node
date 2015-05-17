var co = require('co');
var fs = require('fs');
var path = require('path');
var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;
var HLType = require('lib/globals').HLType;

var H5Tree = function (filePath) {
    this.filePath = filePath;
}

H5Tree.prototype.filePath = "";

H5Tree.prototype.loadH5Metadata = function *(){
    var file = new hdf5.File(this.filePath, Access.ACC_RDONLY);
    var array=file.getMemberNamesByCreationOrder();
    var jsonDoc=Buffer("[\n{\n");
//    if(array.length<=0){
        jsonDoc+="\"text\" : \""+path.basename(this.filePath)+"\",\n";
        jsonDoc+="\"type\" : \"group\",\n";
        jsonDoc+="\"state\" : {" +
           "\"opened\" : true,\n" +
           "\"selected\" : false\n" +
         "},\"children\" : [\n";
        if(array.length>0)jsonDoc+="{\n";
        
//    }
    for (var arrayIndex = 0; arrayIndex < array.length; arrayIndex++)
    {
        var childType = file.getChildType(array[arrayIndex]);
        switch (childType)
        {
            case H5OType.H5O_TYPE_GROUP:
                var tool=file.openGroup(array[arrayIndex]);
                var branch = yield this.loadRecursiveGroup(tool);
                jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                jsonDoc+="\"type\" : \"group\",\n";
                jsonDoc+="\"state\" : {" +
                   "\"opened\" : false,\n" +
                   "\"selected\" : false\n" +
                 "},\"children\" : [\n";
                jsonDoc+=branch;
                jsonDoc+="]\n";
                if(arrayIndex<array.length-1)jsonDoc+="},{\n";
                tool.close();
                break;
            case H5OType.H5O_TYPE_DATASET:
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                    var datasetType = file.getDatasetType(array[arrayIndex]);
                    switch(datasetType){
                        case HLType.HL_TYPE_LITE:
                            jsonDoc+="\"type\" : \"dataset\"\n";
                            break
                        case HLType.HL_TYPE_IMAGE:
                            jsonDoc+="\"type\" : \"image\"\n";
                            break
                        case HLType.HL_TYPE_TABLE:
                            jsonDoc+="\"type\" : \"table\"\n";
                            break
                        case HLType.HL_TYPE_PACKET_TABLE:
                            jsonDoc+="\"type\" : \"packets\"\n";
                            break
                        case HLType.HL_TYPE_TEXT:
                            jsonDoc+="\"type\" : \"text\"\n";
                            break
                        default:
                            jsonDoc+="\"type\" : \"dataset\"\n";
                        break;
                    }
                break;
            default:
                jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                jsonDoc+="\"type\" : \"group\"\n";
                break;
        }
    }
        if(array.length>0)jsonDoc+="}\n";
                jsonDoc+="]\n";
    jsonDoc+="}\n]\n";
    file.close();
    return jsonDoc;
};

H5Tree.prototype.loadRecursiveGroup = function *(group){
    var array=group.getMemberNamesByCreationOrder();
    
    var jsonDoc="";
    if(array.length>0)jsonDoc+="{\n";
    for (var arrayIndex = 0; arrayIndex < array.length; arrayIndex++)
    {
        var childType = group.getChildType(array[arrayIndex]);
        switch (childType)
        {
            case H5OType.H5O_TYPE_GROUP:
                var child=group.openGroup(array[arrayIndex]);
                child.refresh();
//                for((var index=0;index<child.getNumAttrs();index++)
//                {
//                    
//                }
                var a_attrs = "";
                
                for( var property in child)
                    if(child.hasOwnProperty(property) && property !="id")
                    {
                        //a_attrs+="\""+property.replace(/ /g, "_")+"\": \""+child[ property ]+"\" ";
                        a_attrs+="\""+property+"\": \""+child[ property ]+"\" ";
                        a_attrs+=", ";
                    }
                a_attrs = a_attrs.substring(0, a_attrs.length -2);
                jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                if(child.getNumObjs()>0)
                {
                    jsonDoc+="\"type\" : \"group\",\n";
                    jsonDoc+="\"children\" : [\n";
                    jsonDoc+= yield this.loadRecursiveGroup(child);
                    jsonDoc+="]\n,";
                    jsonDoc+="\"data\" : {"+a_attrs+"}\n";
                }
                else
                {
                    jsonDoc+="\"type\" : \"group\"\n";
                }
                child.close();
                break;
             case H5OType.H5O_TYPE_DATASET:
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                    var datasetType = group.getDatasetType(array[arrayIndex]);
                    switch(datasetType){
                        case HLType.HL_TYPE_LITE:
                            jsonDoc+="\"type\" : \"dataset\"\n";
                            break
                        case HLType.HL_TYPE_IMAGE:
                            jsonDoc+="\"type\" : \"image\"\n";
                            break
                        case HLType.HL_TYPE_TABLE:
                            jsonDoc+="\"type\" : \"table\"\n";
                            break
                        case HLType.HL_TYPE_PACKET_TABLE:
                            jsonDoc+="\"type\" : \"packets\"\n";
                            break
                        case HLType.HL_TYPE_TEXT:
                            jsonDoc+="\"type\" : \"text\"\n";
                            break
                        default:
                            jsonDoc+="\"type\" : \"dataset\"\n";
                        break;
                    }
                break;
            default:
                console.dir(childType+" child default for "+array[arrayIndex]);
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\"\n";
                break;
        }
        if(arrayIndex<array.length-1)jsonDoc+="},{\n";
    }
    if(array.length>0)jsonDoc+="}\n";
    return jsonDoc;
};

// export the class
module.exports = H5Tree;
