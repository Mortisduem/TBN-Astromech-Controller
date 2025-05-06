/*
Audio Control to be located in the body of the droid.
  - Converts I2C bytes as commands to Serial instructions for a DY MP3 Player
  - I2C Adress is set in Device_Adresses.h
  - 2nd to last Byte is Volume, Last Byte is Track to Play. Other Bytes will be dropped
  - Serial is not available for debug due to use of CY player on the serial
  - counts of tracks are set in \config\Device_Adresses.h
*/
#include <Wire.h> 
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <EnableInterrupt.h>

//Correct Path as needed to point to the correct files
//#include "d:\GitHub\TBN-Astromech-Controller\config\Device_Adresses.h"
#include "DYPlayerArduino.h"

/* Constants for Audio file counts, Audio board and Hub board currently only use*/

const int loop_delay = 16; 
const int sound_effects = 41;
const int sound_songs = 10;

#define RC_NUM_CHANNELS  6

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_CH5  4
#define RC_CH6  5


// Inputs from the RC receiver
#define RC_CH1_INPUT  A0 //Head left/right
#define RC_CH2_INPUT  A1 //Forwards/Backwards
#define RC_CH3_INPUT  A2 //Head front/back
#define RC_CH4_INPUT  A3 //Left/Right Steering
#define RC_CH5_INPUT  A4 //Warning Light lift
#define RC_CH6_INPUT  A5 //spare channel

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

#define INPUT_SIZE 30

byte inch1, inch2, inch3, inch4, inch5, inch6;

float CH1, CH2, CH3, CH4, CH5, CH6;

byte new_track;
byte new_vol = 20;
unsigned long currentMillis;
unsigned long last_play;
unsigned long check_audio;
byte last_track;
byte last_vol = 20;
int audio_delay;
DY::Player player;
byte Audio_Volume;

void setup() {
  //mode = random(0,10);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.
  player.begin(); 
  player.setPlayingDevice(DY::Device::Sd);
  audio_delay = loop_delay;
  
  pinMode(RC_CH1_INPUT, INPUT);
  pinMode(RC_CH2_INPUT, INPUT);
  pinMode(RC_CH3_INPUT, INPUT);
  pinMode(RC_CH4_INPUT, INPUT);
  pinMode(RC_CH5_INPUT, INPUT);
  pinMode(RC_CH6_INPUT, INPUT);



}

void loop() {
	unsigned int beta;
	currentMillis = millis();
	if (new_track != last_track){
		player.playSpecified(int(new_track));
		last_track = new_track;
	}
	//rc_read_values();
	
	
	beta = (millis()- currentMillis);
	if (beta <= loop_delay){
		//Serial.println("\tLoop time:" + String(beta) + "ms, delaying for " + String(loop_delay) + "ms loops");
		delay(loop_delay - beta);
	}
	else {
	Serial.println("Error: Long loop of " + String(beta) + "ms is longer then " + String(loop_delay) + "ms target");
	}
  audio();
}

void audio() { 			// Audio Control
	Audio_Volume 	= 	map(inch2, 1000, 2000, 0,30);
	int track;
  Serial.println(inch1);
	if (inch1 != 1333 and currentMillis >= check_audio){
		check_audio = currentMillis + audio_delay;
		if (inch5 = 1333){
			//Serial.print("Low Sound bank selected: ");
			track = map(inch1,0000,2000,0,int(sound_effects/2)) + sound_songs + 1;
		}
		else if (inch5 >= 1666) {
			//Serial.print("Music bank selected: ");
			track = map(inch1,0000,2000,0,sound_songs);
		}
		else {
			//Serial.print("High sound bank selected: ");
			track = map(inch1,0000,2000,int(sound_effects/2),sound_effects) + sound_songs + 1;
		}
		//Serial.println(" Playing file " + String(track));
		playsound(track);
	}
}

void playsound(int new_track) {
  if (new_track != last_track){
    player.playSpecified(int(new_track));
    last_track = new_track;
  }
  last_play = currentMillis + audio_delay;
}


void Poll_RC(){
  inch1  = readChannel( CH1, -100 , 100, 0); 
  inch2  = readChannel( CH2, -100 , 100, 0);
  inch3  = readChannel( CH3, 0,30,0);
  inch4  = readChannel( CH4, -150 , 150, 0);   //Dome Move
  inch5  = readChannel( CH5, 0, sound_effects, 0);
  inch6  = readChannel( CH6, 0, 1, 0);
  //RC_Chan7  = readChannel( CH7, -100 , 100, 0);
  //RC_Chan8  = readChannel( CH8, -100 , 100, 0);
  //RC_Chan9  = readChannel( CH9, -100 , 100, 0);
  //RC_Chan10 = readChannel(CH10, -100 , 100, 0);

}

int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}