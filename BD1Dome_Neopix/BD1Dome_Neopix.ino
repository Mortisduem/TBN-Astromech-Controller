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
#include <Wire.h>
#include <FastLED.h>
#include <EEPROM.h>

//Correct Path as needed to point to the correct files
#include "d:\GitHub\TBN-Astromech-Controller\config\Device_Adresses.h"


// Configuration
#define LED_PIN        2          // Pin connected to the WS2812B LED strip
#define NUM_LEDS       251         // Number of LEDs in the strip
#define MAX_ACTIVE_LEDS 75         // Maximum number of LEDs active at the same time
#define MIN_ACTIVE_LEDS 10         // Minimum number of LEDs active at the same time
#define DEFAULT_BRIGHTNESS 128     // Default brightness if not using random brightness
#define LED_TYPE       NEO_KHZ800     // Type of LED strip
#define COLOR_ORDER    GRB         
#define EEPROM_SIZE    200         // Allocate enough space in EEPROM

// Modes: "blue", "red", "yellow", "green", "purple", "pink", "multicolor", "random", "cyan", "orange", "lime", "magenta", "white"
String colorMode = "blue";

// Adjustable settings (under configuration)
int blinkSpeed = 250;             // Speed of blinking (milliseconds delay between updates)
bool enableFade = false;          // Enable/disable fading effect
bool isOn = true;                 // Boolean to track if the LEDs are on or off

// Brightness control settings
bool sameBrightness = false;      // Set to true for all LEDs to have the same brightness
int minBrightness = 1;            // Minimum brightness (0-255) if random brightness is used
int maxBrightness = 200;          // Maximum brightness (0-255) if random brightness is used

CRGB leds[NUM_LEDS];
CRGB randomColor;  // Store a single random color for "random" mode



// Declare our NeoPixel strip object:
//Adafruit_NeoPixel A_Bank(A_BANK_COUNT, A_BANK_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

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
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);  // Set initial brightness (for sameBrightness mode)
  // initialize serial:
  Serial.begin(115200);
  Serial.print("Dark Saber: BD1 NeoPixel & Matrix Control v");
  if (version < 0){
    Serial.println("β."+String(0-version));
  }
  else {
    Serial.println(version);
  }
  Wire.begin(bd1_lights);         // join i2c bus with adress set in address.h
  Wire.onReceive(receiveEvent);    // Update Modes
  Serial.println("\tI2C adresss: " + String(body_lights));
  /*
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
  C_Bank.setBrightness(BRIGHTNESS); */
  
  // Print the welcome message
  Serial.println("Printed Droid BD1 Panel ready:");

  // Load settings from EEPROM
  loadConfigFromEEPROM();

  // Indicate that it's ready for commands
  Serial.println("Ready for commands.");
}

void loop() {
  // Check if a serial command is available
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    handleSerialCommand(command);
  }

  // If LEDs are turned off, skip the normal operation
  if (!isOn) {
    delay(100); // Simple delay when LEDs are off
    return;
  }

  // Turn off all LEDs
  FastLED.clear();

  // Activate a random number of LEDs, between MIN_ACTIVE_LEDS and MAX_ACTIVE_LEDS
  int activeLEDs = random(MIN_ACTIVE_LEDS, MAX_ACTIVE_LEDS + 1);
  for (int i = 0; i < activeLEDs; i++) {
    int ledIndex = random(NUM_LEDS);     // Pick a random LED
    leds[ledIndex] = getColor(colorMode);  // Set color based on selected mode
    
    // Set the brightness of the current LED
    leds[ledIndex].fadeLightBy(255 - getBrightness());  // Adjust brightness if random is enabled
  }

  // Show the LED pattern
  FastLED.show();

  // Apply fade effect if enabled
  if (enableFade) {
    // Wait before applying the fade effect to let the colors show
    delay(blinkSpeed);
    
    // Apply a gradual fade over time
    for (int fadeAmount = 0; fadeAmount < 256; fadeAmount += 5) {
      fadeToBlackBy(leds, NUM_LEDS, 5);  // Fading effect applied slowly
      FastLED.show();
      delay(30);  // Delay for a smoother fading effect
    }
  } else {
    delay(blinkSpeed);  // Just blink without fading
  }
}

// Helper function to get color based on mode
CRGB getColor(String mode) {
  if (mode == "blue") return CRGB(0, 0, 255);           // Blue
  else if (mode == "red") return CRGB(255, 0, 0);       // Red
  else if (mode == "yellow") return CRGB(255, 150, 0);  // Yellow
  else if (mode == "green") return CRGB(0, 255, 0);     // Green
  else if (mode == "purple") return CRGB(150, 0, 150);  // Purple
  else if (mode == "pink") return CRGB(255, 20, 147);   // Pink
  else if (mode == "cyan") return CRGB(0, 255, 255);    // Cyan
  else if (mode == "orange") return CRGB(255, 80, 0);   // Orange
  else if (mode == "lime") return CRGB(50, 205, 50);    // Lime
  else if (mode == "magenta") return CRGB(255, 0, 255); // Magenta
  else if (mode == "white") return CRGB(255, 255, 255); // White
  else if (mode == "multicolor") return CHSV(random8(), 255, 255); // Multicolor: Different color for each LED
  else if (mode == "random") return randomColor;  // Random: Use one random color for all LEDs
  else return CHSV(random8(), 255, 255);  // Fallback for invalid color mode
}

