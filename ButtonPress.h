/*
 * ButtonPress.h
 *
 *  Created on: 22. sep. 2021
 *      Author: Mathias
 */

/*
 * Library can only handle button input from a single PORT
 *
 * Example using PORTE, confidencelevel is the number of cycles that the button must be pressed
 * initSW(PORTE, PTE, confidencelevel, <number of pins used for buttons>, <pin number1>)
 * initSW(PORTE, PTE, confidencelevel, 4, 20, 21, 22, 23)
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

int numberOfButtons;
int *g_bitNumber;				// Pin number
int *pressedConfidence;			// Counter for pressed confidence
int *releasedConfidence;		// Counter for released confidence
int confidenceLevels;			// Required confidence level
volatile char *pressed;	// Holds the pressed status of the initialized switches
GPIO_Type *PTofBits;

#define ENABLE_LCD_PORT_CLOCKS   	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
#define GPIO_W_PULLUP 0x103

/* Function prototypes */
int initSW(PORT_Type *PRT, GPIO_Type *PT, int confidenceLevel, int numBits, ...);
void readSW();
void set_Timeout(int Timeout_ms);

/* Initialize swicthes. */
int initSW(PORT_Type *PRT, GPIO_Type *PT, int confidenceLevel, int bitNums, ...)
{

	PTofBits = PT;
	// Enable all clocks
	ENABLE_LCD_PORT_CLOCKS

	// Save number of buttons and required Confidencelevel for later
	confidenceLevels = confidenceLevel;
	numberOfButtons = bitNums;

	/* Allocate memory for global variables */
	g_bitNumber = (int*) malloc(bitNums * sizeof(int));
	pressedConfidence = (int*) malloc(bitNums * sizeof(int));
	releasedConfidence = (int*) malloc(bitNums * sizeof(int));
	pressed = (char*) malloc(bitNums * sizeof(char));

	// Create a new variable to hold the variable number of arguments
	va_list v1;
	//Enable access to variadic function arguments
	va_start(v1, bitNums);
	for (int i = 0; i < bitNums; i++)
	{
		g_bitNumber[i] = va_arg(v1, int);
		//Make the pins GPIO through Port Control Register
		PRT->PCR[g_bitNumber[i]] &= ~PORT_PCR_MUX_MASK;
		// Enable pullup resistor
		PRT->PCR[g_bitNumber[i]] = GPIO_W_PULLUP;
		// Set data direction as input
		PT->PDDR &= ~(1UL << g_bitNumber[i]);
	}

	return bitNums;
}

/* The function takes no arguments and returns nothing as it is meant to be run from and interrupt handler*/
void readSW()
{
	unsigned portSwitchState = ~PTofBits->PDIR;	// Read value of port bits into portSwitchState

	for (int i = 0; i < numberOfButtons; i++)
	{
		if ((portSwitchState & (1UL << g_bitNumber[i])))
		{
			pressedConfidence[i]++;
			releasedConfidence[i] = 0;
		} else
		{
			pressedConfidence[i] = 0;
			releasedConfidence[i]++;
		}
	}

	for (int i = 0; i < numberOfButtons; i++)
	{
		if (pressedConfidence[i] > confidenceLevels)
		{
			pressed[i] = 1;
		} else if (releasedConfidence[i] > confidenceLevels)
		{
			pressed[i] = 0;
		}
	}

	return;
}

#endif /* BUTTONPRESS_H_ */
