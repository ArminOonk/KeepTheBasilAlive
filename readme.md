# Keep The Basil Alive

I love basil but I always forget to give water. So after a week I end up with a dead basil plant. This has to stop! 

As a nerd there is only one solution for this problem (Improving my memory has proven to be very difficult). Build an automatic watering system for my basil plants. 

The electronics and software are straight forward: [Teensy LC](https://www.pjrc.com/teensy/teensyLC.html), 5V peristaltic pump, soil moisture sensor, leds/button/potentiometer. 

This is the code and laser cut designs for Keep The Basil Alive.

More information and progress can be found on:
https://hackaday.io/project/5504-keep-the-basil-alive


## Connections

These are the connections with the Teensy LC (NOTE: not everything is wired up at this moment).

Name     	| Pin (Teensy LC) 	| Type
-------------------------------------
Pump     	| 10				| Output
Button   	| 2					| Input
Led Button 	| 3					| PWM
Led1		| 4					| PWM
Led2		| 6					| PWM
Led3		| 9 				| PWM
Moisture sensor	| A0			| Analog
Potentiometer	| A1			| Analog
Waterlevel 		| A3			| Analog/Touch
Leak detect		| A4 			| Analog

## Code

The code is writen using Arduino with the Teensy LC add-on.
https://www.pjrc.com/teensy/td_download.html
