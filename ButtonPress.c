/*
 * ButtonPress.c
 *
 *  Created on: 13. okt. 2021
 *      Author: Mathias
 */
#include "ButtonPress.h"


int numberOfButtons;
int *g_bitNumber;				// Pin number
int *pressedConfidence;			// Counter for pressed confidence
int *releasedConfidence;		// Counter for released confidence
int confidenceLevels;			// Required confidence level
volatile char *pressed;	// Holds the pressed status of the initialized switches
volatile duration_t *pressedDur;// Duration flag of the press
volatile int *pressedTime;		// Press time counter
GPIO_Type *PTofBits;			// Holds the port information


/* Initialize swicthes. */
int initSW(PORT_Type *PORTX, GPIO_Type *PTX, int confidenceLevel, int numBits, ...)
{

	PTofBits = PTX;
	// Enable all clocks
	ENABLE_LCD_PORT_CLOCKS

	// Save number of buttons and required Confidencelevel for later
	confidenceLevels = confidenceLevel;
	numberOfButtons = numBits;

	/* Allocate memory for global variables */
	g_bitNumber = (int*) malloc(numBits * sizeof(int));
	pressedConfidence = (int*) malloc(numBits * sizeof(int));
	releasedConfidence = (int*) malloc(numBits * sizeof(int));
	pressed = (char*) malloc(numBits * sizeof(char));
	pressedDur = (duration_t*) malloc(numBits * sizeof(duration_t));
	pressedTime = (int*) malloc(numBits * sizeof(int));

	// Create a new variable to hold the variable number of arguments
	va_list v1;
	//Enable access to variadic function arguments
	va_start(v1, numBits);
	for (int i = 0; i < numBits; i++)
	{
		g_bitNumber[i] = va_arg(v1, int);
		//Make the pins GPIO through Port Control Register
		PORTX->PCR[g_bitNumber[i]] &= ~PORT_PCR_MUX_MASK;
		// Enable pullup resistor
		PORTX->PCR[g_bitNumber[i]] = GPIO_W_PULLUP;
		// Set data direction as input
		PTX->PDDR &= ~(1UL << g_bitNumber[i]);
	}

	return numBits;
}

/* The function takes no arguments and returns nothing as it is meant to be run from and interrupt handler*/
void readSW()
{
	unsigned portSwitchState = ~PTofBits->PDIR;	// Read value of port bits into portSwitchState

	// For each switch
	for (int i = 0; i < numberOfButtons; i++)
	{
		// If switch is pressed
		if ((portSwitchState & (1UL << g_bitNumber[i])))
		{
			// Increment the pressed confidence
			pressedConfidence[i]++;
			// Switch is pressed - set released confidence to 0
			releasedConfidence[i] = 0;
		} else
		{
			// Switch is not pressed. Set pressed confidence to 0
			// and increment released confidence
			pressedConfidence[i] = 0;
			releasedConfidence[i]++;
		}
	}
	// For each switch - Check if pressed confidence or released confidence is
	// above threshold
	for (int i = 0; i < numberOfButtons; i++)
	{
		if (pressedConfidence[i] > confidenceLevels)
		{
			pressed[i] = 1;
			pressedDur[i] = noPress;
		} else if (releasedConfidence[i] > confidenceLevels)
		{
			pressed[i] = 0;
		}
	}
	// Call long / short press function
	readSLSW();
	return;
}
void readSLSW()
{
	for (int i = 0; i < numberOfButtons; i++)
	{
		if (pressed[i])
		{

			pressedTime[i] ++;
		}
		else
		{
			if (pressedTime[i] < LONG_PRESS_TIME)
			{
				pressedDur[i] = shortPress;
			}
			else
			{
				pressedDur[i] = longPress;
			}
			pressedTime[i] = 0;
		}
	}
	return;
}
