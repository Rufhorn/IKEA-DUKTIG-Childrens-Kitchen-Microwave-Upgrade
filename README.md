# IKEA-DUKTIG-Childrens-Kitchen-Microwave-Upgrade
There is a great variety of different projects about upgrading the IKEA childrens kitchen “DUKTIG” by a microwave-simulation out there on the internet. Inspired by those I decided to create my own simple project, especially since my wife was planning to upgrade the look of the kitchen as well for having a great Christmas present for the children. 

<p align="center"><img src="/images/Animation-MW.gif" width="75%">

# Description
The microwave’s main human interface is a single push button. On pressing the button, a bulb inside the microwave lights up, a deep tone starts to make some noise and the three-digit seven segment display starts a countdown form 10 seconds. By pressing the button again, the value of the clock can be increased by 5 seconds. Maximum is 30s. After finishing the countdown, light and sound is switched off and the microwave makes some beep noise, showing “end” on display. This describes the normal operation if the door is closed. 
If the door is open when starting the procedure, the microwave indicates it by some beep noise, shows “---” on display and does not start. If the door is being opened during running operation, the process is stopped immediately and it displays “end” plus makes the ending beep noise.

# Hardware
For the project I used components that I had laying around somewhere so I won't be able to tell you the detailed labelling of each part.

- Atmel Atmega 168 Microcontroller
- 3-digit Seven Segment Display (Common Cathode)
- Mini Speaker (8 Ohms)
- Reed Switch (opener) plus Magnet
- Optocoupler
- BC 337 Bipolar Transistors
- Pushbutton
- E5/E10 mini LED Light Bulb (6V)

The circuit was designed to be permanently disconnected from the power source by a transistor. By pushing the push button the transistor is bridged and the micro controller starts processing, keeping the transistor high. This way the controller is able to disconnect itself from the power source and no current is drawn when it is switched off.
As a power source I use a power bank outside that is placed on the back side of the kitchen. The connection is realized by a simple USB A-type cable. Inside the kitchen this is connected to the breadboard. Since I was too lazy to design a custom PCB I just mounted the breadboard with all the components on inside the kitchen.
The input parameters to the kitchen are given by the single push button that is connected to an input pin as well and the reed switch that senses the door state. 

I hope this inspires you to build your own microwave simulation. ;-)
