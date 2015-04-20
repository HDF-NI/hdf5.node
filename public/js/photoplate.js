        var context = document.getElementById('photoplate').getContext("2d");

         var img2 = new Image();
         img2.onload = function () {
             context.drawImage(img2, 0, 0);
         }
         //img2.src = "img/test.png";
    // connect to the same host this was served from
    var client = new BinaryClient('ws://localhost:9000/binary-endpoint');

     // Received new stream from server!
     client.on('stream', function(stream, meta){    
       // Buffer for parts
       var parts = [];
       // Got new data
       stream.on('data', function(data){
         parts.push(data);
       });
       stream.on('end', function(){
         // Display new data in browser!
         img2.src =(window.URL || window.webkitURL).createObjectURL(new Blob(parts));
       });
     });
     if(!binaryFeatures.supportsBinaryWebsockets){
       document.write('Your browser doesn\'t support binary websockets and is not yet supported by BinaryJS. Fallbacks are in the works but not yet complete. Chrome 15+ and Firefox 11+ are known to work.');
     } else if (binaryFeatures.useBlobBuilder) {
       window.location = '/hw_bb.html';
     }
