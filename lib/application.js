var koa = require('koa');
var route = require('koa-route');
var serve = require('koa-static');
var h5editors = require('lib/h5editors.js');

global.currentH5Path = process.argv[2];
var app = koa();


var H5Tree = require('lib/h5tree.js');

app.use(serve('public'));

app.use(route.get('/h5editors/:path', h5editors.load));

app.use(function *(){
    this.type = 'html';
  this.body = '<html><head>' +
'<meta http-equiv="Content-Language" content="en">' +
          '<meta http-equiv="Content-Type" content="text/html; charset="utf-8">' +
  '<title>jsTree test</title>' +
  '<link href="css/dragbar.css" rel="stylesheet"/>' +
  '<link href="css/h5editing.css" rel="stylesheet"/>' +
  '<link href="css/bootstrap.min.css" rel="stylesheet">' +
  '<!-- 2 load the theme CSS file -->' +
  '<link rel="stylesheet" type="text/css" href="js/jsTree/themes/default/style.min.css" />\n' +
  '<script type="text/javascript" src="js/jsTree/libs/jquery-2.1.1.min.js"></script>' +
  '<!--script type="text/javascript" src="js/jsTree/libs/jquery.cookie.js"></script-->' +
  '<script type="text/javascript" src="js/jquery-ui-1.11.2/jquery-ui.js"></script>' +
  '<script type="text/javascript" src="js/modernizr-latest.js"></script>\n' +
  '<script type="text/javascript">\n' +
  'var $SCRIPT_ROOT = {{ request.script_root|tojson|safe }};\n' +
  '</script>\n' +
  '<script type="text/javascript" src="js/dragbar.js"></script>\n' +
  '</head>';
  var theTree = new H5Tree(currentH5Path);
  var theTreeDoc = yield theTree.grow();
  this.body += '<body><div id="sidebar"><span id="position"></span><div id="dragbar"></div>'+theTreeDoc+'</div><div id="main"> editors/spreadsheets here </div><script src="js/bootstrap.min.js"></script></body></html>';
});

app.listen(3000);