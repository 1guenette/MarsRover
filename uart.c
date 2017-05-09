/**
 *	@file uart.c
 *	@brif Used to enable uart in order to use wifi connection.
 *
 *
 *  @author Jacob Nachman, Vishal Joel, and Luke Tang
 *  @date 04/06/2017
 */

#include "uart.h"

volatile uint32_t isr_counter = 0;
volatile char isr_char_buffer[50];

/**
 * Method used to enable the UART registers.
 * @author Jacob Nachman, Vishal Joel, and Luke Tang
 */
void uart_init(void){
	//enable clock to GPIO, R1 = port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	//enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx (SeeDataSheet)
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
	//enable alternate functions on port b pins 0 and 1
	GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);
	//enable Rx and Tx on port B on pins 0 and 1
	GPIO_PORTB_PCTL_R |= 0x00000011;
	//set pin 0 and 1 to digital
	GPIO_PORTB_DEN_R |= (BIT0 | BIT1);
	//set pin 0 to Rx or input
	GPIO_PORTB_DIR_R &= ~BIT0;
	//set pin 1 to Tx or output
	GPIO_PORTB_DIR_R |= BIT1;


	uint16_t iBRD = 8;
	uint16_t fBRD = 44;

	//turn off uart1 while we set it up
	UART1_CTL_R &= ~(UART_CTL_UARTEN);
	//set baud rate
	UART1_IBRD_R = iBRD;
	UART1_FBRD_R = fBRD;
	//set frame, 8 data bits, 1 stop bit, no parity, no FIFO
	UART1_LCRH_R = UART_LCRH_WLEN_8 ;
	//use system clock as source
	UART1_CC_R = UART_CC_CS_SYSCLK;
	//re-enable enable RX, TX, and uart1
	UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

/**
 * Method used to send a character via UART.
 * @param data The character you wish to be sent over UART.
 * @author Jacob Nachman, Vishal Joel, and Luke Tang
 */
void uart_sendChar(char data) {
//wait until there is room to send data
	while (UART1_FR_R & 0x20) {
	}
//send data
	UART1_DR_R = data;
}

/**
 * Method that allows UART to revieve data.
 * @return  returns the data to be recieved by UART.
 * @author Jacob Nachman, Vishal Joel, and Luke Tang
 */
char uart_receive(void) {
	char data = 0;
//wait to receive
	while (UART1_FR_R & UART_FR_RXFE) {
	}
//mask 4 error bits and grab 8 data bits
	data = (char) (UART1_DR_R & 0xFF);
	return data;
}

/**
 * Sends a buffer to UART.
 * @param buffer The buffer to be sent to UART.
 * @param len    Length of the buffer.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
void uart_sendBuffer(char* buffer,int len){
	while((len--) > 0){
		uart_sendChar(*(buffer++));
	}
}

/**
 * This method is used to clear the UART registers.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
void uart_flush(void) {
	volatile char dummy;
	while(!(UART1_FR_R & UART_FR_RXFE)) {
		//Flush FIFO
		dummy = (char)(UART1_DR_R & 0xFF);
	}
}

/**
 * This method is used to send strings to UART.
 * @param	data The string to be send to UART
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
//sends char array over uart1
void uart_sendStr(const char *data){
    while(*data != '\0') {
        uart_sendChar(*data);
        data++;
    }
    //send null character
    uart_sendChar(0);
}

/**
 * This method is used to send numbers to UART.
 * @param	num The number to be send to UART
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
void uart_sendNum(int num) {
	char str[15];
	sprintf(str, "%d", num);
	uart_sendStr(str);
}

/**
 * The Interrupt handler for UART which is called via sending and recieving information.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
void UART1_Handler(void){
	//received a byte
	if(UART1_MIS_R & UART_MIS_RXMIS){
		isr_char_buffer[isr_counter]= (char)(UART1_DR_R & 0xFF);
		isr_counter++;
		UART1_ICR_R |= UART_ICR_RXIC;
	}
	//sent a byte
	else if(UART1_MIS_R & UART_MIS_TXMIS){
		UART1_ICR_R |= UART_ICR_TXIC;
	}

}
