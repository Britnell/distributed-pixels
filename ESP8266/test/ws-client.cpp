/*
 * 
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"

#define NUM_PIXELS  17
#define PRINTOUT    true

#define PIN         2

#define _D      if(PRINTOUT)
#define p(x)    _D Serial.print(x)
#define pl(x)   _D Serial.println(x)

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

long ticker;
int pid = 0;

void writePixels(uint8_t* buffer, size_t length){
  uint8_t r,g;
  int pix = 0;
  char col = 'r';

  for(int x=0;x<length; x++){
    if(col=='r'){
      r = buffer[x];
      col = 'g';
    }
    else if(col=='g'){
      g = buffer[x];
      col = 'b';
    }
    else {
      if(pix<NUM_PIXELS)
        pixels.setPixelColor(pix,pixels.Color(r,g,buffer[x]));
      pix++;
      col = 'r';
    }
  }
  pixels.show();
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  p(" WS-EVENT :\t");
	switch(type) {
		case WStype_DISCONNECTED:
			p("[WSc] Disconnected! : ");
      pl(WiFiMulti.run());

			break;
		case WStype_CONNECTED: {
			_D Serial.printf("[WSc] connected URL : %s \n",payload);

			webSocket.sendTXT("ESP-Connected");
		}
			break;
		case WStype_TEXT:
			_D Serial.printf("[WSc] get text: %s \n",payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
      p("BIN-Package ");       pl(pid++ );

      writePixels(payload,length);

      // for(int i=0;i<10;i++){
      //   p(payload[i]); p(",");
      // }      pl();

			break;
        case WStype_PING:
          Serial.printf("[WSc] get ping\n");
          break;
        case WStype_PONG:
          Serial.printf("[WSc] get pong\n");
          break;
    }

}

void setup() {
  pixels.begin(); 
 
  // * 
	_D Serial.begin(115200);
	_D Serial.setDebugOutput(true);
  _D p("\n\n\n");

	for(uint8_t t = 4; t > 0; t--) {
		p("[SETUP] BOOT WAIT %d...\n");
		_D Serial.flush();
		delay(1000);
	}

	WiFiMulti.addAP(ssid,password);

	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	webSocket.begin("192.168.188.59", 8080, "/");

	webSocket.onEvent(webSocketEvent);

	webSocket.setReconnectInterval(1000);
  
  webSocket.enableHeartbeat(15000, 3000, 2);

}

void loop() {
	webSocket.loop();
}
