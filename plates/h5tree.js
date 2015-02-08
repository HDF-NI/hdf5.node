$(function () {
    $('#h5tree').jstree(${treedata}).on('delete_node.jstree', function (e, data) {
					$.get('?operation=delete_node', { 'id' : data.node.id })
						.fail(function () {
							data.instance.refresh();
						});
				})
				.on('create_node.jstree', function (e, data) {
					$.get('?operation=create_node', { 'type' : data.node.type, 'id' : data.node.parent, 'text' : data.node.text })
						.done(function (d) {
							data.instance.set_id(data.node, d.id);
						})
						.fail(function () {
							data.instance.refresh();
						});
				})
				.on('rename_node.jstree', function (e, data) {
					$.get('?operation=rename_node', { 'id' : data.node.id, 'text' : data.text })
						.done(function (d) {
							data.instance.set_id(data.node, d.id);
						})
						.fail(function () {
							data.instance.refresh();
						});
				})
				.on('move_node.jstree', function (e, data) {
					$.get('?operation=move_node', { 'id' : data.node.id, 'parent' : data.parent })
						.done(function (d) {
							//data.instance.load_node(data.parent);
							data.instance.refresh();
						})
						.fail(function () {
							data.instance.refresh();
						});
				})
				.on('copy_node.jstree', function (e, data) {
					$.get('?operation=copy_node', { 'id' : data.original.id, 'parent' : data.parent })
						.done(function (d) {
							//data.instance.load_node(data.parent);
							data.instance.refresh();
						})
						.fail(function () {
							data.instance.refresh();
						});
				})
				.on('changed.jstree', function (e, data) {
					if(data && data.selected && data.selected.length) {
						$.get('?operation=get_content&id=' + data.selected.join(':'), function (d) {
							if(d && typeof d.type !== 'undefined') {
								$('#data .content').hide();
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
										$('#data .code').show();
										$('#code').val(d.content);
										break;
									case 'png':
									case 'jpg':
									case 'jpeg':
									case 'bmp':
									case 'gif':
										$('#data .image img').one('load', function () { $(this).css({'marginTop':'-' + $(this).height()/2 + 'px','marginLeft':'-' + $(this).width()/2 + 'px'}); }).attr('src',d.content);
										$('#data .image').show();
										break;
									default:
										$('#data .default').html(d.content).show();
										break;
								}
							}
						});
					}
					else {
						$('#data .content').hide();
						$('#data .default').html('Select a file from the tree.').show();
					}
				});
    $('#h5tree').on('select_node.jstree', function (e, data) {
    var names = $('#h5tree').jstree(true).get_path(data.node,'/',false);
    /*names = names.replace(/,/g, '');*/
    var i, j, r = [];
    for(i = 0, j = data.selected.length; i < j; i++) {
      r.push(data.instance.get_node(data.selected[i]).text);
    }
    $('#h5tree').on("hover_node.jstree", function (e, data) {
      data.node.title="This is some information for our tooltip.";
    });
    //$('#main').html('XPath: ' + encodeURIComponent(names));
    $.get("http://localhost:3000/h5editors/"+encodeURIComponent(names), function(data) {if(data.length>0)$('#main').html(data);});
    });
 });