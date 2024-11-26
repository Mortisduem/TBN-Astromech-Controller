# TBN Astromech Control System
The "To Be Named" Control System is a custom electronic package for dome rotation, sound, and motion inspired by the Padawan360 (https://github.com/dankraus/padawan360) control system using RC remotes instead of the Xbox USB Receiver typically used.

## Contents
- [TBN Astromech Control System](#TBN)
- [Intro](#intro)
- [Currently Supported Features](#currently)
- [Planned Features](#planned)
- [Setting Up the hub](#setting) 


## Intro

This is a flexible control system for 1:1 scale remote controlled Astromech that was inspired by the padawan360 system by dankraus. It is capable of translating input from PWM, IBUS or SBUS RC systems to be able to control foot, dome, and shoulder drive motors. As well as being able to trigger lights, sounds, run programmed animations or any thing else over I2C. The core to the system is the use of a Arduino Mega 2650 that serves as the hub of the system, with the use of other Arduino boards to handle things like lights and sounds.
This is a flexible control system for 1:1 scale remote controlled Astromech that was inspired by the padawan360 system by dankraus. It is capable of translating input from PWM, IBUS or SBUS RC systems to be able to control foot, dome, and shoulder drive motors. As well as being able to trigger lights, sounds, run programmed animations or any thing else over I2C. The core to the system is the using a Arduino Mega 2650 that serves as the hub of the system, with the use of other Arduino boards to handle things like lights and sounds.

## Currently Supported Features
- ### Flexible Remote Control connections to provide flexible input of controls
- ### Different Remote Protocols to provide flexible input options
  #### 10 Channel PWM
  The most universal way of receiving radio control data from a remote, limited to 10 channels due to polling rate of the Mega. By default these channels are mapped to digital pins 2 - 11 to be mapped as channels 0-9 for control of functions. This mode currently does not enable controlling the foot drive mix, so I recommend connecting the 2 PWM channels mixed to drive the droid from the receiver directly to a Cytron MDDS30 or Sabertooth motor driver in PWM mode. 
  #### FrSky IBUS
  A non-inverted Serial communication used by some FrSky remote systems, limited to 14 channels of control and mapped to channels 0-13 for assignment. Serial3 is used to receive the data from the receiver on the Mega, connect pin 15 to the servo data port on the receiver. While the protocol supports two way communications, only receive is used currently
  #### Futuba and FrSky SBUS
  A inverted Serial communication commonly used by Futaba and FrSky remote systems, limited to 16 channels and mapped to channels 0-15 for assignment.Serial3 is used to receive the data from the receiver on the Mega, connect pin 15 to the assigned channel used for Sbus out on the receiver, this will depend on how your transmitter and receiver are setup. You will need to invert the signal for the Mega to be able to read the input correctly. A 2N7000 MOSFET can be used to do this with .
  | 2N7000 |      |
  |--------|------|
  | Source | GND  |
  | Gate   | SBUS |
  | Drain  | TX3  |
  
- ### Foot Drive Motor Control
  #### Cytron MDDS30 Motor Driver
  Support for the [Cytron MDDS30](https://www.cytron.io/p-30amp-7v-35v-smartdrive-dc-motor-driver-2-channels) motor driver in simple serial mode, connected to the Mega on Serial1. While limited to 60amps peak and 30 amps cont. it is still a good match for most droids. This controller lacks some of the extra features of the Sabertooth 32a.
  #### PWM Signals
  Mixed PWM signal output on the board for other Motor Controllers   

- ### Dome Drive Motor Control Options
  #### PWM Signaling
  PWM signal with option of Direction signal to control generic RC ESCs or other compatible motor controllers like the Cytron MD10C
  
- ### Basic Switch of Leg Modes
  Support to switch between 2 and 3 leg modes with use of two motors being driven by a Cytron MDDS10. Left and right side shoulders driven independently. Control will set a speed limit of foot drives while not in 3 leg mode to help limit tip over.
  
- ### Independent Lighting Controllers
  Arduino Nanos used to control banks of Neopixel strips. 10 pre-programmed lighting modes with support for customization commands for custom animations run from the hub. 

- ### Remap-able Functions
  Functions can be remapped to different RC channels to better accommodate remotes with fewer available channels. 
  
- ### Independent Audio Control 
  Use an Arduino Nano to translate I2C commands to Serial for a DY MP3 player or similar boards.

## Planned Features
<<<<<<< Updated upstream
  ### Sabertooth Motor Driver Support
  Support for Sabertooth motor controllers is planned but currently not implemented due to conflicts with Cytron controllers.
=======
  - Support for Sabertooth motor controllers is planned but currently not implemented due to conflicts with Cytron controllers.
  - Support of switch Channels 17 and 18 in SBUS mode

## Setting Up
  Most options are set in the config folder, you will need to correct the links to the path to be the correct file path each config header file for your PC or mac
  Body_Config.h is used for most setting for the main Hub board
  - ### Remote_Mode is used to select the receiver is sending data to the main hub.
    - 0 disables the polling wait for good RC data, it is intended to be used for with a static droid.
    - 1 SBUS Mode, Serial3 is used to listen on TX3 and during startup will wait for good data before starting the rest of the droid. Supports upto 16 channels of servo control
    - 2 IBUS Mode, Serial3 is used to listem on TX3 and will hold the startup till it has good data before starting the rest of the droid. Supports upto 14 channels of servo control
    - 10 PWM Mode, Pins 2 to 11 are used to read upto 10 channels of PWM from a RC receiver. I recomend wiring the PWM for the foot drives and Dome drive directly to the motor controllers if you use this mode
  - Foot_Drives configure the control method used by the Hub to drive the foot drives
    - 0 disables Motor driver and motor mixing
    - 1 Cytron motor driver, Serial1 is to command a Cytron MDDS30 in simple serial mode.
    - 10 PWM out, Provides a mixed PWM split into left and right drive signals. FD_PWM_LEFT and FD_PWN_RIGHT define the pins used for this output
  - Dome_Drive defines the control method used for for the dome drive
    - 0 disables
    - 1 PWM + DIR, PWM for the magnitude of the spin and Direction (high/low), DD_PWN_PIN pin out for the magnitude while DD_DIR_PIN provides the direction to turn
    - 2 PWM Only, Same as DIR put stop is at 50% and direction is above or below this point
  - Leg_232 is used to enable a basic 2-3-2 leg switching control scheme driving a Cytron MDDS10 in simple serial mode using Serial2, 4 limit switches are needed to be wired to ground when triggered. Leg_Slow cap the top speed the foot drives can go while the droid is not in 3 leg mode to help prevent tip overs
    | Limit   | PIN |
    |---------|-----|
    | RightFr | 51  |
    | RightBk | 53  |
    | LeftFr  | 50  |
    | LeftBk  | 52  |
  - ### BUS Configs
  These are used to set the min and max range of values to be expected in each mode from the receiver. Values outside of this range will be interpreted as a bad serial packet from the receiver, and avgerage value will used to till the next good packet. swhigh and swlow are the values for switch possitions and asume the use of a 3 possition switch, values below sw_low with be seen as a switch in the low position, above sw_high in a high postion, and between the two to be middle. 2 position switches generaly are above or below sw_low and sw_high
  - ### Functuction Mapping
    Assign what channel the hub should asign to what function, multiple fuctions can be assigned to the same channel
    Here are a few notes on what each fucntion does:
    - Drive_LR_CH and Drive_FB_CH : generate the foot drive mix, left/right and front/back 
    - Drive_Dome_CH : spin of the dome
    - Audio_Volume_CH : on the fly volume adjustment, set to -2 to have the audio trigger at max volume or -1 for 50% volume
    - Gripper_APOS_CH and Interface_APOS_CH : Used to puppet the Gripper, Interface, and Utlity Arms outside of animation cycles
    - Left_Arm_Mode_CH and Right_Arm_Mode_CH : Open and close doors for the Gripper and Interface arms, and selects if the APOS groups are controling the Utlity arms or Gripper and Interface arms
    - Data_Door_CH : Open and close the Data pannel door
    - Light_Mode_CH:
	- Light_Select_CH:
	- Audio_Mode_CH:
	- Audio_Bank_CH:
	- Animation_Select_CH:
	- Shoulder_Mode_CH:
	- Trigger_CH: Switching low will trigger the selected Audio. Switching high will trigger lights
>>>>>>> Stashed changes
