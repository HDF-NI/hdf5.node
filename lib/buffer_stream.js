var util = require('util');
var Readable = require('stream').Readable;


 var BufferStream = function (source) {

 if ( ! Buffer.isBuffer( source ) ) {
 
throw( new Error( "Source must be a buffer." ) );
 
}
  
    var options={ objectMode: false };
//    if(args.length>0)options=args[0];
    Readable.call(this, options);
    this._source = source;
    this._offset = 0;
    this._length = source.length;
};

util.inherits(BufferStream, Readable);

BufferStream.prototype._read = function( size ) {
 
if ( this._offset < this._length ) {
this.push( this._source.slice( this._offset, ( this._offset + size ) ) );
this._offset += size;
}

if ( this._offset >= this._length ) {
 
this.push( null );
 
}
 
};

// export the class
module.exports = BufferStream;
