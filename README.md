#TBN Astromech Control System
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
    The most universal way of receiving radio control data from a remote, limited to 10 channels due to polling rate of the input. By default these channels are mapped to digital pins 2 threw 11

