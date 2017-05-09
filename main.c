/**
*	@file main.c
*	@breif This file contains various functions used during our final project as well as the main method we used
*	when demoing our final project.
*
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*
*	@date 4/14/17
*/

#include <math.h>
#include "lcd.h"
#include "ping.h"
#include "timer.h"
#include "driverlib/interrupt.h"
#include "WiFi.h"
#include "movement.h"
#include "open_interface.h"
#include "music.h"
#include "ir.h"

unsigned mid_width;// 22000==90, 36500==180, 7000==0;		//Higher value, more counter-clockwise  BOT0
unsigned pulse_period = 320000;

int count = 0;
int degrees = 0;
int object_count = 0;
int smallest_location = 0;
int smallest_width = 99999;
int index = -1;

int IR_distance;
int sonar_distance;

int flag = 0;

char string_builder[] = "";

/**
* This method is used to initalize the timer.
* @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void clock_timer_init(){
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	GPIO_PORTB_PCTL_R |= 0x00007000;
	TIMER3_CTL_R &= ~(TIMER_CTL_TBEN);
	TIMER3_CFG_R |= TIMER_CFG_16_BIT;
	TIMER3_TBMR_R = 0x03 | 0x04;
	TIMER3_ICR_R = TIMER_ICR_TBTOCINT;
	TIMER3_IMR_R |= TIMER_IMR_CBEIM;
	NVIC_EN3_R |= 0x20;
	TIMER3_CTL_R |= TIMER_CTL_TBEN;
}

/**
* This method is used to move the servo in order to scan 180 degrees.
* @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
* @param degree	Determines the degrees the servo should move
*/
void move_servo(unsigned degree)
{
	if(degree <= 180 && degree >= 0)
	{

		mid_width = (163.89 * degree) + 7083.3;
		TIMER1_TBMATCHR_R = (pulse_period - mid_width) & 0xFFFF; // if you want to move servo to the middle
		TIMER1_TBPMR_R = (pulse_period - mid_width) >> 16;
		timer_waitMillis(25);
	}
}

/**
*	This method is used to get the diameter of an object detected by the sensor
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*	@param angle		The degree at which we are looking at the object
*	@param startDist	The first know instance the object is detected at
*	@param endDist		The last know instance the object is detected at
*/
double getDiameter(double angle, int startDist, int endDist)
{
	double val =  sqrt(((startDist*startDist)+(endDist*endDist))-((2*startDist*endDist)*cos((angle*3.14159/180))));
	return val;
}

