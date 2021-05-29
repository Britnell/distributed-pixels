#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <Adafruit_NeoPixel.h>
#include <Hash.h>
#include "secrets.h"

#define MQTT_HOST IPAddress(192, 168, 188, 68)
#define MQTT_PORT 1883

#define p(x)    Serial.print(x)
#define pl(x)   Serial.println(x)

#define PIN         2
#define NUM_PIXELS  30

const int BUFFER_SIZE = 1600;
const int chunks = 90+1;
const int period = 50;
const char* clientID = "ESP-2";

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);


AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  // Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
}

void connectToMqtt() {
  Serial.println("Re-Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // 
  wifiReconnectTimer.once(2, connectToWifi);
}


void onMqttConnect(bool sessionPresent) {
  // pl("Re-Connected to MQTT.");

  uint16_t packetIdSub = mqttClient.subscribe("l", 2);
  p("Subscribing to LED : 'l' ");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.attach(1,connectToMqtt);
    // mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  //
}

void onMqttUnsubscribe(uint16_t packetId) {
  //
}



byte buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;
int lang = 0;

void tailcrement(){
  if(++tail==BUFFER_SIZE) tail = 0;
}
void headcrement(){
  if(++head==BUFFER_SIZE) head = 0;
}

int printout = 0;

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  
  // if(lang+len<BUFFER_SIZE)
  if(topic[0]=='l')
  {
    for(unsigned int p=0; p<len; p++){
      buffer[tail] = (int)(payload[p]);
      tailcrement();
    }
    lang += len;
    // if(++printout==10){
    if(1){
      p("\t BUffer : "); pl(lang);
      printout = 0;
    }
  }
}

void onMqttPublish(uint16_t packetId) {
  //
}

void setup() {
  Serial.begin(115200);
  pixels.begin(); 

  Serial.println();
  Serial.println();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setClientId(clientID);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}



byte bite[chunks+1];
uint8_t r, g, col = 0;
int pix=0;

void loop() {

  // get chunk 
  if(lang>=0){
    
    // for(int x=0;x<chunks;x++){  

    byte bit = buffer[head];

    if(bit==255){
      // p(" FLUSH , L: "); pl(lang); 
      pixels.show();
      pix = 0;        col = 0;
      delay(period);
    }
    else {
      if(col==0){
        r = bit;          col = 1;
      }
      else if(col==1){
        g = bit;          col = 2;
      }
      else {
        pixels.setPixelColor(pix,pixels.Color(r,g,bit) );
        pix++;              col = 0;
      }
    }
    headcrement();
    lang--;
    // }
    // lang -= chunks;
  }

}
