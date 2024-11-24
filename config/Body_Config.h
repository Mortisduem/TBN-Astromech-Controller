/*
Main Hub Constants/Varibles using a Aurduino Mega 2650 as the main hub of the droid
These varibles and consts curently only used on Hub
*/

/*
Control interface or how we connect the Reciever to the Droid
   0 - No Remote bypasses polling of remote, use Serial terminal to command droid
   1 - SBUS serial 16 Channels Max (0-15) requires inverting signal to work
   2 - IBUS serial 14 Channels Max (0-13)
  10 - Raw PWM signaling  10 Channels Max, recomend wiring PWM for drives directly to Cytron Drives as  
*/
#define Remote_Mode 1

/*
Foot Motor controller options
   0 - No Motor Drivers
   1 - Cytron MDDS30
   2 - Sabertooth (not supported yet)
  10 - PWN Out, on Mega
*/
#define Foot_Drives 1
#define FD_PWM_LEFT  46
#define FD_PWM_RIGHT 44

/*
Dome Motor Control
   0 - No Motor Driver
   1 - PWM + DIR for Cytron MD10C or other similar  
   2 - PWM Only 
   3 - Syren, This will also disable 2-3 mode
*/
#define Dome_Drive 1
#define DD_PWN_PIN 12
#define DD_DIR_PIN 13

/*
Shoulder Motor Control currently only suports the Cytron MDDS10 for basic switching of legs between 2 and 3 leg mode
Leg_Slow is the pressentage to slow down drive speed, only used while not in 3 leg mode with back endstops triggered and 2-3-2 option is enabled
*/
#define Leg_232 1
const int Leg_Slow = 50;

/*
Dome Accerator is intended to use an Adafruit ADXL345 mounted on the dome of the droid to allow easier locking of the direction of the dome while turing the body not enabled yet.
*/
#define Dome_Accerator false

/*
Configurable range limits for differnt types of remote control systems
values outside of the min/max with be interperted as part of a bad read of the reciever
swhigh/swlow are used to check the possistion of switches, 3 possistion switches are assumed to be used
*/
//SBUS Config
const int sbus_min = 172;
const int sbus_max = 1811;
const int sbus_swlow = 661;
const int sbus_swhigh = 1322;

//IBUS Config
const int ibus_min = 1000;
const int ibus_max = 2000;
const int ibus_swlow = 1333;
const int ibus_swhigh = 1666;

//RC PWM Config
const int rc_min = -127;
const int rc_max = 127;
const int rc_swlow = -42;
const int rc_swhigh = 42;

/* 
Channel Asignment for different functions, assigning the same channel to a function means both will use that channel.
SBUS and FBUS mode channels are shifted down 1 (Channel 1 is read as Channel 0), 16 channels in SBUS and 14 channels in IBUS
Raw PWM is mapped to pin numbers 2-11 to channels 0-10 
*/
#define Drive_LR_CH 0
#define Drive_FB_CH 1
#define Drive_Dome_CH 2
#define Audio_Volume_CH 3
#define Gripper_APOS_CH 4
#define Interface_APOS_CH 5
#define Left_Arm_Mode_CH 6
#define Right_Arm_Mode_CH 7
#define Data_Door_CH 8
#define Light_Mode_CH 9
#define Light_Select_CH 12
#define Audio_Mode_CH 10
#define Audio_Bank_CH 11
#define Animation_Select_CH 13
#define Shoulder_Mode_CH 14
#define Trigger_CH 15


/* Constants for idle sound effect, if no sound is called after X time then play random sounds randomly */
const unsigned long idle_Timer = 300000;  // Time to wait after last called sound
   const unsigned int idle_Min = 20000;       // Minium wait between idle sounds 
   const unsigned int idle_Max = 45000;      // Max wait

// Cytron Drives Locations
#define LEG_CD 19
#define SHOULDER_CD 17
#define BAUDRATE  9600
#define BOARD_ID  0
//- fPivYLimt  : The threshold at which the pivot action starts
//                This threshold is measured in units on the Y-axis
//                away from the X-axis (Y=0). A greater value will assign
//                more of the joystick's range to pivot actions.
//                Allowable range: (0..+100)
float fPivYLimit = 50.0;

//MB: Gripperarm amimaton variables...
int GripperAni1a = 0; //Gripper animation counter
int InterfaceAni1a = 0; //Interface animation counter

//Door Open and Close positions for the Servos (Breadpan doors)
int LeftDoorOpen=150;
int LeftDoorClose=480;
int RightDoorOpen=550;
int RightDoorClose=175;

//gripper values
int GripperOpen=270;
int GripperClose=325;
int GripperArmIn=100;
int GripperArmOut=570;

//interface values
int InterfaceOpen=325;
int InterfaceClose=270;
int InterfaceArmIn=570;
int InterfaceArmOut=170;

//Utility values
int UtlityUpperOpen = 580;
int UtlityUpperClose = 200;
int UtlityLowwerOpen = 500;
int UtlityLowwerClose = 200;

//Holo home position
int Holo1_1=400;
int Holo2_1=415;

//Holo postion 1
int Holo1_2=300;
int Holo2_2=425;

//Holo position 2
int Holo1_3=300;
int Holo2_3=300;

//Holo postion 3
int Holo1_4=500;
int Holo2_4=400;

//Holo postion 4
int Holo1_5=420;
int Holo2_5=530;
  
  
/*
----- Global Varibles for Main Hub -----
No changes should be needed after this point
Values asigned here are boot defaults and may change depending on droid state and enabled options
*/
//Light tracking and other variables
int domeA_mode;
int domeB_mode;
int neck_mode;
int door_mode;
char inChar;
int sound_select;
int Drive_Right; 
int Drive_Forward; 
int Drive_Dome;  
int Audio_Volume = 10;
// Sbus Varibles
bool bad_bus; //set TRUE if SBUS or IBUS data reads outside of range set
//Shoulder Drive Varibles
int SD_Left = 0;
int SD_Right = 0;
int Shoulder_Pos = 0;
int Shoulder_Target = -1;
bool RF;
bool RR;
bool LF;
bool LR;
bool Shoulder_homed = true;

//MB: Holoprojector amimaton variables...
 bool isHPOn = false;
 int HoloAni1a = 0; //Holo animation counter
 int HoloAni1b = 0; //Holo animation counter 2
 unsigned int Animation_Pedro;
 int HoloPWMstart =0; //Holo PWM starting postion (0, 4, 8 are the three holoprojectors)

// Foot drive outputs
int speedLeft, speedRight; 
int RC_DomeSpeed;
int RC_DomeDir;
int test;

// Dome animation
unsigned int  Dome_Ani_Speed;
int Dome_Animating = 0;
unsigned long Dome_Start;

//Vars for idle waits and contol checks
unsigned long currentMillis;
unsigned long lastMillis;
unsigned long idle_Delay;
unsigned long check_audio;

unsigned int sef_count;
unsigned int sef_start;
unsigned int mus_count;
unsigned int mus_start;

// Door Open logic tracking
bool Gripper_Door = false;
bool Interface_Door = false;
bool Data_Door = false;

//sub board coms check
bool body_servo_good = false;


const int SABERTOOTHBAUDRATE = 9600;