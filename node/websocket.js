const WebSocket = require('ws');

const wss = new WebSocket.Server({
  port: 8080,
  perMessageDeflate: {
    zlibDeflateOptions: {
      // See zlib defaults.
      chunkSize: 1024,
      memLevel: 7,
      level: 3
    },
    zlibInflateOptions: {
      chunkSize: 10 * 1024
    },
    // Other options settable:
    clientNoContextTakeover: true, // Defaults to negotiated value.
    serverNoContextTakeover: true, // Defaults to negotiated value.
    serverMaxWindowBits: 10, // Defaults to negotiated value.
    // Below options specified as default values.
    concurrencyLimit: 10, // Limits zlib concurrency for perf.
    threshold: 2048 // Size (in bytes) below which messages
    // should not be compressed.
  }
});

wss.on('connection', function connection(ws,req) {
    let ip = req.socket.remoteAddress;
    console.log(' new connection ', ip )

    ws.on('message', function incoming(message) {
      console.log('received: %s', message);
    });
  
    ws.send('welcome');

    ws.on('close', function close(){
      console.log(' WS disconnected ', ip);
    });

  });



  function scale(x,min,max,omin,omax){
    let prop = (x-min)/(max-min);
    return omin + (omax-omin) * prop;
}
function limit(x,min,max){
    if(x<min)
        return min;
    if(x>max)
        return max;
    
    return x;
}

function expon(x,min,max,ymin,ymax){
    let prop = (x-min)/(max-min);
    return ymin + (ymax-ymin) * prop * prop;
}

  var len = 72*3;
  var x =0;
  setInterval(()=>{
      let buff = Buffer.alloc(len);
      for(let i=0;i<len;i+=3){
          let val = 50+50*Math.sin((x+i)/10);
          val = expon(val,0,100,0,250)

          buff.writeUInt8(val,i);
          buff.writeUInt8(0,i+1);
          buff.writeUInt8(0,i+2);
      }
      x++;

      // ws.send(buff);
      wss.clients.forEach(function each(client) {
          if (client.readyState === WebSocket.OPEN) {
            client.send(buff);
          }
        });
    
  },50)
