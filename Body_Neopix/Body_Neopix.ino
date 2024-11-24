// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <LedControl.h>
#include "d:\GitHub\TBN-Astromech-Controller\config\Device_Adresses.h"

//Data Door Matrix
#define DIN 12
#define CS 11
#define CLK 10



// Which pin on the Arduino is connected to the NeoPixels?
#define NECK_PIN	3
#define DOOR_PIN	4
#define DATA_PIN	5

// How many NeoPixels are attached to the Arduino?
#define NECK_COUNT  60
#define DOOR_COUNT  18
#define DATA_COUNT	5
//#define ROWS 4
//#define COLS 8
 // NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255 // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel neck(NECK_COUNT, NECK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel doors(DOOR_COUNT, DOOR_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel data(DOOR_COUNT, DOOR_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
LedControl lc = LedControl(DIN,CLK,CS,0);

byte neck_mode =  0;
byte door_mode = 0;
byte neck_led = 0;
byte door_led = 0;
byte start;
byte end;
byte red;
byte green;
byte blue;

const int speed_mult = loop_delay * loop_mult;  //Limit the refresh to a multi of the Hub loop and smooth out scrolling
unsigned long currentMillis;

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(115200);
  Serial.print("Dark Saber: Body NeoPixel & Matrix Control v");
  if (version < 0){
    Serial.println("β."+String(0-version));
  }
  else {
    Serial.println(version);
  }
  Wire.begin(body_lights);         // join i2c bus with adress set in address.h
  Wire.onReceive(receiveEvent);    // Update Modes
  Serial.println("\tI2C adresss: " + String(body_lights));
  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  lc.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
  lc.setIntensity(0,15);       // Set the brightness to maximum value
  lc.clearDisplay(0);          // and clear the display
  //mode = random(0,10);


  neck.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  neck.show();            // Turn OFF all pixels ASAP
  neck.setBrightness(BRIGHTNESS);
  doors.begin(); 
  doors.show();
  doors.setBrightness(BRIGHTNESS);
  data.begin(); 
  data.show();
  data.setBrightness(BRIGHTNESS);
  Serial.println("Ready!!");
}

