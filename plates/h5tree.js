var creating=false;
var cutting=false;
$(function () {
    $('#h5tree').jstree(${treedata}).on('delete_node.jstree', function (e, data) {
                                    var branch = $('#h5tree').jstree(true).get_path(data.node);
                                    branch.shift();
                                    var names=branch.join("/");
                                    $.post("/delete_node/"+encodeURIComponent(names), function(returnedData) {

                                    });
				})
                                .on('create_node.jstree', function (e, data) {
                                    creating=true;
				})
				.on('rename_node.jstree', function (e, data) {
                                    var branch = $('#h5tree').jstree(true).get_path(data.node);
                                    branch.shift();
                                    var names=branch.join("/");
                                    if(creating){
                                    $.post("/create_node/"+encodeURIComponent(names), function(returnedData) {

                                    });
                                        $('#h5tree').jstree(true).set_type(data.node, "group");
                                    }
                                    else{
                                        names+="["+data.old+"]";
                                    $.post("/rename_node/"+encodeURIComponent(names), function(returnedData) {

                                    });
                                    }
                                    creating=false;
				})
				.on('move_node.jstree', function (e, data) {
                                    console.log(data.old_parent);
                                    var branch = $('#h5tree').jstree(true).get_path(data.node);
                                    branch.shift();
                                    var names=branch.join("/");
                                    var old_branch = $('#h5tree').jstree(true).get_path(data.old_parent);
                                    old_branch.shift();
                                    var old_names=old_branch.join("/");
                                    console.log(old_names);
                                    $.post("/move_node/"+encodeURIComponent(names+"["+old_names+"]"), function(returnedData) {

                                    });
				})
				.on('cut_node.jstree', function (e, data) {
                                    var branch = $('#h5tree').jstree(true).get_path(data.node);
                                    branch.shift();
                                    var names=branch.join("/");
                                    //$.post("/cut_node/"+encodeURIComponent(names), function(returnedData) {
                                    //});
                                    cutting=true;
				})
				.on('copy_node.jstree', function (e, data) {
                                    var branch = $('#h5tree').jstree(true).get_path(data.node);
                                    branch.shift();
                                    var names=branch.join("/");
                                    console.log("hit copy node ");
                                    var old_branch = $('#h5tree').jstree(true).get_path(data.old_parent);
                                    old_branch.shift();
                                    var old_names=old_branch.join("/");
                                    console.log(old_names);
                                    $.post("/copy_node/"+encodeURIComponent(names+"["+old_names+"]"), function(returnedData) {

                                    });
                                      cutting=false;
				})
//				.on('paste.jstree', function (e, data) {
//                                    console.log("hit paste "+data.mode);
//                                    var branch = data.node;
//                                    branch.shift();
//                                    var names=branch.join("/");
//                                    console.log("hit paste "+cutting);
//                                    if(cutting===true)names+="#cut";
//                                    $.post("/paste_node/"+encodeURIComponent(names), function(returnedData) {
//                                    });
//                                        cutting=false;
//				})
				.on('changed.jstree', function (e, data) {
					if(data && data.selected && data.selected.length) {
						$.get('?operation=get_content&id=' + data.selected.join(':'), function (d) {
							if(d && typeof d.type !== 'undefined') {
//								$('#data .content').hide();
console.log(d.type);
								switch(d.type) {
									case 'text':
									case 'txt':
									case 'md':
									case 'htaccess':
									case 'log':
									case 'sql':
									case 'php':
									case 'js':
									case 'json':
									case 'css':
									case 'html':
//										$('#data .code').show();
//										$('#code').val(d.content);
										break;
									case 'png':
									case 'jpg':
									case 'jpeg':
									case 'bmp':
									case 'gif':
//										$('#data .image img').one('load', function () { $(this).css({'marginTop':'-' + $(this).height()/2 + 'px','marginLeft':'-' + $(this).width()/2 + 'px'}); }).attr('src',d.content);
//										$('#data .image').show();
										break;
									default:
//										$('#data .default').html(d.content).show();
										break;
								}
							}
						});
					}
					else {
//						$('#data .content').hide();
//						$('#data .default').html('Select a file from the tree.').show();
					}
        });
    $('#h5tree').on('select_node.jstree', function (e, data) {
        var branch = $('#h5tree').jstree(true).get_path(data.node);
        branch.shift();
        var names=branch.join("/");
        if(names){
            switch($('#h5tree').jstree(true).get_type(data.node)){
                case "dataset":
                $.get("/dataset_h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
                    break;
                case "image":
                $.get("/image_h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
                    break;
                case "table":
                $.get("/table_h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
                    break;
                case "packets":
                $.get("/packets_h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
                    break;
                case "text":
                $.get("/text_h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
                    break;
            }
        }
    });
    $('#h5tree').on('hover_node.jstree',function(e,data){
        var branch = $('#h5tree').jstree(true).get_path(data.node);
        branch.shift();
        var names=branch.join("/");
//        names+="#attributes";
        $.get("/attributes_h5editors/"+encodeURIComponent(names), function(returnedData) {
            $("#"+data.node.id).prop('title', returnedData);
        });
    });
 });