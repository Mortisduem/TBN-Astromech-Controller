# TBN Astromech Control System
This is a custom electronic package for dome rotation, sound, and motion inspired by the Padawan360 (https://github.com/dankraus/padawan360) control system using RC remotes instead of the Xbox USB Receiver typically used.

## Contents
- [TBN Astromech Control System](#TBN)
- [Intro](#intro)
- [Currently Supported Features](#currently)


## Intro

This is a flexible control system for 1:1 scale remote controlled Astromech that was inspired by the padawan360 system by dankraus. It is capable of translating input from PWM, IBUS or SBUS RC systems to be able to control foot, dome, and shoulder drive motors. As well as being able to trigger lights, sounds, run programmed animations or any thing else over I2C. The core to the system is the use of a Arduino Mega 2650 that serves as the hub of the system, with the use of other Arduino boards to handle things like lights and sounds.

## Currently Supported Features
- ### Flexible Remote Control connections to provide flexible input of controls
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
  #### Sabertooth Motor Driver
  Support for Sabertooth motor controllers is planned but currently not implemented due to conflicts with Cytron controllers.

- ### Dome Drive Motor Control Options
  #### PWM Signaling
  PWM signal with option of Direction signal to control generic RC ESCs or other compatible motor controllers like the Cytron MD10C
  #### Syren 10
  Support planed but not implemented yet due to conflicts with Cytron controllers
  
- ### Basic Switch of Leg Modes
  Support to switch between 2 and 3 leg modes with use of two motors being driven by a Cytron MDDS10. Left and right side shoulders driven independently. Control will set a speed limit of foot drives while not in 3 leg mode to help limit tip over.
  
- ### Independent Lighting Controllers
  Arduino Nanos used to control banks of Neopixel strips. 10 pre-programmed lighting modes with support for customization commands for custom animations run from the hub. 

- ### Remap-able Functions
  Functions can be remapped to different RC channels to better accommodate remotes with fewer available channels. 
  
- ### Independent Audio Control 
  Uses a Arduino Nano to translate I2C commands to Serial to use a DY MP3 player or similar boards.