/**
*	This method shows the raw data of objects less than 30 centimeters away in a 180 degree scan.
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void detect2()
{
	degrees = 0;
	move_servo(degrees);
	timer_waitMillis(500);
	while(degrees < 181)
	{

		char data[100];

		int irVal = ir_getDistance();
		ping_trigger();
		int pingVal = ping_getDistance();

		timer_waitMillis(30);

		if(pingVal>30)
		{
			sprintf(data, "%d %i\n\r",degrees, pingVal);
			uart_sendStr(data);
		}
		else
		{
			sprintf(data, "%d OBJECT\n\r", degrees);
			uart_sendStr(data);
		}

		degrees +=1;
		move_servo(degrees);
		//lcd_printf("%i, %i cm", pingVal, cent);
	}

	move_servo(90);
}

/**
*	This method shows the raw data of objects in a 180 degree scan along with their distance away.
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void detect4()
{
	degrees = 0;
	move_servo(degrees);
	timer_waitMillis(500);
	while(degrees < 181)
	{
		char data[100];

		int irVal = ir_getDistance();
		ping_trigger();
		int pingVal = ping_getDistance();

		timer_waitMillis(30);


		sprintf(data, "%d \t%i \t %i\n\r",degrees, pingVal, irVal);
		uart_sendStr(data);


		degrees +=1;
		move_servo(degrees);
	}

	move_servo(90);


}

/**
*	This 180 degree scan only displays the number of objects nearby along with their location, in degrees.
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void detect3()
{
	degrees = 0;
	move_servo(degrees);
	timer_waitMillis(500);
	while(degrees < 180)
	{
		run_cycle();
	}
	uart_sendStr("Scan Complete\r\n");

}

/**
*	This method is used in detect 3 to accurately get the objects along with their size and distance.
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void run_cycle()
{
		move_servo(degrees);
	    degrees += 1;
	    IR_distance = 0;
	    int i;
	    for(i = 0; i < 10; i++){
			IR_distance += ir_getDistance();
		}
		ping_trigger();
		sonar_distance = ping_getDistance();
		IR_distance = (IR_distance/10);


		if(IR_distance < 30 && flag == 0){ //should use IR
			calculateObject();
		}

		lcd_printf("Degrees: %d\nIR: %d\nSonar: %d\n", degrees, IR_distance, sonar_distance);


		timer_waitMillis(10);
}

/**
*	This method is the specific method that actually calculates the objects and their size and distance away.
*	@author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void calculateObject(){
	int distance;
	int length;
	int location;
	char * string;

	int startdistance = sonar_distance;

	unsigned int start = degrees;
	flag = 1;
	while(IR_distance < 30){//should use IR
		if(degrees < 0)
			return;
		if(degrees > 180){
			return;
		}
		run_cycle();
	}
	flag = 0;
	unsigned int end = degrees;
	int radial_width = end - start;
	int actual_width = getDiameter(radial_width, startdistance, sonar_distance);

	if(radial_width < 3){
		return;
	}
	uart_sendStr("Object found!\n\r");
	if(smallest_width > actual_width && actual_width > 0){
		smallest_width = actual_width;
		smallest_location = (start) + (radial_width/2);
	    index = object_count + 1;
	}
	if(actual_width !=0){


	length = actual_width;
	location = (start) + (radial_width/2);
	distance = sonar_distance;



	uart_sendStr("Size: ");
	uart_sendNum(length);
	uart_sendStr("cm");
	uart_sendStr("\t");
	uart_sendStr("Distance:");
	uart_sendNum(distance);
	uart_sendStr("cm");
	uart_sendStr("\t");
	uart_sendStr("Location:");
	uart_sendNum(location);
	uart_sendStr(" degrees");
	uart_sendStr("\n\r");
	}
}

/**
*	This method is used to detect the cliff in the middle of the course.
*	@author	Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*	@param *sensor_data	This is the data from the sensors on the cybot
*/
int cliffFrontDetect(oi_t *sensor_data)
{
	if(sensor_data->cliffFrontLeftSignal | sensor_data->cliffFrontRightSignal)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
* This method is used to initalize the servo in order to be able to scan objects.
* @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void servo_timer_init(){
	SYSCTL_RCGCGPIO_R |= BIT1; // Turn on clock to GPIOportB5
	GPIO_PORTB_DIR_R |= BIT5; //PB5 to output
	GPIO_PORTB_AFSEL_R |= BIT5;
	GPIO_PORTB_PCTL_R = 0x700000; //T1CCP1
	GPIO_PORTB_DEN_R |= 0b00100000;
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; //turn on clk for timer1
	TIMER1_CTL_R &= 0xFEFF; //disable timer to config
	TIMER1_CFG_R = TIMER_CFG_16_BIT; //set size of timer to 16
	TIMER1_TBMR_R |= 0b00001010; //periodic and PWM enable
	int pulse_period = 320000;
	TIMER1_TBILR_R = pulse_period & 0xFFFF; //lower 16 bits of the interval
	TIMER1_TBPR_R = pulse_period >> 16; //set the upper 8 bits of the interval
	TIMER1_TBMATCHR_R = (320000-24000) & 0xFFFF;
	TIMER1_TBPMR_R = (320000 - 24000) >> 16;
	//GPTMTAMATCHR1 = pulse_period - mid_width; // if you want to move servoto the middle
	TIMER1_CTL_R |= 0x0100; //enable timer
}

/**
* This is our main method which is used to send commands to uart in order to be able to make the robot do certain commands.
* @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/
void main()
{


	lcd_init();
	lcd_printf("initializing...\n");
	ping_init();
	servo_timer_init();
	ir_init();
	uart_init();
	//WiFi_start("password\0");



	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	oi_setWheels(0, 0); // Make sure
	//wheels aren't moving upon start-up.



	while(1)//BOT 5

	{
		oi_update(sensor_data);

		char cmd = uart_receive();
		lcd_printf("char %s", cmd);

		if(cmd == 'a')
		{
			turn(sensor_data, 93);
		}
		else if(cmd == 'w')
		{
			movement(sensor_data, 100);
		}
		else if(cmd == 'd') //turn right
		{
			turn(sensor_data, -93);
		}
		else if(cmd == 's')
		{
			movement(sensor_data, -50);
		}
		else if(cmd == 'q')
		{
			turn(sensor_data, 15);
		}
		else if(cmd == 'e')
		{
			turn(sensor_data, -15);
		}

		if(cmd == '1')
		{
			detect2();
		}
		if(cmd == '2')
		{
			detect3();
		}
		if(cmd == '3')
		{
			load_songs(0);
			oi_play_song(0);
		}
		if(cmd == '4')
		{
			detect4();
		}
	}
}
