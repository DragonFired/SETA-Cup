#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini

static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object

DFRobotDFPlayerMini mp3_player;

#define LiftSwitchPin 7
#define LoopSwitchPin 8

int LiftSwitch; 
int LoopSwitch; 
bool LiftTriggered = false;

#include <Adafruit_NeoPixel.h>
#define LEDPIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, LEDPIN, NEO_GRB + NEO_KHZ800);

void setup(){
  // start serial port at 115200 bps:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.write("Welcome to Audio mp3_player v0.7\n");
  pinMode(LiftSwitchPin,INPUT_PULLUP);   // Define the Lift Switch input as digital input.
  pinMode(LoopSwitchPin,INPUT_PULLUP);   // Define the Loop Switch input as digital input.

  LiftTriggered = false;
  softwareSerial.begin(9600);// Start communication with DFPlayer Mini
  bool running = false;

  bool LoopOn = !digitalRead(LoopSwitchPin); 
  if (LoopOn) {
    Serial.println("Looping mode: On");
  }
  else {
    Serial.println("Looping mode: Off");
  }

  while(!running) {
    if (mp3_player.begin(softwareSerial)) {
      Serial.println("Player OK");
      // Set volume to maximum (0 to 30).
      mp3_player.volume(30);
      mp3_player.play(1);
      running = true;
    }
    else {
      Serial.println("Connecting to DFPlayer Mini failed!");
//      running = true;
    }
  }
  strip.begin();
  strip.setBrightness(100);
  strip.show(); // Initialize all pixels to 'off'
}
 
void loop() {  
  uint8_t wait = 20;
  uint8_t startColor = 200;
  uint8_t endColor = 256;
  bool ledsOn = false;
  
  for (int j=startColor; j < 256; j++) {     // cycle all 256 colors in the wheel
    if (checkSwitchPlayAudio()) {
      ledsOn = true;
    }
    else {
      ledsOn = false;
    }
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      if (!ledsOn) {
        strip.setBrightness(0);
        strip.show();
      }
      else {
        strip.setBrightness(100);
        strip.show();
      }

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

int checkSwitchPlayAudio() {
  LiftSwitch = !digitalRead(LiftSwitchPin); 
  LoopSwitch = !digitalRead(LoopSwitchPin); 

  if (LiftSwitch == true) { //if LiftSwitch pressed then play wav file
    if(!LiftTriggered) {
      LiftTriggered = true;
      if (LoopSwitch) {
        mp3_player.loop(1);
      }
      else {
        mp3_player.play(1);
      }
      if(mp3_player.available()) {
        Serial.println("Audio is playing");
      }
    }
  }
  if (LiftSwitch == false) { // if LiftSwitch pressed then play wav file
    if(!mp3_player.available()) {
      mp3_player.pause();      //stops the music
      Serial.println("Audio isn't playing");
    }
    LiftTriggered = false;
  }
  return !mp3_player.available();
}

  
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
