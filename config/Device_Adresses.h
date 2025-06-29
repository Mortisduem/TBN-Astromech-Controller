/*
Configurable varibles/const shared on multible boards 
*/

// Adresses for I2C Items and Other I2c Item
const int body_servos = 0x40; //Adress 64 Body board 16 channels
const int dome_servos = 0x41; //Adress 65 Dome board 16 channels
const int  bd1_servos = 0x42; //Adress 66 BD1 Dome rider
const int body_lights = 0x50; // Nano in body handles Neopixs And one Matrix Array
const int dome_lights = 0x51; // Nano Dome lights
const int  bd1_lights = 0x52; // Nano BD1 Dome Rider lights
const int   dome_acel = 0x53;
const int  body_audio = 0x30; //To offload Audio contols from Hub to Nano and stop serial out delays from delaying main loop

//-----  Global Varibles common to all boards -----
//Build Version, Negitive Values for Beta builds, mostly for checking boards at boot
//Positive version numbers will eventualy disable Verbose Debug Serial 
const float version = -1.0;		

/*
Used to try and keep clocks common to all boards 
	loop_delay: 16 for ~ 60 loops per second (delay in millis)
				Recommend 32 if using raw RC PWM for control ~30 loops per second
    loop_mult : multiplies the loop_delay to slow loops for lighting animations
				16 x 5 for a 80ms delay or ~12 updates a second 
   audio_mult : delay audio checks to help prevent stuttering audio when triggered 
				16 x 10 for ~6 times a second
animation_res : resolution of animations time checking
				divides current millis by animation_res to  
			    100 provides a resesolution of 0.1 of a second tickrate
*/
const int loop_delay = 16; 
const int loop_mult = 5;
const int audio_mult = 10;
const int animation_res = 100;

/* Constants for Audio file counts, Audio board and Hub board currently only use*/
const int sound_effects = 164;
const int sound_songs = 10;
