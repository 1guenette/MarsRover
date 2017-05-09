/**
* 	@file ir.h
* 	@brief Using IR sensor on GPIO PortB pin 4 using ADC0 channel AIN10
* 	Used the IR code supplied by the TA's.
*
*	Created on: Jul 12, 2016
* 	@author	Dane, Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
#include "lcd.h"
#include "timer.h"
#include "stdio.h"
#include "math.h"

void ADC_init(void);
unsigned ADC_read(char channel);
float ir_getDistance();
