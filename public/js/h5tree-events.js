$('#jstree')
  // listen for event
  .on('changed.jstree', function (e, data) {
    var i, j, r = [];
    for(i = 0, j = data.selected.length; i < j; i++) {
      r.push(data.instance.get_node(data.selected[i]).text);
    }
    $('#event_result').html('Selected: ' + r.join(', '));
  })
  // create the instance
  .jstree();
		