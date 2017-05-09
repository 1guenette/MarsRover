/**
*	@file ping.c
*	@breif This file contains various functions used with the ping sensor.
*	Given by TA's modified by us.
*
*	@author Matt Post, Zachary Glanz, Eric Middleton Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson.
*
*	@date 4/14/17
*/

#include "ping.h"

volatile uint32_t risingEdge;
volatile uint32_t fallingEdge;
volatile uint32_t pulseLength;
volatile bool isRisingEdge;

//Local function prototypes
void ping_enableIRQ(void);
void TIMER3B_Handler(void);

/**
* 	This method initializes the registers required for ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
void ping_init()
{
	///Initialize the GPIO pin

	SYSCTL_RCGCGPIO_R |= BIT1; //Turn on PORTB Sys Clk
	GPIO_PORTB_DEN_R |= BIT3; //Enable PB3


	///Initialize Timer3B

	SYSCTL_RCGCTIMER_R |= BIT3; // Turn on clock to TIMER3

	//Configure the timer for input capture mode
	TIMER3_CTL_R &= ~TIMER_CTL_TBEN; // 0x100 disable timerB to allow us to change settings
	TIMER3_CFG_R |= TIMER_CFG_16_BIT; // 0x4 set to 16 bit timer
	TIMER3_TBMR_R |= (TIMER_TBMR_TBMR_CAP | TIMER_TBMR_TBCDIR | TIMER_TBMR_TBCMR); //0x17 set for capture mode, up count
	TIMER3_TBPR_R = 0;
	TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH; // 0xC00 set bits 11:10 for both edge triggering
	TIMER3_TBILR_R = 0xFFFF; // set top counter to maximum 24 bit value (~1.049s max)

	//clear TIMER3B interrupt flags
	TIMER3_ICR_R = (TIMER_ICR_TBMCINT | TIMER_ICR_CBECINT | TIMER_ICR_CBMCINT | TIMER_ICR_TBTOCINT);
	TIMER3_IMR_R |= (TIMER_IMR_CBEIM | TIMER_IMR_TBTOIM); // 0x00000400 enable TIMER3B capture and time-out interrupts


	///Configure interrupts

	//intialize local interrupts
	//NVIC_PRI9_R |= 0x00000020; //give priority 1 to interrupt 36(T3CCP1) set bits 7:5 to 1
	//look for TIMER 3B interupt 36 0x0D0
	NVIC_EN1_R |= 0x00000010; //enable interrupt for IRQ36(T3CCP1) set bit 36-32=4, a write of 0 has no effect

	//register ISR
	IntRegister(INT_TIMER3B, TIMER3B_Handler);

	//intialize global interrupts
	//ping_enableIRQ();
	IntMasterEnable();
}

/**
* 	This method enables the ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
void ping_trigger()
{
	//Set PB3 as normal GPIO
	GPIO_PORTB_AFSEL_R &= ~BIT3; //0x8
	GPIO_PORTB_PCTL_R &= ~(0x0000F000);//off

	//Set port to output
	GPIO_PORTB_DIR_R |= BIT3; // 0x8 Pin 3 is T1CCP1 Pin 2 is blue LED

	//Pulse Ping Sensor High to Low with ~ 5us delay
	GPIO_PORTB_DATA_R |= BIT3; //0x8 set PB3 high
	timer_waitMicros(5);
	GPIO_PORTB_DATA_R &= ~BIT3; //0x8 set PB3 low

	isRisingEdge = true; //Flag to indicate rising edge first

	GPIO_PORTB_DIR_R &= ~BIT3; //Set PB3 to input

	//Configure GPIO PB3 to input capture mode
	GPIO_PORTB_AFSEL_R |= BIT3;
	GPIO_PORTB_PCTL_R |= 0x00007000; //enable pin3 alternate function 7 T3CCP1

	//enable TIMER3B and start counting
	TIMER3_CTL_R |= TIMER_CTL_TBEN;// 0x100
}

/**
* 	This method uses the timer3 registers to determine the falling edge/rising edge.
* 	@author Matt Post, Zachary Glanz, Eric Middleton
*
*/
void TIMER3B_Handler(void)
{
	if(TIMER3_MIS_R & TIMER_MIS_CBEMIS) {// fixed condition
		if(isRisingEdge)
		{
			TIMER3_ICR_R = TIMER_ICR_CBECINT; //clear capture interrupt flag

			risingEdge = ((int)TIMER3_TBPS_R << 16) | TIMER3_TBR_R; //captures time of rising edge event

			isRisingEdge = false; //now capturing falling edge
		}
		else
		{
			TIMER3_ICR_R = TIMER_ICR_CBECINT; //clear capture interrupt flag

			fallingEdge = ((int)TIMER3_TBPS_R << 16) | TIMER3_TBR_R; //capture time of falling edge

			TIMER3_CTL_R &= ~TIMER_CTL_TBEN; //disable timerB
		}
	}
}

/**
* 	This method gets the distance using the ping sensor.
* 	@author Matt Post, Zachary Glanz, Eric Middleton Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson.
*
*/
float ping_getDistance()
{
	//Correction for overflow situation
	int overflow = (fallingEdge < risingEdge);

	//get pulse length
	pulseLength = fallingEdge + (overflow<<24) - risingEdge;

	//calculate distance in mm
	return pulseLength / PING_FACTOR;
}
