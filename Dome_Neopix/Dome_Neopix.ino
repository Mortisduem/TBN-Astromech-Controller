/*
Dome Lighting Control board for TBN Controler system
I2C input to select the lighting mode, board controls some basic lighting animations to offload from the main hub.
Preprogrammed modes for each bank are
 0 - Lights Off (Clears 
 1 - All White
 2 - All Red
 3 - All Green
 4 - All Blue
 5 - All Yellow
 6 - All Purple
 7 - All Cyan
 8 - Disco, Random color to Random Pixels
 9 - Theater Left
10 - Theater Right
11 - Police Bar
12 - Mannualy writen from Hub controler, sets ranges of leds to a requested color
*/
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
#define A_BANK_PIN	3
#define B_BANK_PIN	4
#define C_BANK_PIN	5

// How many NeoPixels are attached to the Arduino?
#define A_BANK_COUNT	60
#define B_BANK_COUNT	18
#define C_BANK_COUNT	5
//#define ROWS 4
//#define COLS 8

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255 // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel A_Bank(A_BANK_COUNT, A_BANK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel B_Bank(B_BANK_COUNT, B_BANK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel C_Bank(C_BANK_COUNT, C_BANK_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
LedControl Matrix = LedControl(DIN,CLK,CS,0);

int A_Bank_mode =  0;
int B_Bank_mode = 0;
int C_Bank_mode = 0;
int A_Bank_led = 0;
int B_Bank_led = 0;
int C_Bank_led = 0;
const int speed_mult = loop_delay * loop_mult; //Limit the refresh to a multi of the Hub loop and smooth out scrolling
unsigned long currentMillis;

String inputString = "";      // a String to hold incoming C_Bank
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
  Matrix.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
  Matrix.setIntensity(0,15);       // Set the brightness to maximum value
  Matrix.clearDisplay(0);          // and clear the display
  //mode = random(0,10);


  A_Bank.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  A_Bank.show();            // Turn OFF all pixels ASAP
  A_Bank.setBrightness(BRIGHTNESS);
  B_Bank.begin(); 
  B_Bank.show();
  B_Bank.setBrightness(BRIGHTNESS);
  C_Bank.begin(); 
  C_Bank.show();
  C_Bank.setBrightness(BRIGHTNESS);
  Serial.println("Ready!!");
}

