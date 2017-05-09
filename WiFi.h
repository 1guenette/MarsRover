/**
 * @file WiFi.h
 * @breif Wifi class header file, created by the TA's modified by us as needed.
 *
 * @author Modified: Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <stdint.h>
#include "uart.h"

int WiFi_start(char *psk);

int WiFi_stop();


#endif /* WIFI_H_ */
