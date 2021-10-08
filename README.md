# button_lib_KL25Z
Debounce library for FRDM-KL25Z

The debounce library is designed to be used with a timer interrupt.

The interrupt must call readSW(). This will update the pressed[] array that contains the pressed flags (1 for pressed, 0 for released).
The library is setup for active low switches with internal pullup enabled.

#### USAGE ####
initSW(PORT_Type, GPIO_Type, confidenceLevel, numBits, ...)

confidenceLevel is the number of cycles that the button must be pressed or released before the flag changes state.
numBits is the number of pins to be debounced - numBits is used to access the variadic arguments. 

Example usage with buttons on PORTD pins 20, 21 and 22.

initSW(PORTD, PTD, 10, 3, 20, 21, 22)

####

readSW() must be called from a timer interrupt. Preferably at 1 ms intervals.

pressed[] contains the pressed flag for each switch in the order they were initialized

####
Long and short buttonpress are also registered. These are saved in pressedDur[] in the 
same order as pressed[].

pressedDur[] can have three values. 'noPress', 'shortPress' & 'longPress'. 'shortPress' and 
'longPress' is updated when the switch is released. Next time the switch is pressed, the 
value of pressedDur[] will become 'noPress'.
The user can set the value of pressedDur[] to 'noPress' once he/she has registered the press.