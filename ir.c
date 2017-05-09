
/**
* 	@file ir.c
* 	@brief Using IR sensor on GPIO PortB pin 4 using ADC0 channel AIN10
* 	Used the IR code supplied by the TA's.
*
*	Created on: Jul 12, 2016
* 	@author	Original: Dane Larson	Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/

#include "ir.h"

volatile isr_int_value;

/**
* 	This method enables the registers to initialize the IR sensor.
* 	@author Dane Larson
*
*/

void ir_init()
{
	//enable ADC 0 module on port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

	//enable clock for ADC
	SYSCTL_RCGCADC_R |= 0x1;

	//enable port B pin 4 to work as alternate functions
	GPIO_PORTB_AFSEL_R |= 0x10;

	//set disable digital
	GPIO_PORTB_DEN_R &= ~0x10;

	//disable analog isolation for the pin
	GPIO_PORTB_AMSEL_R |=0x10;

	//initialize the port trigger source as processor (default)
	GPIO_PORTB_ADCCTL_R = 0x00;

	//disable SS0 sample sequencer to configure it
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN0;

	//initialize the ADC trigger source as processor (default)
	ADC0_EMUX_R = ADC_EMUX_EM0_PROCESSOR;

	//set 1st sample to use the AIN10 ADC pin
	ADC0_SSMUX0_R |= 0x000A;

	//enable raw interrupt
	ADC0_SSCTL0_R |= (ADC_SSCTL0_IE0 | ADC_SSCTL0_END0);

	//enable hardware oversampling (averaging)
	ADC0_SAC_R |= ADC_SAC_AVG_64X;

	//re-enable ADC0 SS0
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN0;
}

/**
* 	This function gets the input from the IR sensor.
* 	@author Dane Larson
*/
int ir_getValue()
{
	//initiate SS0 conversion
	ADC0_PSSI_R=ADC_PSSI_SS0;

	//wait for ADC conversion to be complete
	while((ADC0_RIS_R & ADC_RIS_INR0) == 0){
		//wait
	}

	//read value from FIFO
	int value =0;
	value = ADC0_SSFIFO0_R;

	//clear interrupt
	ADC0_ISC_R=ADC_ISC_IN0;

	return value;
}

/**
* 	This function is used to convert the readings from the IR sensor into centimeters so that we can properly know where to go.
* 	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*
*/
float ir_getDistance(){
	isr_int_value = ir_getValue();
		if(isr_int_value > 660){
			float distance = isr_int_value*(-0.02079)+54.17;
			return distance;
		}else{
			float squared = (isr_int_value)*(isr_int_value);
			float distance = (.000005640972)*(squared)-(0.16359229)*isr_int_value+(140.0176);
			return distance;
		}

}
