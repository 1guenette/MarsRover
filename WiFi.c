/**
 * @file WiFi.c
 * @breif File used to enable and initialize wifi. Provided by TA, modified by us.
 *
 * @author Original: ericm Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */

#include <string.h>

#include "WiFi.h"
#include "uart.h"

#define COMMAND_PIN		0x04

#define COMMAND_START	0
#define COMMAND_STOP	1

#define RETVAL_SUCCESS	0

uint8_t _sendCommand(uint8_t command, uint8_t* param, uint8_t len);

/**
 * Used to start up the Wifi on the TM4C123.
 * @param  psk Password to Wifi
 * @return	-1 if unsuccessful 0 if it is
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
int WiFi_start(char *psk) {
	//Initialize UART
	uart_init();

	//Initialize command pin for WiFi connection
	SYSCTL_RCGCGPIO_R |= BIT1; //Turn on GPIO for WiFi command mode
	GPIO_PORTB_DEN_R |= COMMAND_PIN; //Enable pin2
	GPIO_PORTB_DIR_R |= COMMAND_PIN;
	GPIO_PORTB_DATA_R &= ~COMMAND_PIN; //Clear command pin

	//Wait for pin to settle
	timer_waitMillis(1);
	timer_waitMillis(1);
	uint8_t response = _sendCommand(COMMAND_START, psk, strlen(psk) + 1);

	if(response != RETVAL_SUCCESS) {
		return -1;
	}
	else {
		return 0;
	}
}

/**
 * Used to disable the wifi on the TM4C123.
 * @return stop command
 */
int WiFi_stop() {
	return _sendCommand(COMMAND_STOP, NULL, 0);
}

/**
 * Used to send the stop command to wifi.
 * @param  command the command to be sent
 * @param  param   the parameters for the command
 * @param  len     the length of the command
 * @return         the response from sending the command
 */
uint8_t _sendCommand(uint8_t command, uint8_t* param, uint8_t len) {
	//Raise command flag
	GPIO_PORTB_DATA_R |= COMMAND_PIN;

	//Send WiFi command

	uart_sendChar(command);

	//Send parameter (if applicable)
	if(len > 0) {

		uart_sendBuffer(param, len);
	}

	//Wait for response from WiFi microcontroller
	int response = uart_receive();

	//Lower the command flag
	GPIO_PORTB_DATA_R &= ~COMMAND_PIN;

	return response;
}
