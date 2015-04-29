var fs = require('fs');
var hdf5 = require('hdf5');
var koa = require('koa');
var route = require('koa-route');
var serve = require('koa-static');
var MainView = require('views/MainView.js');

var Access = require('lib/globals').Access;

var h5editors = require('lib/h5editors.js');
var h5modifiers = require('lib/h5modifiers.js');

global.port = process.argv[2];
global.currentH5Path = process.argv[3];
var app = koa();


var H5Tree = require('lib/h5tree.js');

app.use(serve('public'));

app.use(route.get('/h5editors/:path', h5editors.load));
app.use(route.post('/create_node/:path', h5modifiers.create));
app.use(route.post('/rename_node/:path', h5modifiers.rename));
app.use(route.post('/move_node/:path', h5modifiers.move));
app.use(route.post('/copy_node/:path', h5modifiers.copy));
app.use(route.post('/paste_node/:path', h5modifiers.paste));
app.use(route.post('/delete_node/:path', h5modifiers.deleteit));
app.use(route.post('/make_image/:path', h5modifiers.makeImage));
app.use(route.post('/make_table/:path', h5modifiers.makeTable));

app.use(function *(){
    this.type = 'html';
    this.body=new MainView(this, global.currentH5Path);
});

if(!fs.existsSync(currentH5Path)){
    var file = new hdf5.File(currentH5Path, Access.ACC_TRUNC);
    file.close();
}

app.listen(global.port);