void loop() {
	unsigned int beta;
	currentMillis = millis();
  if (door_mode != 0){
    if ((currentMillis % 2) == 1){
      lc.setColumn(0, random(8), random(255));
    }
    else {
      lc.setRow(0, random(8), random(255));
    }
    data.setPixelColor(random(0,DATA_COUNT),data.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); 
    data.show(); 
  }
  else {
    lc.clearDisplay(0); 
    data.clear();
    data.show();
  }
  switch (neck_mode) {
    case 0:
      neck.clear();
      neck.show();
      break;
    case 1:   // All White
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(255,255,255,255));
        neck.show();
      }
      break;
    case 2:   // All Red
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(255,0,0,0));
        neck.show();
      }
      break;
    case 3:   // All Green
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(0,255,0,0));
        neck.show();
      }
      break;
    case 4:   // All Blue
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(0,0,255,0));
        neck.show();
      }
      break;
    case 5:   // All yellow
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(255,255,0,0));
        neck.show();
      }
      break;
    case 6:   // All Purple
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(255,0,255,0));
        neck.show();
      }
      break;
    case 7:   // All Cyan
      for (neck_led= 0; neck_led< NECK_COUNT; neck_led++){
        neck.setPixelColor(neck_led, neck.Color(0,255,255,0));
        neck.show();
      }
      break;
    case 8:   //Random Disco
      neck.setPixelColor(random(0,NECK_COUNT),neck.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); 
      neck.show(); 
      //delay(random(5,15));
      break;  
    case 9:   //Forward spin
        neck.clear(); 
        neck.setPixelColor(((neck_led+ 0)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 1)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 2)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 3)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 4)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 5)% NECK_COUNT),neck.Color(255,  0,255));
        neck.setPixelColor(((neck_led+ 20)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 21)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 22)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 23)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 24)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 25)% NECK_COUNT),neck.Color(255,  0,255));
        neck.setPixelColor(((neck_led+ 40)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 41)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 42)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 43)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 44)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 45)% NECK_COUNT),neck.Color(255,  0,255));
        neck.show();
        //delay(100);
        neck_led++;
      
      break;
    case 10:   //SpinBack
        neck.clear(); 
        neck.setPixelColor(((neck_led+ 0)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 1)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 2)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 3)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 4)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 5)% NECK_COUNT),neck.Color(255,  0,255));
        neck.setPixelColor(((neck_led+ 20)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 21)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 22)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 23)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 24)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 25)% NECK_COUNT),neck.Color(255,  0,255));
        neck.setPixelColor(((neck_led+ 40)% NECK_COUNT),neck.Color(  0,  0,255));
        neck.setPixelColor(((neck_led+ 41)% NECK_COUNT),neck.Color(  0,255,255));
        neck.setPixelColor(((neck_led+ 42)% NECK_COUNT),neck.Color(  0,255,  0));
        neck.setPixelColor(((neck_led+ 43)% NECK_COUNT),neck.Color(255,255,  0));
        neck.setPixelColor(((neck_led+ 44)% NECK_COUNT),neck.Color(255,  0,  0));
        neck.setPixelColor(((neck_led+ 45)% NECK_COUNT),neck.Color(255,  0,255));
        neck.show();
       //delay(100);
       neck_led--;
      
      break;
    	case 11: // Police Lights
		    neck.setPixelColor((NECK_COUNT - neck_led), doors.Color(255,  0,  0));
	    	neck.setPixelColor((neck_led), doors.Color(0,  0,  255));
	    	neck.show();
	    	neck_led++;
		  break;
		case 12:
			for(int i = start; i <= end; i++){
				neck.setPixelColor(i, doors.Color(red, green, blue));
			}
			neck.show();
			break;
  }
  
  if(neck_led< 0){
    neck_led= NECK_COUNT;
  }
  else if(neck_led> NECK_COUNT){
    neck_led= 0;
  }
  switch(door_mode){
	  case 0:
      doors.clear();
      doors.show();
      break;
    case 1:   // All White
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(255,255,255,255));
        doors.show();
      }
      break;
    case 2:   // All Red
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(255,0,0,0));
        doors.show();
      }
      break;
    case 3:   // All Green
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(0,255,0,0));
        doors.show();
      }
      break;
    case 4:   // All Blue
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(0,0,255,0));
        doors.show();
      }
      break;
    case 5:   // All yellow
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(255,255,0,0));
        doors.show();
      }
      break;
    case 6:   // All Purple
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(255,0,255,0));
        doors.show();
      }
      break;
    case 7:   // All Cyan
      for (door_led= 0; door_led< DOOR_COUNT; door_led++){
        doors.setPixelColor(neck_led, doors.Color(0,255,255,0));
        doors.show();
      }
      break;
    case 8:   //Random Disco
      doors.setPixelColor(random(0,DOOR_COUNT),doors.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); // Red
      doors.show(); 
      //delay(random(5,15));
      break;  
    case 9:   //Forward spin
        doors.clear(); 
        doors.setPixelColor(((door_led+ 0)% DOOR_COUNT),doors.Color(  0,  0,255));
        doors.setPixelColor(((door_led+ 1)% DOOR_COUNT),doors.Color(  0,255,255));
        doors.setPixelColor(((door_led+ 2)% DOOR_COUNT),doors.Color(  0,255,  0));
        doors.setPixelColor(((door_led+ 3)% DOOR_COUNT),doors.Color(255,255,  0));
        doors.setPixelColor(((door_led+ 4)% DOOR_COUNT),doors.Color(255,  0,  0));
        doors.setPixelColor(((door_led+ 5)% DOOR_COUNT),doors.Color(255,  0,255));
        doors.show();
        //delay(100);
        door_led++;
      
      break;
    case 10:   //SpinBack
        doors.clear(); 
        doors.setPixelColor(((door_led+ 0)% DOOR_COUNT),doors.Color(  0,  0,255));
        doors.setPixelColor(((door_led+ 1)% DOOR_COUNT),doors.Color(  0,255,255));
        doors.setPixelColor(((door_led+ 2)% DOOR_COUNT),doors.Color(  0,255,  0));
        doors.setPixelColor(((door_led+ 3)% DOOR_COUNT),doors.Color(255,255,  0));
        doors.setPixelColor(((door_led+ 4)% DOOR_COUNT),doors.Color(255,  0,  0));
        doors.setPixelColor(((door_led+ 5)% DOOR_COUNT),doors.Color(255,  0,255));
       doors.show();
       //delay(100);
       door_led++;
      
      break;
	  case 11: // Police Lights
		doors.setPixelColor((DOOR_COUNT - door_led), doors.Color(255,  0,  0));
		doors.setPixelColor((door_led), doors.Color(0,  255,  0));
		doors.show();
		door_led++;
		break;
	case 12:
		for(int i = start; i <= end; i++){
			doors.setPixelColor(i, doors.Color(red, green, blue));
		}
		doors.show();
		break;
  }
  
  if(door_led< 0){
    door_led= DOOR_COUNT;
  }
  else if(door_led> DOOR_COUNT){
    door_led= 0;
  }
  beta = (millis()- currentMillis);
	if (beta <= speed_mult){
		//Serial.println("\tLoop time:" + String(beta) + "ms, delaying for " + String(loop_delay*5) + "ms loops");
		delay(speed_mult - beta);
	}
	else {
	Serial.println("Error: Long loop of " + String(beta) + "ms is longer then " + String(speed_mult) + "ms target");
	}
}

 void receiveEvent(int howMany){
  int test;
  while(1 < Wire.available()) // loop through all but the last
  {
    test = Wire.read(); // receive byte as a integer
	if (test = 12){
		byte bank = 0;
		switch (Wire.read()){
			case 1:
			neck_mode = 12;
			break;
			case 2:
			door_mode = 12;
			break;
		}
		start = Wire.read();
		end = Wire.read();
		red = Wire.read();
		green = Wire.read();
		blue = Wire.read();
	}
    if (test != -1){
      door_mode = test;
      Serial.println("Door mode :" + String(door_mode));
    }
  }
  test = Wire.read(); // receive byte as a integer
    if (test != -1){
     neck_mode = test;
      Serial.println("Neck mode :" + String(neck_mode));
    }    // receive byte as an integer
  // print the integer
}