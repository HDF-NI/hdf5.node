        var canvas = document.getElementById('photoplate');
        var context = canvas.getContext("2d");

    // connect to the same host this was served from
    var client = new BinaryClient('ws://localhost:9000/binary-load-image');

     // Received new stream from server!
     client.on('stream', function(stream, meta){    
       var imageData=context.createImageData(canvas.width, canvas.height);
       var pos=0;
       // Got new data
       stream.on('data', function(data){
           var dv = new DataView(data);
//         parts.push(data);
        for(var i=0;i<data.byteLength;i+=meta.planes)
        {
                // set red, green, blue, and alpha:
                imageData.data[pos++] =dv.getUint8(i);
                imageData.data[pos++] = dv.getUint8(i+1);
                imageData.data[pos++] = dv.getUint8(i+2);
                (meta.planes>3) ? imageData.data[pos++] =dv.getUint8(i+3) : imageData.data[pos++] = 255; // opaque alpha
        }
       });
       stream.on('end', function(){
         // Display new data in browser!
            context.putImageData(imageData, 0, 0);
            client.close();
       });
     });
