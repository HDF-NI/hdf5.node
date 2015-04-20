var koa = require('koa');
var route = require('koa-route');
var serve = require('koa-static');
var MainView = require('views/MainView.js');

var h5editors = require('lib/h5editors.js');
var h5modifiers = require('lib/h5modifiers.js');

global.port = process.argv[2];
global.currentH5Path = process.argv[3];
var app = koa();


var H5Tree = require('lib/h5tree.js');

app.use(serve('public'));

app.use(route.get('/h5editors/:path', h5editors.load));
app.use(route.post('/h5modifiers/:path', h5modifiers.load));

app.use(function *(){
    this.type = 'html';
    this.body=new MainView(this, global.currentH5Path);
});

app.listen(global.port);