/*
Main Hub of the Droid Control on an Arduino Mega
- Read SBUS in on serial3 and save to data array (serial inverter needed for good reads)
- AnimationConfig.h Stores Global constants and varibles for:
  - Sbus switch sets
  - Sbus range limits
  - Servo limits
  - Audio Delay and Volume
  - Cytron Motor Driver config
  - Main Hub Varibles and State Varibles
- Device_Adresses.h stores the adresses of slave devices and shared Board Global varibles
  - Loop delay is to keep boards tick rate to similar rate
  - 
- Dome Cytron is PWM and Direction Pin
- Animation Loop Handled localy
*/
#include <Arduino.h>
#include <Smoothed.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_PWMServoDriver.h>
//#include <Adafruit_ADXL345_U.h>
#include "headers\sbus.h"
#include <IBusBM.h>
#include "headers\Cytron_SmartDriveDuo.h"	// Altered Header file to use Serial 1 not Serial 0 on mega board for Smartdrive control

//#include "DYPlayerArduino.h"
#include "d:\GitHub\TBN-Astromech-Controller\config\Body_Config.h"				//Config of Body
#include "d:\GitHub\TBN-Astromech-Controller\config\Device_Adresses.h"
//Config Sbus
/* SBUS object, reading SBUS */
  bfs::SbusRx sbus_rx(&Serial3);
/* SBUS object, writing SBUS */
//bfs::SbusTx sbus_tx(&Serial3);
/* SBUS data */
  bfs::SbusData data;

  IBusBM IBus; // IBus object for receivig signals from transmitter/receiver

  Cytron_SmartDriveDuo smartDrive(SERIAL_SIMPLIFIED, LEG_CD, BAUDRATE);
  Cytron_SmartDriveDuo smartSholder(SERIAL_SIMPLIFIED, SHOULDER_CD, BAUDRATE);

//Door and arm smoothing
  Smoothed <int> Gripper_Arm_Pos;
  Smoothed <int> Gripper_Arm_Door;
  Smoothed <int> Interface_Arm_Pos;
  Smoothed <int> Interface_Arm_Door;
  Smoothed <int> Utilty_Upper;
  Smoothed <int> Utilty_Lower;

// Initialise the player, it defaults to using Serial.
  Adafruit_PWMServoDriver body = Adafruit_PWMServoDriver(body_servos); //Adress 64 Body board 16 channels
  Adafruit_PWMServoDriver dome = Adafruit_PWMServoDriver(dome_servos); //Adress 65 Dome board 16 channels
//Common Varibles
  unsigned int audio_delay = loop_delay * audio_mult;
  unsigned long ani_clock;
  int r_max, r_min, r_swhigh, r_swlow; //remote limits
  unsigned long last_a_tick;
