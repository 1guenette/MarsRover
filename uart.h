/**
 * @file uart.h
 * @breif The header files for the uart class.
 *
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 * @date 04/06/17
 *
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void uart_sendChar(char data);

char uart_receive(void);

void uart_init(void);

void uart_sendBuffer(param, len);

void uart_sendNum(int num);

void UART1_Handler(void);

void uart_sendStr(const char *data);

void uart_flush(void);

#endif /* UART_H_ */
