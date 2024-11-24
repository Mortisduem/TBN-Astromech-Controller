# TBN Astromech Control System
This is a customizable electronic package for dome rotation, sound, and motion inspired by the Padawan360 (https://github.com/dankraus/padawan360) control system using RC remotes instead of the Xbox USB Reciver typicaly used.

## Contents
- [TBN Astromech Control System](#TBN)
- [Intro](#intro)
- [Currently Supported Features](#currently)


## Intro

This is a felxible control system for 1:1 scale remote controled Astromech that was inspired by the padawan360 sytem by dankraus. It is capaiable of translating input from PWM, IBUS or SBUS RC systems to be able to control foot, dome, and shoulder drive motors. Aswell as being able to trigger lights, sounds, run preprogramed animations or any thing else over I2C. The core to the system is the use of a Arduino Mega 2650 that serves as the hub of the system

## Currently Suported Features
- ### Flexible Remote Control connections to provide flexible control schemes
 #### 10 Channel raw PWM
   The most universal way of receiving radio control data from a remote, limited to 10 channels due to polling rate of the Mega. By default these channels are mapped to digital pins 2 - 11 to be mapped as channels 0-9 for control of functions. This mode currently does not enable controling the foot drive mix, so I recomend connecting the 2 PWM channels mixed to drive the droid from the reciever dirrectly to a Cytron MDDS30 or Sabertooth motor driver in PWM mode. 
 #### FrSky IBUS
   A non-inverted Serial comunication used by some FrSky remote systems, limited to 14 channels of control and mapped to channels 0-13 for assignment. Serial3 is used to receive the data from the receiver on the Mega, connect pin 15 to the servo data port on the receiver. While the protocal supports two way communications, only recive is used currently
 #### Futuba and FrSky SBUS
   A inverted Serial comunication commonly used by Futaba and FrSky remote systems, limited to 16 channels and mapped to channels 0-15 for assignment.Serial3 is used to receive the data from the receiver on the Mega, connect pin 15 to the assigned channel used for Sbus out on the receiver, this will depend on how your transmitter and receiver are setup. 
  
- ### Foot Drive Motor Control
#### Cytron MDDS30 Motor Driver
  Support for the [Cytron MDDS30](https://www.cytron.io/p-30amp-7v-35v-smartdrive-dc-motor-driver-2-channels) motor driver in simple serial mode, connected to the Mega on Serial1. While limited to 60amps peak and 30 amps cont. it is still a good match for most droids. This controler lacks some of the extra features of the Sabertooth 32a.

- ### Dome Drive Motor Control Options
#### PWM Signaling
  PWM signal with option of Direction signal to control genaric RC ESCs and compatible motor controllers like the Cytron MD10C
  