void setup() {
  //Start I2C bus with Mega as master and 
  Serial.begin(115200);
  Serial.print("Dark Saber Hub version:");
  if (version < 0){
    Serial.println("β."+String(0-version));
  }
  else {
    Serial.println(version);
  }
 
  Serial.println("Config Settings for droid:");
  Serial.println("   I2C Adresses set for");
  Serial.println("\t-Body  Servos: " + String(body_servos));
  Serial.println("\t-Body  Lights: " + String(body_lights));
  Serial.println("\t-Body   Audio: " + String(body_audio));
  Serial.println("\t-Dome  Servos: " + String(dome_servos));
  Serial.println("\t-Dome  Lights: " + String(dome_lights));
  Serial.println("\t-Dome Control: TBA");
  Serial.println("   Funtions mapped to Channels:");
  Serial.println("\t  Drive L/R: "+ String(Drive_LR_CH));
  Serial.println("\tDrive_Front: "+ String(Drive_FB_CH));
  Serial.println("\t Drive_Dome: "+ String(Drive_Dome_CH));
  Serial.println("\t  Audio Vol: "+ String(Audio_Volume_CH));
  Serial.println("\t Grip A POS: "+ String(Gripper_APOS_CH));
  Serial.println("\tInter A POS: "+ String(Interface_APOS_CH));
  Serial.println("\t  Data Door: "+ String(Data_Door_CH));
  Serial.println("\tLights Mode: "+ String(Light_Mode_CH));
  Serial.println("\t  Light Sel: "+ String(Light_Select_CH));
  Serial.println("\t Audio Mode: "+ String(Audio_Mode_CH));
  Serial.println("\t  Audio Sel: "+ String(Audio_Bank_CH));
  Serial.println("\t   Anim Sel: "+ String(Animation_Select_CH));
  Serial.println("\t Shoul Mode: "+ String(Shoulder_Mode_CH));
  Serial.println("\t    Trigger: "+ String(Trigger_CH));
  Serial.println("Starting Boot:");  
  I2C_Bus_Setup();
  triggerI2C(dome_lights,9);
  triggerI2C(body_lights,9);
  switch (Remote_Mode){
	  case 1:
		Serial.println("  SBUS Mode Enabled: Tx/Rx on Serial3");
		Sbus_Setup();
		Serial.println("  SBUS Remote Connected");
		break;
	  case 2:
		Serial.println("  IBUS Mode Enabled: Tx/Rx on Serial3");
		Ibus_Setup();
		Serial.println("  IBUS Remote Connected");
		break;
	  case 10:
		Serial.println("  RC PWM Mode Enabled:");
		RC_Setup();
		Serial.println("  RC PWM Active");
		break;
	  default:
		Serial.println("  Using Serial over USB for droid control, Not fully enabled");
		break;
  }
  Serial.println("  -Setting hub PWM out to High Freq");
  triggerI2C(dome_lights,1);
  triggerI2C(body_lights,1);
  //Update PWMs to High Freq
  int erase = 7;
  TCCR1B &= ~erase; // cancel pre-scaler of 64
  TCCR2B &= ~erase;
  TCCR3B &= ~erase;
  TCCR4B &= ~erase;
  TCCR1B |= 1;   // 1 pre-scaler to remove 490hz whine in dome speed control
  TCCR2B |= 1;
  TCCR3B |= 1;
  TCCR4B |= 1;                    
  
  triggerI2C(dome_lights,2);
  triggerI2C(body_lights,2);
  Serial.println("  - Starting Dome and Body Servo communication");
  //start pwn for servos
  dome.begin();
  dome.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  body.begin();
  body.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  //Setup Dome and Leg drive connections
  switch(Dome_Drive){
	  case 1:
		Serial.println("  - Starting PWM+DIR Drive for Dome");
		pinMode(12, OUTPUT); //Pin 12 for dome drive PWM
		digitalWrite(12,LOW);
		pinMode(13, OUTPUT); //Dome Dir
		digitalWrite(13,LOW);
		break;
	  case 2:
		Serial.println("  - Starting PWM Drive for Dome");
		pinMode(12, OUTPUT); //Pin 12 for dome drive PWM
		digitalWrite(12,LOW);
		break;
	  case 3:
		//Syren WIP will disable Serial2 due to 
		break;
  }
  
  
  Serial.println("  - Shoulder Limit Switch modes");
  //Setup Shoulder Limit Switches and Home Shoulders
  for (int thisPin = 50; thisPin < 54; thisPin++) {
    pinMode(thisPin, INPUT_PULLUP);
  }
  
  //DS_RC.go(0,1);
  //DS_Ani.go(0,1);
  //triggerI2C(dome_lights,2);
  switch  (Remote_Mode){
	  case 10:
	  Serial.println("  -No Driver control Enabled\n\tNOTE: RC PWM mode in use, No Drive Serial enabled, Wire PWM signal to Drive motor directly");
	  break;
	  case 0:
	  Serial.println("  -Staring Coms for Cytron_SmartDriveDuos:");
	  delay(2000); // Delay for 2 seconds before starting seria1 for the leg drives.
	  Serial.println("    - Foot Driver");
	  smartDrive.initialByte(0x80); // MDDS30
	  default:
	  Serial.println("\tStaring Coms for Cytron_SmartDriveDuos");
	  delay(2000); // Delay for 2 seconds before starting seria1 for the leg drives.
	  Serial.println("    - Foot Driver");
	  smartDrive.initialByte(0x80); // MDDS30
	  break;
  }
  triggerI2C(dome_lights,3);
  triggerI2C(body_lights,3);
  if(Leg_232 and Dome_Drive != 3){
	  Serial.println("  -2-3-2 Mode Enabled");
	  Serial.println("    - Shoulder Driver:");
	  smartSholder.initialByte(0x55); // MDS10
	  Serial.println("        Homing Shoulder Pos");
	  homeShoulders();
  }
  else {
	  Serial.println("  -2-3-2 Mode Disabled");
  }
  Serial.println("  -Homing Body Servos");
  homebodyservos();
  Serial.println("  -Audio set at "+String(sound_effects)+" effects, "+String(sound_songs)+" songs");
  //Start smoothing of arms
	Gripper_Arm_Pos.begin(SMOOTHED_EXPONENTIAL, 6);
	Gripper_Arm_Door.begin(SMOOTHED_EXPONENTIAL, 3);
	Interface_Arm_Pos.begin(SMOOTHED_EXPONENTIAL, 6);
	Interface_Arm_Door.begin(SMOOTHED_EXPONENTIAL, 3);
	Utilty_Lower.begin(SMOOTHED_EXPONENTIAL, 6);
	Utilty_Upper.begin(SMOOTHED_EXPONENTIAL, 6);
  // Set track #25 then play to alert startup Complete
  playsound(25);  
  Serial.println("Boot Finished");
  triggerI2C(dome_lights,0);
  triggerI2C(body_lights,0);
  //Start Idle sound delay
  idle_Delay = idle_Timer + millis();
  check_audio = millis();
  
}