void loop() {
	unsigned int beta;
	currentMillis = millis();
  if (B_Bank_mode != 0){
    if ((currentMillis % 2) == 1){
      Matrix.setColumn(0, random(8), random(255));
    }
    else {
      Matrix.setRow(0, random(8), random(255));
    }
    C_Bank.setPixelColor(random(0,C_BANK_COUNT),C_Bank.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); 
    C_Bank.show(); 
  }
  else {
    Matrix.clearDisplay(0); 
    C_Bank.clear();
    C_Bank.show();
  }
  
  switch (A_Bank_mode) {
    case 0:
      A_Bank.clear();
      A_Bank.show();
      break;
    case 1:   // All White
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(255,255,255,255));
        A_Bank.show();
      }
      break;
    case 2:   // All Red
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(255,0,0,0));
        A_Bank.show();
      }
      break;
    case 3:   // All Green
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(0,255,0,0));
        A_Bank.show();
      }
      break;
    case 4:   // All Blue
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(0,0,255,0));
        A_Bank.show();
      }
      break;
    case 5:   // All yellow
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(255,255,0,0));
        A_Bank.show();
      }
      break;
    case 6:   // All Purple
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(255,0,255,0));
        A_Bank.show();
      }
      break;
    case 7:   // All Cyan
      for (A_Bank_led= 0; A_Bank_led< A_BANK_COUNT; A_Bank_led++){
        A_Bank.setPixelColor(A_Bank_led, A_Bank.Color(0,255,255,0));
        A_Bank.show();
      }
      break;
    case 8:   //Random Disco
      A_Bank.setPixelColor(random(0,A_BANK_COUNT),A_Bank.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); 
      A_Bank.show(); 
      //delay(random(5,15));
      break;  
    case 9:   //Forward spin
        A_Bank.clear(); 
        A_Bank.setPixelColor(((A_Bank_led+ 0)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 1)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 2)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 3)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 4)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 5)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 20)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 21)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 22)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 23)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 24)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 25)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 40)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 41)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 42)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 43)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 44)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 45)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.show();
        //delay(100);
        A_Bank_led++;
      
      break;
    case 10:   //SpinBack
        A_Bank.clear(); 
        A_Bank.setPixelColor(((A_Bank_led+ 0)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 1)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 2)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 3)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 4)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 5)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 20)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 21)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 22)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 23)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 24)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 25)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 40)% A_BANK_COUNT),A_Bank.Color(  0,  0,255));
        A_Bank.setPixelColor(((A_Bank_led+ 41)% A_BANK_COUNT),A_Bank.Color(  0,255,255));
        A_Bank.setPixelColor(((A_Bank_led+ 42)% A_BANK_COUNT),A_Bank.Color(  0,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 43)% A_BANK_COUNT),A_Bank.Color(255,255,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 44)% A_BANK_COUNT),A_Bank.Color(255,  0,  0));
        A_Bank.setPixelColor(((A_Bank_led+ 45)% A_BANK_COUNT),A_Bank.Color(255,  0,255));
        A_Bank.show();
       //delay(100);
       A_Bank_led--;
      
      break;
    	case 11: // Police Lights
		    A_Bank.setPixelColor((A_BANK_COUNT - A_Bank_led), B_Bank.Color(255,  0,  0));
	    	A_Bank.setPixelColor((A_Bank_led), B_Bank.Color(0,  250,  0));
	    	A_Bank.show();
	    	A_Bank_led++;
		  break;
  }
  if(A_Bank_led< 0){
    A_Bank_led= A_BANK_COUNT;
  }
  else if(A_Bank_led> A_BANK_COUNT){
    A_Bank_led= 0;
  }
  
  switch(B_Bank_mode){
	  case 0:
      B_Bank.clear();
      B_Bank.show();
      break;
    case 1:   // All White
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(255,255,255,255));
        B_Bank.show();
      }
      break;
    case 2:   // All Red
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(255,0,0,0));
        B_Bank.show();
      }
      break;
    case 3:   // All Green
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(0,255,0,0));
        B_Bank.show();
      }
      break;
    case 4:   // All Blue
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(0,0,255,0));
        B_Bank.show();
      }
      break;
    case 5:   // All yellow
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(255,255,0,0));
        B_Bank.show();
      }
      break;
    case 6:   // All Purple
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(255,0,255,0));
        B_Bank.show();
      }
      break;
    case 7:   // All Cyan
      for (B_Bank_led= 0; B_Bank_led< B_BANK_COUNT; B_Bank_led++){
        B_Bank.setPixelColor(B_Bank_led, B_Bank.Color(0,255,255,0));
        B_Bank.show();
      }
      break;
    case 8:   //Random Disco
      B_Bank.setPixelColor(random(0,B_BANK_COUNT),B_Bank.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); // Red
      B_Bank.show(); 
      //delay(random(5,15));
      break;  
    case 9:   //Forward spin
        B_Bank.clear(); 
        B_Bank.setPixelColor(((B_Bank_led+ 0)% B_BANK_COUNT),B_Bank.Color(  0,  0,255));
        B_Bank.setPixelColor(((B_Bank_led+ 1)% B_BANK_COUNT),B_Bank.Color(  0,255,255));
        B_Bank.setPixelColor(((B_Bank_led+ 2)% B_BANK_COUNT),B_Bank.Color(  0,255,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 3)% B_BANK_COUNT),B_Bank.Color(255,255,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 4)% B_BANK_COUNT),B_Bank.Color(255,  0,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 5)% B_BANK_COUNT),B_Bank.Color(255,  0,255));
        B_Bank.show();
        //delay(100);
        B_Bank_led++;
      
      break;
    case 10:   //SpinBack
        B_Bank.clear(); 
        B_Bank.setPixelColor(((B_Bank_led+ 0)% B_BANK_COUNT),B_Bank.Color(  0,  0,255));
        B_Bank.setPixelColor(((B_Bank_led+ 1)% B_BANK_COUNT),B_Bank.Color(  0,255,255));
        B_Bank.setPixelColor(((B_Bank_led+ 2)% B_BANK_COUNT),B_Bank.Color(  0,255,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 3)% B_BANK_COUNT),B_Bank.Color(255,255,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 4)% B_BANK_COUNT),B_Bank.Color(255,  0,  0));
        B_Bank.setPixelColor(((B_Bank_led+ 5)% B_BANK_COUNT),B_Bank.Color(255,  0,255));
       B_Bank.show();
       //delay(100);
       B_Bank_led++;
      
      break;
	  case 11: // Police Lights
		B_Bank.setPixelColor((B_BANK_COUNT - B_Bank_led), B_Bank.Color(255,  0,  0));
		B_Bank.setPixelColor((B_Bank_led), B_Bank.Color(0,  250,  0));
		B_Bank.show();
		B_Bank_led++;
		break;
  }
  if(B_Bank_led< 0){
    B_Bank_led= B_BANK_COUNT;
  }
  else if(B_Bank_led> B_BANK_COUNT){
    B_Bank_led= 0;
  }
  
  switch(C_Bank_mode){
	  case 0:
      C_Bank.clear();
      C_Bank.show();
      break;
    case 1:   // All White
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(255,255,255,255));
        C_Bank.show();
      }
      break;
    case 2:   // All Red
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(255,0,0,0));
        C_Bank.show();
      }
      break;
    case 3:   // All Green
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(0,255,0,0));
        C_Bank.show();
      }
      break;
    case 4:   // All Blue
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(0,0,255,0));
        C_Bank.show();
      }
      break;
    case 5:   // All yellow
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(255,255,0,0));
        C_Bank.show();
      }
      break;
    case 6:   // All Purple
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(255,0,255,0));
        C_Bank.show();
      }
      break;
    case 7:   // All Cyan
      for (C_Bank_led= 0; C_Bank_led< C_BANK_COUNT; C_Bank_led++){
        C_Bank.setPixelColor(C_Bank_led, C_Bank.Color(0,255,255,0));
        C_Bank.show();
      }
      break;
    case 8:   //Random Disco
      C_Bank.setPixelColor(random(0,C_BANK_COUNT),C_Bank.Color(min(random(0,4)*64,255),  min(random(0,4)*64,255),   min(random(0,4)*67,255))); // Red
      C_Bank.show(); 
      //delay(random(5,15));
      break;  
    case 9:   //Forward spin
        C_Bank.clear(); 
        C_Bank.setPixelColor(((C_Bank_led+ 0)% C_BANK_COUNT),C_Bank.Color(  0,  0,255));
        C_Bank.setPixelColor(((C_Bank_led+ 1)% C_BANK_COUNT),C_Bank.Color(  0,255,255));
        C_Bank.setPixelColor(((C_Bank_led+ 2)% C_BANK_COUNT),C_Bank.Color(  0,255,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 3)% C_BANK_COUNT),C_Bank.Color(255,255,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 4)% C_BANK_COUNT),C_Bank.Color(255,  0,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 5)% C_BANK_COUNT),C_Bank.Color(255,  0,255));
        C_Bank.show();
        //delay(100);
        C_Bank_led++;
      
      break;
    case 10:   //SpinBack
        C_Bank.clear(); 
        C_Bank.setPixelColor(((C_Bank_led+ 0)% C_BANK_COUNT),C_Bank.Color(  0,  0,255));
        C_Bank.setPixelColor(((C_Bank_led+ 1)% C_BANK_COUNT),C_Bank.Color(  0,255,255));
        C_Bank.setPixelColor(((C_Bank_led+ 2)% C_BANK_COUNT),C_Bank.Color(  0,255,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 3)% C_BANK_COUNT),C_Bank.Color(255,255,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 4)% C_BANK_COUNT),C_Bank.Color(255,  0,  0));
        C_Bank.setPixelColor(((C_Bank_led+ 5)% C_BANK_COUNT),C_Bank.Color(255,  0,255));
       C_Bank.show();
       //delay(100);
       C_Bank_led++;
      
      break;
	  case 11: // Police Lights
		C_Bank.setPixelColor((C_BANK_COUNT - C_Bank_led), C_Bank.Color(255,  0,  0));
		C_Bank.setPixelColor((C_Bank_led), C_Bank.Color(0,  250,  0));
		C_Bank.show();
		C_Bank_led++;
		break;
  }
  if(C_Bank_led< 0){
    C_Bank_led= C_BANK_COUNT;
  }
  else if(C_Bank_led> C_BANK_COUNT){
    C_Bank_led= 0;
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
    if (test != -1){
      B_Bank_mode = test;
      Serial.println("Door mode :" + String(B_Bank_mode));
    }
  }
  test = Wire.read(); // receive byte as a integer
    if (test != -1){
     A_Bank_mode = test;
      Serial.println("Neck mode :" + String(A_Bank_mode));
    }    // receive byte as an integer
  // print the integer
}