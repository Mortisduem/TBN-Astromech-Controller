/*
Audio Control to be located in the body of the droid.
  - Converts I2C bytes as commands to Serial instructions for a DY MP3 Player
  - I2C Adress is set in Device_Adresses.h
  - 2nd to last Byte is Volume, Last Byte is Track to Play. Other Bytes will be dropped
*/
#include <Wire.h>
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "d:\GitHub\TBN-Astromech-Controller\config\Device_Adresses.h"
#include "DYPlayerArduino.h"


byte new_track;
byte new_vol = 20;
unsigned long currentMillis;
unsigned long last_play;
byte last_track;
byte last_vol = 20;
int audio_delay;
DY::Player player;

void setup() {
  //mode = random(0,10);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.
  Wire.begin(body_audio);          // join i2c bus with address for the audio body board
  Wire.onReceive(receiveEvent);    // Updade
  player.begin(); 
  player.setPlayingDevice(DY::Device::Sd);
  audio_delay = loop_delay * audio_mult;
}

void loop() {
	unsigned int beta;
	currentMillis = millis();
	if (new_track != last_track){
		player.playSpecified(int(new_track));
		last_track = new_track;
	}
	beta = (millis()- currentMillis);
	if (beta <= loop_delay){
		//Serial.println("\tLoop time:" + String(beta) + "ms, delaying for " + String(loop_delay) + "ms loops");
		delay(loop_delay - beta);
	}
	else {
	Serial.println("Error: Long loop of " + String(beta) + "ms is longer then " + String(loop_delay) + "ms target");
	}
}

void playsound(){
  if (new_track != last_track){
    player.playSpecified(int(new_track));
    last_track = new_track;
  }
  last_play = currentMillis + audio_delay;
}


 void receiveEvent(int howMany)
{
  Serial.println("Got data!");
  while(1 < Wire.available()) // loop through all but the last
  {
    new_vol = Wire.read(); // receive byte as a integer
    Serial.println("\tv- " + String(new_vol));
    if (new_vol != last_vol){
      last_vol = new_vol;
      player.setVolume(last_vol);
    }
  }
  new_track = Wire.read();    // receive byte as an integer
	Serial.println("\tt- " + String(new_track));
  if (currentMillis >= last_play){
	  last_track = 0;
  }
}