void I2C_Bus_Setup(){
	Serial.println("  Starting I2C bus");
	Wire.begin();
	for(int i = 10; i < 128; i++){
		Wire.requestFrom(i,1,true);
		while(Wire.available()){
			byte dump = Wire.read();
			switch(i){
				case body_servos :
					Serial.println("    - body servos found "+ String(i));
					body_servo_good = true;
					break;
				case dome_servos :
					Serial.println("    - dome servos found "+ String(i));
					//dome_servo_good = true;
					break;
				case body_lights :
					Serial.println("    - body lights found "+ String(i));
					//body_lights_good = true
					break;
				case dome_lights :
					Serial.println("    - dome lights found "+ String(i));
					//dome_lights_good = true;
					break;
				case body_audio :
					Serial.println("    - body audio found "+ String(i));
					//body_audio_good = true;
					break;
				default:
					Serial.println("    - unknown device at " + String(i));
					break;
			}
		}
		delay(10);
	}
}

void Sbus_Setup(){	// Setup Sbus Comunication
  /* Start SBUC communication */
  sbus_rx.Begin();
  Serial.println("    - Waiting on good Sbus Data...");
  //sbus_tx.Begin();
  bool test = true;
  while (sbus_rx.Read() != true){
	  //loops while waiting on Sbus reads for remote to connect.
  }
  r_min = sbus_min;
  r_max = sbus_max;
  r_swlow = sbus_swlow;
  r_swhigh = sbus_swhigh;
}

void Ibus_Setup(){	// Setup Ibus Comunication
	//PENDING
	IBus.begin(Serial2);
	Serial.println("    - Waiting for Good Ibus Data...");
	while (IBus.cnt_rec==0) delay(100);
	r_min = ibus_min;
	r_max = ibus_max;
	r_swlow = ibus_swlow;
	r_swhigh = ibus_swhigh;
}

void RC_Setup() {	// Setup Radio Control via PWM 
	//Not Fully Implemented
	Serial.print("    - RC PWM mode pin setup:\n");
	for(byte i = 2; i < 12; i++){
		pinMode(i, INPUT);
		Serial.println("\t  CH["+String(i-2)+"] : "+String(i));
	}
	r_min = rc_min;
	r_max = rc_max;
	r_swlow = rc_swlow;
	r_swhigh = rc_swhigh;
}

