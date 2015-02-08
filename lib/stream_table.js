var util = require('util');
var Transform = require('stream').Transform;

var args;
var rows;
var columns;
var i;
var j;


 var StreamTable = function (rows, columns) {
     this.rows=rows;
     this.columns=columns;
     this.i=0;
     this.j=0;

    var options={ objectMode: true };
    Transform.call(this, options);
};

util.inherits(StreamTable, Transform);


StreamTable.prototype._transform = function(chunk, encoding, done) {
    if(this.i===0 && this.j===0)this.push("<table>\n");
        if(this.j===0)this.push("<tr>");
        this.push("<td>"+chunk+"</td>");
        this.j++;
        if(this.j===this.columns){
            this.i++;
            this.push("</tr>\n"+this.i+" "+this.j+"\n");
            this.j=0;
        }
        if(this.i===this.rows){
            this.i=0;
        }

  done();
};

StreamTable.prototype._flush = function(done) {
  this.push("</tr>\n</table>\n");
  done();
};
// export the class
module.exports = StreamTable;
