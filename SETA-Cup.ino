#include <SD.h>                      // need to include the SD library
// Pins usage on Arduino （with SD card）
//   D4: SD_CS
//   D11: SD_DI
//   D12: SD_DO
//   D13: SD_CLK
#define SDChipPin 4

#include <TMRpcm.h>           //  also need to include this library...

#define LiftSwitchPin 7
#define LoopSwitchPin 8
#define AudioOutPin 9

TMRpcm Audio;   // create an object for use in this sketch
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
  Serial.write("Welcome to Audio player v0.6\n");
  pinMode(LiftSwitchPin,INPUT_PULLUP);   // Define the Lift Switch input as digital input.
  pinMode(LoopSwitchPin,INPUT_PULLUP);   // Define the Loop Switch input as digital input.

  LiftTriggered = false;
  Audio.speakerPin = AudioOutPin;
  
  int result = SD.begin(SDChipPin);
  while (!result) {
    Serial.println("Initialization failed!");
    result = SD.begin(SDChipPin);
  }
  Serial.println("Initialization done. SD card available");

  //root = SD.open("/");
  //printDirectory(root, 0);
  Audio.setVolume(5);        // 0 to 7. Set volume level. Distortion for anthing above 5 :-(
  
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
  LiftSwitch = digitalRead(LiftSwitchPin); 
  LoopSwitch = !digitalRead(LoopSwitchPin); 

  if (LiftSwitch == LOW) { //if LiftSwitch pressed then play wav file
    if(!LiftTriggered) {
      LiftTriggered = true;
      Audio.play("CCHAMP.wav");
      Audio.loop(LoopSwitch); // 0 or 1. Can be changed during playback for full control of looping.
      if(Audio.isPlaying()) {
        Serial.println("Audio is playing");
      }
    }
  }
  if (LiftSwitch == HIGH) { // if LiftSwitch pressed then play wav file
    if(Audio.isPlaying()) {
      Audio.stopPlayback();      //stops the music, but leaves the timer running
      Serial.println("Audio isn't playing");
    }
    LiftTriggered = false;
  }
  return Audio.isPlaying();
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
