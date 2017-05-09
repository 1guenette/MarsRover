/**
 * @file movement.h
 * @breif This is the header file used for the movement class.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
#include "lcd.h"
#include "open_interface.h"

void movement(oi_t *sensor_data, int meters);
void turn(oi_t *sensor_data, int degrees);
void race(oi_t *sensor_data);
