var hdf5 = require('hdf5');
var h5lt = require('h5lt');

var Access = require('lib/globals').Access;
var CreationOrder = require('lib/globals').CreationOrder;
var State = require('lib/globals').State;
var H5OType = require('lib/globals').H5OType;

var H5Tree = function (filePath) {
    this.filePath = filePath;
}

H5Tree.prototype.grow = function *(){
    var h5Metadata = yield this.loadH5Metadata();
  return '<div id="h5tree"></div>\n<script src="js/jsTree/libs/jquery-2.1.1.js"></script><script src="js/jsTree/libs/jquery.json.min.js"></script><script src="js/jsTree/jstree.js"></script><script>' +
  '$(function () {' +
    '$(\'#h5tree\').jstree({' +
		'\"core\" : {' +
			'\"data\" :'+h5Metadata+'}' +
	'});\n' +
    '$(\'#h5tree\').on("changed.jstree", function (e, data) {' +
    '  console.log(data.selected);' +
    '});\n' +
    '$(\'#h5tree\').on(\'select_node.jstree\', function (e, data) {' +
    'var names = $(\'#h5tree\').jstree(true).get_path(data.node,\'/\',false);' +
    '/*names = names.replace(/,/g, \'\');*/' +
    'var i, j, r = [];' +
    'for(i = 0, j = data.selected.length; i < j; i++) {' +
    '  r.push(data.instance.get_node(data.selected[i]).text);' +
    '}\n' +
    '$(\'#h5tree\').on("hover_node.jstree", function (e, data) {' +
    '  data.node.title="This is some information for our tooltip.";' +
    '});\n' +
    '//$(\'#main\').html(\'XPath: \' + encodeURIComponent(names));\n' +
    '$.get("http://localhost:3000/h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$(\'#main\').html(data);});\n' +
    '});\n' +
 '});' +
  '</script>';
};

H5Tree.prototype.filePath = "";

H5Tree.prototype.loadH5Metadata = function *(){
    var file = new hdf5.File(this.filePath, Access.ACC_RDONLY);
    var array=file.getMemberNamesByCreationOrder();
    var jsonDoc="[\n{\n";
    for (var arrayIndex = 0; arrayIndex < array.length; arrayIndex++)
    {
        var tool=file.openGroup(array[arrayIndex]);
        var branch = yield this.loadRecursiveGroup(tool);
        jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n\"state\" : {" +
           "\"opened\" : true,\n" +
           "\"selected\" : false\n" +
         "},\"children\" : [\n";
        jsonDoc+=branch;
        jsonDoc+="]\n";
        if(arrayIndex<array.length-1)jsonDoc+=",\n";
    }
    jsonDoc+="}\n]\n";
    file.close();
    return jsonDoc;
};

H5Tree.prototype.loadRecursiveGroup = function *(group){
    var array=group.getMemberNamesByCreationOrder();
    
    var jsonDoc="{\n";
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
                //console.dir("loadRecursiveGroup #objs "+child.getNumObjs()+" "+array[arrayIndex]);
                if(child.getNumObjs()>0)
                {
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\",\n";
                    jsonDoc+="\"children\" : [\n";
                    jsonDoc+= yield this.loadRecursiveGroup(child);
                    jsonDoc+="]\n,";
                    jsonDoc+="\"data\" : {"+a_attrs+"}\n";
                }
                else
                {
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\"\n";
                }
                break;
            default:
                    jsonDoc+="\"text\" : \""+array[arrayIndex]+"\"\n";
                break;
        }
        if(arrayIndex<array.length-1)jsonDoc+="},{\n";
    }
    jsonDoc+="}\n";
    return jsonDoc;
};

// export the class
module.exports = H5Tree;