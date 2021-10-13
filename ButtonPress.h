/*
 * ButtonPress.h
 *
 *  Created on: 22. sep. 2021
 *      Author: Mathias
 */

/*
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
 */

#ifndef BUTTONPRESS_H_
#define BUTTONPRESS_H_

#include "board.h"
#include <stdlib.h>
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include <stdarg.h>

#define LONG_PRESS_TIME 700 //Time in ms

typedef enum
{
	noPress,
	shortPress,
	longPress,

} duration_t;

// Variable are declared in ButtonPress.c
extern int numberOfButtons;
extern int *g_bitNumber;				// Pin number
extern int *pressedConfidence;			// Counter for pressed confidence
extern int *releasedConfidence;		// Counter for released confidence
extern int confidenceLevels;			// Required confidence level
extern volatile char *pressed;	// Holds the pressed status of the initialized switches
extern volatile duration_t *pressedDur;// Duration flag of the press
extern volatile int *pressedTime;		// Press time counter
extern GPIO_Type *PTofBits;			// Holds the port information


#define ENABLE_LCD_PORT_CLOCKS   	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
#define GPIO_W_PULLUP 0x103

/* Function prototypes */

// Initialize switches
int initSW(PORT_Type *PORTX, GPIO_Type *PTX, int confidenceLevel, int numBits, ...);
// Read switch states and set pressed[] flag if switch is above threshold
void readSW();
// Updates pressedDur[] with shortPress or longPress
void readSLSW();

#endif /* BUTTONPRESS_H_ */
