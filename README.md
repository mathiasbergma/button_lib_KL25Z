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