// Function to select a single random color for the "random" mode
void selectRandomColor() {
  randomColor = CRGB(random(255), random(255), random(255));  // Generate a single random RGB color
}

// Helper function to get brightness
int getBrightness() {
  if (sameBrightness) {
    return DEFAULT_BRIGHTNESS;  // Use the default brightness for all LEDs
  } else {
    // Random brightness between minBrightness and maxBrightness
    return random(minBrightness, maxBrightness + 1);
  }
}

// Save configuration to EEPROM
void saveConfigToEEPROM() {
  EEPROM.write(0, 1);  // Mark settings as valid

  // Save colorMode
  for (int i = 0; i < colorMode.length(); i++) {
    EEPROM.write(1 + i, colorMode[i]);
  }
  EEPROM.write(1 + colorMode.length(), '\0');  // Null-terminate string

  // Save other settings
  EEPROM.put(51, blinkSpeed);
  EEPROM.put(55, enableFade);
  EEPROM.put(56, sameBrightness);
  EEPROM.put(57, minBrightness);
  EEPROM.put(61, maxBrightness);
  EEPROM.put(65, isOn);

  Serial.println("Settings saved to EEPROM");
}

// Load configuration from EEPROM
void loadConfigFromEEPROM() {
  if (EEPROM.read(0) == 1) {  // Check if settings are valid
    // Load colorMode
    char colorModeBuf[50];
    for (int i = 0; i < 50; i++) {
      colorModeBuf[i] = EEPROM.read(1 + i);
    }
    colorMode = String(colorModeBuf);

    // Load other settings
    EEPROM.get(51, blinkSpeed);
    EEPROM.get(55, enableFade);
    EEPROM.get(56, sameBrightness);
    EEPROM.get(57, minBrightness);
    EEPROM.get(61, maxBrightness);
    EEPROM.get(65, isOn);

    Serial.println("Settings loaded from EEPROM");
  } else {
    Serial.println("No valid settings found in EEPROM");
  }
}

// Clear the entire EEPROM
void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF);  // Reset to default (uninitialized state)
  }
  Serial.println("EEPROM cleared");
}

// Function to handle incoming serial commands
void handleSerialCommand(String command) {
  command.trim();
  
  if (command.startsWith("color ")) {
    colorMode = command.substring(6);
    if (colorMode == "random") {
      selectRandomColor();  // Select a new random color when "random" mode is chosen
    }
    Serial.println("Color mode set to " + colorMode);
  } 
  else if (command.startsWith("blinkSpeed ")) {
    blinkSpeed = command.substring(11).toInt();
    Serial.println("Blink speed set to " + String(blinkSpeed));
  }
  else if (command.startsWith("fade ")) {
    String fadeState = command.substring(5);
    if (fadeState == "on") {
      enableFade = true;
      Serial.println("Fade enabled");
    } else if (fadeState == "off") {
      enableFade = false;
      Serial.println("Fade disabled");
    }
  }
  else if (command.startsWith("sameBrightness ")) {
    String brightnessState = command.substring(15);
    if (brightnessState == "on") {
      sameBrightness = true;
      Serial.println("Same brightness for all LEDs enabled");
    } else if (brightnessState == "off") {
      sameBrightness = false;
      Serial.println("Random brightness for each LED enabled");
    }
  }
  else if (command.startsWith("minBrightness ")) {
    minBrightness = command.substring(14).toInt();
    Serial.println("Minimum brightness set to " + String(minBrightness));
  }
  else if (command.startsWith("maxBrightness ")) {
    maxBrightness = command.substring(14).toInt();
    Serial.println("Maximum brightness set to " + String(maxBrightness));
  }
  else if (command == "on") {
    isOn = true;
    Serial.println("LEDs turned on");
  }
  else if (command == "off") {
    isOn = false;
    FastLED.clear();
    FastLED.show();
    Serial.println("LEDs turned off");
  }
  else if (command == "save") {
    saveConfigToEEPROM();
  }
  else if (command == "clear") {
    clearEEPROM();
  }
  else if (command == "status") {
    Serial.println("Current settings:");
    Serial.println("Color mode: " + colorMode);
    Serial.println("Blink speed: " + String(blinkSpeed));
    Serial.println("Fade: " + String(enableFade ? "on" : "off"));
    Serial.println("Same brightness: " + String(sameBrightness ? "on" : "off"));
    Serial.println("Min brightness: " + String(minBrightness));
    Serial.println("Max brightness: " + String(maxBrightness));
    Serial.println("LEDs: " + String(isOn ? "on" : "off"));
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