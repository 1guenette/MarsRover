/**
*	@file ping.h
*	@breif This file contains various functions used with the ping sensor.
*	Provided by TA, modified by us as needed.
*
*	@author Matt Post, Zachary Glanz, Eric Middleton Modified: @author Matt Post, Zachary Glanz, Eric Middleton Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson.
*
*	@date 4/14/17
*/

#ifndef PING_H_
#define PING_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>
#include "Timer.h"
#include "driverlib/interrupt.h"


#define BIT0		0x01
#define BIT1		0x02
#define BIT2		0x04
#define BIT3		0x08
#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80

#define PING_FACTOR	930.24f //Factor to get distance in cm

/**
* 	This method initializes the registers required for ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
void ping_init();

/**
* 	This method enables the ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
void ping_trigger();

/**
* 	This method gets the distance using the ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
float ping_getDistance();

#endif