void loop() {			//Main Loop
	unsigned int beta;
	currentMillis = millis();
  switch (Remote_Mode){
	  case 1:
		Sbus_Poll();
		break;
	  case 2:
		Ibus_Poll();
		break;
	  case 10:
		RC_Poll();
		break;
  }
	if(bad_bus != true){
		body_motion(); // Drive the Droid
		audio();  // Audio Block
		animations();
		lights();
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

void Sbus_Poll () {		// Poll Sbus 
	bad_bus = false;
  if (sbus_rx.Read()) {
    /* Grab the received data */
    data = sbus_rx.data();
  }
  //Serial.print("Sbus Read :");
	for (int8_t i = 0; i < data.NUM_CH; i++) {
      //Serial.print(data.ch[i]);
      //Serial.print("\t");
	  if((data.ch[i] < r_min or data.ch[i] > r_max)and bad_bus != true){
		  bad_bus = true;
	  }
    }
	if (bad_bus){
		Serial.println("ERROR: Bad Sbus Read " + String(currentMillis));
		for (int8_t i = 0; i < data.NUM_CH; i++) {
			data.ch[i] = (r_min + r_max)/2;
		}
		
    }
		
	//Serial.println(" ");
	
}

void Ibus_Poll () {		// Poll Ibus
	//pending testing
	bad_bus = false;
	for (int8_t i = 0; i < 14; i++){
		data.ch[i] = IBus.readChannel(i);
      //Serial.print("\t");
	  if((data.ch[i] < r_min or data.ch[i] > r_max)and bad_bus != true){
		  bad_bus = true;
	  }
	}
	if (bad_bus){
		Serial.println("ERROR: Bad Ibus Read " + String(currentMillis));
		for (int8_t i = 0; i < data.NUM_CH; i++) {
			data.ch[i] = (r_min + r_max)/2;
		}
		
    }
}

void RC_Poll (){		// Poll RC Channels
	//Not Fully Implemented yet
	for(byte i = 0; i < 10; i++){
		data.ch[i] = readChannel(2+i,r_min,r_max,0);
	}
} 

void homeShoulders(){	//Home Shoulders
	bool home = false;
	byte stage = 0;
	currentMillis = millis();
	//Serial.println("\tHoming Shoulders:");
	while (!home and Shoulder_homed){
		RF = digitalRead(51);
		RR = digitalRead(53);
		LF = digitalRead(50);
		LR = digitalRead(52);
		switch (stage){
		case 0:
			if (LR and RR){
				smartSholder.control(100, -100);
			}
			else if (LR){
				smartSholder.control(100, 0);
			}
			else if(RR) {
				smartSholder.control(0, -100);
			}
			else {
				smartSholder.control(0, 0);
				stage += 1;
			}
			break;
		case 1:
			if (!LF and !RF){
				smartSholder.control(100, -100);
			}
			else if (!LF){
				smartSholder.control(100, 0);
			}
			else if(!RF) {
				smartSholder.control(0, -100);
			}
			else {
				smartSholder.control(0, 0);
				stage += 1;
			}
			break;
		case 2:
			if (LF and RF){
				smartSholder.control(-50, 50);
			}
			else if (LF){
				smartSholder.control(-50, 0);
			}
			else if(RF) {
				smartSholder.control(0, 50);
			}
			else {
				smartSholder.control(0, 0);
				stage += 1;
			}
			break;
		case 3:
			if (!LF and !RF){
				smartSholder.control(50, -50);
			}
			else if (!LF){
				smartSholder.control(50, 0);
			}
			else if(!RF) {
				smartSholder.control(0, -50);
			}
			else {
				smartSholder.control(0, 0);
				home = true;
				Shoulder_Pos = -1;
			}
			break;
		}
		if ((millis()- currentMillis) >= 30000) {
			Shoulder_homed = false;
			Shoulder_Pos = -1;
			smartSholder.control(0, 0);
			Serial.println("ERROR: Homing shoulders fail, Shoulder drive disabled");
		}
	}
}

void homebodyservos(){
	//Serial.println("\tHoming body Servos");
	for(byte i = 0; i < 10; i++){
		Gripper_Arm_Pos.add(GripperArmIn);
		Gripper_Arm_Door.add(LeftDoorClose);
		Interface_Arm_Pos.add(InterfaceArmIn);
		Interface_Arm_Door.add(RightDoorClose);
		Utilty_Lower.add(UtlityLowwerClose);
		Utilty_Upper.add(UtlityUpperClose);
	}
  body.setPWM(5,0,UtlityLowwerClose);
  body.setPWM(4,0,UtlityUpperClose);
	delay(250);
  body.setPWM(3,0,GripperClose);
  body.setPWM(10,0,InterfaceClose);
  delay(250);
  body.setPWM(2,0,GripperArmIn);
  body.setPWM(9,0,InterfaceArmIn);
  delay(250);
  body.setPWM(1,0,RightDoorClose);
  body.setPWM(8,0,LeftDoorClose);
}

void body_motion(){	
	switch (Remote_Mode){
		case 10:
			//RC PWM signals wired directly to Cytron Motor Drivers
			break;
		default:
			Drive_Right = map(data.ch[Drive_LR_CH], r_min, r_max, -100 , 100); 
			Drive_Forward = map(data.ch[Drive_FB_CH], r_min, r_max, -100 , 100);
			Drive_Dome = map(data.ch[Drive_Dome_CH], r_min, r_max, -150 , 150);   	//Dome Move
			Drive_Mix();
			dome_move();
			break;
	}
  
  if(Shoulder_homed and Leg_232){
	  moveSholders();
  }
  arms();
  smartDrive.control(speedLeft, speedRight);
}

void dome_move(){		//Move Dome
  //Dome movement
					 
  if((Drive_Dome) < 0){
      digitalWrite(13,HIGH);
  }
  else if((Drive_Dome) > 0){
      digitalWrite(13,LOW);
  }
  else{
      Drive_Dome = 0;
      digitalWrite(13,LOW);
  }
  test = abs(Drive_Dome) + Dome_Ani_Speed;
  RC_DomeSpeed = constrain(test,0, 150);
  analogWrite(12,RC_DomeSpeed);
}

void Drive_Mix(){		//Mix Drives for Foot Drives
  float   nMotPremixL;    // Motor (left)  premixed output
  float   nMotPremixR;    // Motor (right) premixed output
  int     nPivSpeed;      // Pivot Speed 
  float   fPivScale;      // Balance scale b/w drive and pivo
	
  // Calculate Drive Turn output due to Joystick X input
  if (Drive_Forward >= 0) {
    // Forward
    nMotPremixL = (Drive_Forward>=0)? 100.0 : (100.0 + Drive_Forward);
    nMotPremixR = (Drive_Forward>=0)? (100.0 - Drive_Forward) : 100.0;
  } else {
    // Reverse
    nMotPremixL = (Drive_Forward>=0)? (100.0 - Drive_Forward) : 100.0;
    nMotPremixR = (Drive_Forward>=0)? 100.0 : (100.0 + Drive_Forward);
  }

  // Scale Drive output due to Joystick Y input (throttle)
  nMotPremixL = nMotPremixL * Drive_Right/100.0;
  nMotPremixR = nMotPremixR * Drive_Right/100.0;

  // Now calculate pivot amount
  // - Strength of pivot (nPivSpeed) based on Joystick X input
  // - Blending of pivot vs drive (fPivScale) based on Joystick Y input
  nPivSpeed = Drive_Forward;
  fPivScale = (abs(Drive_Right)>fPivYLimit)? 0.0 : (1.0 - abs(Drive_Right)/fPivYLimit);

  // Calculate final mix of Drive and Pivot
  int Left = (1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed);
  int Right = (1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed);
  
  //leg position override
    if (Shoulder_Pos != 1 and Leg_232){
	  speedLeft = ((Leg_Slow * Left) / 100) ;
	  speedRight = ((Leg_Slow * Right) / 100);
	  //Serial.println("Low speed " + String(Leg_Slow) + ": " + String(speedLeft) + " ," + String(speedRight));
	}
	else {
		speedLeft = Left;
		speedRight = Right;
	}
}

void moveSholders() { 	//Switch between 2-3 leg modes
	RF = digitalRead(51);
	RR = digitalRead(53);
	LF = digitalRead(50);
	LR = digitalRead(52);
	
	if (data.ch[Shoulder_Mode_CH] < r_swlow and Shoulder_Pos != -1) {
			if (!LF and !RF){
				smartSholder.control(100, -100);
			}
			else if (!LF){
				smartSholder.control(100, 0);
			}
			else if(!RF) {
				smartSholder.control(0, -100);
			}
			else {
				smartSholder.control(0, 0);
				Shoulder_Pos = -1;
			}
	}
	else if (data.ch[Shoulder_Mode_CH]> r_swhigh and Shoulder_Pos != 1){
			if (!LR and !RR){
				smartSholder.control(-100, 100);
			}
			else if (!LR){
				smartSholder.control(-100, 0);
			}
			else if(!RR) {
				smartSholder.control(0, 100);
			}
			else {
				smartSholder.control(0, 0);
				Shoulder_Pos = 1;
			}
	}
	else /*if(Shoulder_Pos != 0)*/{
		smartSholder.control(0, 0);
		Shoulder_Pos = 0;
	}
}

void arms(){ 			//Body arms & door control
	//rewrite inprogress
	int GAD = Gripper_Arm_Door.get();
	int IAD = Interface_Arm_Door.get();
	int GAP = Gripper_Arm_Pos.get();
	int IAP = Interface_Arm_Pos.get();
	int UUP = Utilty_Upper.get();
	int ULP = Utilty_Lower.get();
	
	if(data.ch[Left_Arm_Mode_CH] <= r_swlow){
		if(GAP == GripperArmIn){
			Gripper_Arm_Door.add(LeftDoorClose);
		}
		else{
			Gripper_Arm_Pos.add(GripperArmIn);
		}
		if (UUP != UtlityUpperClose){
			Utilty_Upper.add(UtlityUpperClose);
		}
	}
	else if (data.ch[Left_Arm_Mode_CH] >= r_swhigh){
		//Left side arm & door
		if(GAD != LeftDoorOpen){
			Gripper_Arm_Door.add(LeftDoorOpen);
		}
		else {
			Gripper_Arm_Pos.add(map(data.ch[Gripper_APOS_CH],r_min,r_max,GripperArmIn,GripperArmOut));
		}
	}
	else {
		//Upper Utlity Arm
		Utilty_Upper.add(map(data.ch[Gripper_APOS_CH],r_min,r_max,UtlityUpperClose,UtlityUpperOpen));
	}
	if(data.ch[Right_Arm_Mode_CH] <= r_swlow){
		//Right side all close and in
		if(IAP == InterfaceArmIn){
			Interface_Arm_Door.add(RightDoorClose);
		}
		else{
			Interface_Arm_Pos.add(InterfaceArmIn);
		}
		if (ULP != UtlityLowwerClose){
			Utilty_Lower.add(UtlityLowwerClose);
		}
	}
	else if (data.ch[Right_Arm_Mode_CH] >= r_swhigh){
		//Right side arm & door
		if(IAD != RightDoorOpen){
			Interface_Arm_Door.add(RightDoorOpen);
		}
		else {
			Interface_Arm_Pos.add(map(data.ch[Interface_APOS_CH],r_min,r_max,InterfaceArmIn,InterfaceArmOut));
		}
	}
	else {
		//Lowwer Ut Arm
		Utilty_Lower.add(map(data.ch[Interface_APOS_CH],r_min,r_max,UtlityLowwerClose,UtlityLowwerOpen));
	}
	int GAD2 = Gripper_Arm_Door.get();
	int IAD2 = Interface_Arm_Door.get();
	int GAP2 = Gripper_Arm_Pos.get();
	int IAP2 = Interface_Arm_Pos.get();
	int UUP2 = Utilty_Upper.get();
	int ULP2 = Utilty_Lower.get();
	bool update_channels = false;
	if(GAD != GAD2){
		update_channels = true;
		Serial.print("  GAD: "+String(GAD2));
		body.setPWM(8,0,GAD2);
	}
	if(IAD != IAD2){
		update_channels = true;
		Serial.print("  IAD: "+String(IAD2));
		body.setPWM(1,0,IAD2);
	}
	if(GAP != GAP2){
		update_channels = true;
		Serial.print("  GAP: "+String(GAP2));
		body.setPWM(2,0,GAP2);
	}
	if(IAP != IAP2){
		update_channels = true;
		Serial.print("  IAP: "+String(IAP2));
		body.setPWM(9,0,IAP2);
	}
	if(UUP != UUP2){
		update_channels = true;
		Serial.print("  UUP: "+String(UUP2));
		body.setPWM(4,0,UUP2);
	}
	if(ULP != ULP2){
		update_channels = true;
		Serial.print("  ULP: "+String(ULP2));
		body.setPWM(5,0,ULP2);
	}
	if (update_channels){
		Serial.println("");
	}
}

void audio() { 			// Audio Control
	Audio_Volume 	= 	map(data.ch[Audio_Volume_CH], r_min, r_max, 0,30);
	int track;
	if (data.ch[Trigger_CH] <= r_swlow and currentMillis >= check_audio){
		check_audio = currentMillis + audio_delay;
		if (data.ch[Audio_Mode_CH] <= r_swlow){
			Serial.print("Low Sound bank selected: ");
			track = map(data.ch[Audio_Bank_CH],r_min,r_max,0,int(sound_effects/2)) + sound_songs + 1;
		}
		else if (data.ch[Audio_Mode_CH] >= r_swhigh) {
			Serial.print("Music bank selected: ");
			track = map(data.ch[Audio_Bank_CH],r_min,r_max,0,sound_songs);
		}
		else {
			Serial.print("High sound bank selected: ");
			track = map(data.ch[Audio_Bank_CH],r_min,r_max,int(sound_effects/2),sound_effects) + sound_songs + 1;
		}
		switch (track){
			case 9:
				Serial.print("Pedro Pedro Pedro Mode Active! ");
				Animation_Pedro = currentMillis;
				//triggerI2C(body_lights,9);
				//Dome_Animating == 1;
				//Dome_Start == millis();
              break;
			case 0:
				if (ani_clock != 0){
					Animation_Pedro = 0;
					triggerI2C2(body_lights,door_mode,neck_mode);
					triggerI2C2(dome_lights,domeA_mode,domeB_mode);
				}
				break;
			default:
				Animation_Pedro = 0;
				break;
        }
		Serial.println(" Playing file " + String(track));
		playsound(track);
	}
}

void lights(){ 			//Lighting modes
	//rewrite pending
	
	byte mode;
	if (data.ch[Trigger_CH] >= r_swhigh and currentMillis >= check_audio){
		check_audio = currentMillis + audio_delay;
		mode = map(data.ch[Animation_Select_CH],r_min,r_max,0,11);
		if (data.ch[Light_Select_CH] <= r_swlow){ // Control just body lights
			if( data.ch[Light_Mode_CH] <= r_swlow){
				door_mode = mode; //Changes Door lights
			}
			else if (data.ch[Light_Mode_CH] >= r_swhigh){
				neck_mode = mode; //Changes Neck labsights
				door_mode = mode;
			}
			else{
				neck_mode = mode; //Change Both
			}
			triggerI2C2(body_lights,door_mode,neck_mode);
		}
		else if(data.ch[Light_Select_CH] >= r_swhigh) {// Changes lights for both body and dome
			if( data.ch[Light_Mode_CH] <= r_swlow){
				door_mode = mode; //Changes Door lights
				domeA_mode = mode;
			}
			else if (data.ch[Light_Mode_CH] >= r_swhigh){
				neck_mode = mode; //Changes Neck labsights
				door_mode = mode;
				domeA_mode = mode;
				domeB_mode = mode;
			}
			else{
				neck_mode = mode; //Change Both
				domeB_mode = mode;
			}
			triggerI2C2(body_lights,door_mode,neck_mode);
			triggerI2C2(dome_lights,domeA_mode,domeB_mode);
		} 
		else { // just Dome Lights
			if( data.ch[Light_Mode_CH] <= r_swlow){
				domeA_mode = mode;
			}
			else if (data.ch[Light_Mode_CH] >= r_swhigh){
				domeA_mode = mode;
				domeB_mode = mode;
			}
			else{
				domeB_mode = mode;
			}
			triggerI2C2(dome_lights,domeA_mode,domeB_mode);
		}
	}
	
}

void playsound(byte track){
  Wire.beginTransmission(body_audio);
  Wire.write(Audio_Volume);
  Wire.write(track);
  Wire.endTransmission();
 // Serial.println("\t" + String(track) + ": Sent Via I2C to ID:" + String(body_audio));
}

void customlight(byte deviceID, byte l_string, byte start, byte end, byte red, byte green, byte blue){
	Wire.beginTransmission(deviceID);
	Wire.write(12);
	Wire.write(l_string);
	Wire.write(start);
	Wire.write(end);
	Wire.write(red);
	Wire.write(green);
	Wire.write(blue);
	Wire.write(12);
	Wire.write(12);
	Wire.endTransmission();
}

void triggerI2C(byte deviceID, byte eventID) {
  Wire.beginTransmission(deviceID);
  Wire.write(eventID);
  Wire.endTransmission();
  //Serial.println("\t" + String(eventID) + ": Sent Via I2C  to ID:" + String(deviceID));
}

void triggerI2C2(byte deviceID, byte eventID, byte eventID2) {
  Wire.beginTransmission(deviceID);
  Wire.write(eventID2);
  Wire.write(eventID);
  Wire.endTransmission();
  //Serial.println("\t" + String(eventID2) +", " + String(eventID) + ": Sent Via I2C  to ID:" + String(deviceID));
}

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 2000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

void animations(){
	ani_clock = (currentMillis - Animation_Pedro)/animation_res;//
	
	if(last_a_tick != ani_clock){
		if (Animation_Pedro > 0){
			//Animation_Pedro --;
			Serial.println("   Animation Time :" + String(ani_clock));
			if (Animation_Pedro > 0){
				Ani_Padreo();
			}
		}
		else{
			ani_clock = 0;
		}
		last_a_tick = ani_clock;
	}
}

void Ani_Padreo(){
    if (ani_clock < 130){
      triggerI2C(dome_lights,8);
      triggerI2C(body_lights,8);
    }
    else if (ani_clock < 240){
      if((ani_clock + 1) % 10 == 0){
        triggerI2C(body_lights,10);
      }
      else if((ani_clock +1) % 10 == 5){
        triggerI2C(body_lights,9);
      }
      triggerI2C(dome_lights,2 + ((Animation_Pedro/4) % 6));
    }
    else if (ani_clock < 276){
      triggerI2C(body_lights,8);
    }
    else if (ani_clock < 288){
      triggerI2C(dome_lights,10);
      triggerI2C(body_lights,10);
      //body.setPWM(4,0,UtlityLowwerOpen);
			body.setPWM(5,0,(UtlityUpperOpen+UtlityUpperClose)/2);
    }
    else if (ani_clock < 390){
      triggerI2C(body_lights,8);
      body.setPWM(4,0,UtlityLowwerClose);
      body.setPWM(5,0,UtlityUpperClose);
    }
    else if (ani_clock < 400){
      triggerI2C(dome_lights,9);
      triggerI2C(body_lights,9);
      body.setPWM(4,0,(UtlityLowwerOpen+UtlityLowwerClose)/2);
    }
    else if (ani_clock < 500) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if (ani_clock < 600){
      if((ani_clock + 1) % 6 == 0){
        triggerI2C(body_lights,10);
      }
      else if((ani_clock +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      if ((ani_clock + 1) % 6 == 0){
        body.setPWM(2,0,GripperArmOut);
        body.setPWM(9,0,InterfaceArmOut);
      }
      else 
      if ((ani_clock + 1) % 6 == 3){
        body.setPWM(2,0,GripperArmOut-200);
        body.setPWM(9,0,InterfaceArmOut+100);
      }
      triggerI2C(dome_lights,2 + ((ani_clock/4) % 6));
      body.setPWM(4,0,UtlityLowwerClose);
      body.setPWM(5,0,UtlityUpperClose);
    }
    else if (ani_clock < 700) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if(ani_clock < 800){
        body.setPWM(2,0,GripperArmIn);
        body.setPWM(9,0,InterfaceArmIn);
    }
    else if (ani_clock < 900) {      
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
    }
    else if (ani_clock < 1000){
        triggerI2C(body_lights,8);
        triggerI2C(dome_lights,0);
    }
    else if (ani_clock < 1100){
      triggerI2C(body_lights,0);
    }
    else if (ani_clock < 1200) {      
      body.setPWM(8,0, LeftDoorOpen);
      body.setPWM(1,0, RightDoorOpen);
    }
    else if (ani_clock < 1300) {
      body.setPWM(2,0,GripperArmOut);
      body.setPWM(9,0,InterfaceArmOut);
    }
    else if (ani_clock < 1400){
      if((ani_clock + 1) % 6 == 0){
        triggerI2C(body_lights,10);
        //body.setPWM(8,0, LeftDoorOpen);
      //body.setPWM(1,0, RightDoorClose);
      }
      else if((ani_clock +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      if ((ani_clock + 1) % 6 == 0){
        body.setPWM(2,0,GripperArmOut);
        body.setPWM(9,0,InterfaceArmOut);
      }
      else 
      if ((ani_clock + 1) % 6 == 3){
        body.setPWM(2,0,GripperArmOut-200);
        body.setPWM(9,0,InterfaceArmOut+100);
      }
      triggerI2C(dome_lights,2 + ((ani_clock/4) % 6));
    }
    else if (ani_clock < 1500){
      
  body.setPWM(2,0,GripperArmIn);
  body.setPWM(9,0,InterfaceArmIn);
    }
    else if (ani_clock < 1600){
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
  
				//body.setPWM(4,0,UtlityLowwerOpen);
				//body.setPWM(5,0,UtlityUpperOpen);
    }
    else if (ani_clock < 1700){
      if((ani_clock + 1) % 6 == 0){
        triggerI2C(body_lights,10);
      }
      else if((ani_clock +1) % 6 == 3){
        triggerI2C(body_lights,9);
      }
      triggerI2C(dome_lights,2 + ((ani_clock/4) % 6));
    }
	else { //if (ani_clock < 2750)
      triggerI2C(dome_lights,0);
      triggerI2C(body_lights,0);
      body.setPWM(8,0, LeftDoorClose);
      body.setPWM(1,0, RightDoorClose);
  		body.setPWM(4,0,UtlityLowwerClose);
		body.setPWM(5,0,UtlityUpperClose);
      Animation_Pedro = 0;
    }
  }