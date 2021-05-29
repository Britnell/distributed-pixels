#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN         2
#define NUM_PIXELS  10

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Init ");

  pixels.begin(); 

  for( int i=0; i<NUM_PIXELS; i++){
    pixels.setPixelColor(i,pixels.Color(55,0,0));
    pixels.show();
    delay(100);
  }
}

void loop() {
  delay(200);
  // put your main code here, to run repeatedly:
}