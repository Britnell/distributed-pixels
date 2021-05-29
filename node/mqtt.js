

const mqtt = require('mqtt')
const client  = mqtt.connect('mqtt://ancientpi.local')
 

// * * * * * * * * * * * * * * * * * * * 

const N_LEDS = 30
const T_LEDS = 90

const PERIOD = 55
const N_FRAMES = 6
const frames = []


// * * * * * * * * * * * * * * * * * * * 

client.on('connect', function () {
    client.publish('t', 'node re-connected ')

    client.subscribe('t', function (err) { })
    client.subscribe('l', function (err) { })

})
 
var last = 0

client.on('message', function (topic, message) {
    if(topic==='t'){
        // console.log(' #T - ', message.toString())
    }
    else if(topic==='l'){
        console.log(' #LEDS - (period) ', Date.now()-last )
        last = Date.now()
    }
  
})


for(let x=0;x<N_FRAMES; x++){
    let buff = Buffer.alloc(T_LEDS+1)
    buff.writeUInt8(255,T_LEDS);
    frames.push(buff);
}

function sendLEDS(){
    let message = Buffer.concat(frames);
    client.publish('l',message);
    // console.log(' SEN FRAME ', message.length )
}

var x = 0;
var a = 0;


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

var x=0;
var f=0;

function expon(x,min,max,ymin,ymax){
    let prop = (x-min)/(max-min);
    return ymin + (ymax-ymin) * prop * prop;
}

setInterval(()=>{
    // let x = Math.floor(Math.sin(a) * 128);

    // create frame
    for(let l=0;l<N_LEDS;l++){
        // let v = (l==x)?100:0
        let v = Math.floor( 50+50 * Math.sin(Math.PI*2* (x-l+1)/N_LEDS*2 ))
        frames[f].writeUInt8(expon(v,0,100,0,250),l*3+0)
        frames[f].writeUInt8(0,l*3+1)
        frames[f].writeUInt8(0,l*3+2)
    }
    frames[f].writeUInt8(255,T_LEDS);
    
    x += 0.1;
    // if(x>=N_LEDS)   x=0;      

    if(++f==N_FRAMES){
        sendLEDS()
        f=0;
    }

}, PERIOD )
  

// client.end()
// var v1 = Math.floor(127 * Math.abs(Math.cos(x/50)) );
// var v2 = Math.floor(127 * Math.abs(Math.sin(x/50)) );
// LEDS.writeUInt8(v1,b);
// LEDS.writeUInt8(v2,b+1);
// LEDS.writeUInt8(0,b+2);


// for(var b=0;b<N_LEDS;b++){
//     let dist = Math.abs(x-b)
//     let val = 0;
//     if(dist<rad){
//         val = scale(dist,0,rad,255,0);
//         val = limit(val,0,255);
//     }
//     LEDS.writeUInt8(val,b*3+0)
//     LEDS.writeUInt8(val/4,b*3+1)
//     LEDS.writeUInt8(0,b*3+2)
// }
// console.log(LEDS)
// sendLEDS(LEDS);
// x+= 0.3;
// if(x>=N_LEDS)   x=0;