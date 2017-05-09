/**
* @file 	movement.c
* @breif	This class was used to make the robot move and turn certain distances
*	and degrees.
* @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
*/

#include "open_interface.h"

/**
 * This method is used to, not only make the robot move forward and backwards,
 * but also to detect cliffs, objects, and other obstacles.
 * @param sensor_data This is the sensor data from the cybots sensors
 * @param meters      This is the distance input, though input in centimeters, not meters.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 */
void movement(oi_t *sensor_data, int meters)
{
	int sum = 0;

	if(meters < 0) //MOVING BACK
	{
		oi_setWheels(-150, -150); // move backward; full speed

		while (sum > (meters))
		{
			oi_update(sensor_data);
			sum += sensor_data->distance;
		}

		oi_setWheels(0, 0); // stop
	}

	else
	{
		oi_setWheels(150, 150); //MOVE FORWARD
		while (sum < meters)
		{

			oi_update(sensor_data);
			sum += sensor_data->distance;

			int colorRightFront = sensor_data->cliffFrontRightSignal;
			int colorLeftFront = sensor_data->cliffFrontLeftSignal;
			int colorRightSide = sensor_data->cliffRightSignal;
			int colorLeftSide= sensor_data->cliffLeftSignal;

			int bumperCenterLeft = sensor_data->lightBumpCenterLeftSignal;
			int bumperCenterRight = sensor_data->lightBumpCenterRightSignal;
			int bumperMidRight = sensor_data->lightBumpFrontRightSignal;
			int bumperMidLeft = sensor_data->lightBumpFrontLeftSignal;
			int bumperLeft = sensor_data->lightBumpLeftSignal;
			int bumperRight = sensor_data-> lightBumpRightSignal;

			if((colorRightFront > 2799 || colorLeftFront > 2799) && (colorRightFront < 2900 || colorLeftFront < 2900))//IF DETECT WHITE LINE
			{
				oi_setWheels(0,0);
				uart_sendStr("WHITE LINE FRONT\n\r");
				return;
			}
			else if((colorRightSide > 2799 || colorLeftSide > 2799) &&  (colorRightSide < 2900 || colorLeftSide < 2900)){
				oi_setWheels(0,0);
				if(colorRightSide > 2799)
				{
					uart_sendStr("WHITE LINE-RIGHT Side\n\r");
				}
				if(colorLeftSide > 2799)
				{
					uart_sendStr("WHITE LINE- LEFT Side\n\r");
				}

				return;
			}
			else if(sensor_data->cliffFrontLeft == 1 | sensor_data->cliffFrontRight == 1 | sensor_data->cliffLeft == 1 | sensor_data->cliffRight == 1)
			{
				oi_setWheels(0, 0);


				if(sensor_data->cliffFrontRight == 1 | sensor_data->cliffFrontLeft == 1)
				{
					uart_sendStr("CLIFF-CENTER\n\r");
				}

				if(sensor_data->cliffRight == 1)
				{
					uart_sendStr("CLIFF-RIGHT Side\n\r");
				}
				if(sensor_data->cliffLeft == 1)
				{
					uart_sendStr("CLIFF- LEFT Side\n\r");
				}

				movement(sensor_data, -30);
				return;
			}

			else if(bumperCenterLeft > 100 || bumperCenterRight > 100){
				oi_setWheels(0, 0);
				uart_sendStr("OBJECT CENTER DON'T MOVE\n\r");
				return;
			}

			else if(bumperMidLeft > 100 || bumperMidRight > 100){
					if(bumperMidLeft > 100){
						oi_setWheels(0, 0);
						uart_sendStr("OBJECT LEFT FRONT DON'T MOVE\n\r");
						return;
					}
					else if(bumperMidRight > 100){
						oi_setWheels(0, 0);
						uart_sendStr("OBJECT RIGHT FRONT DON'T MOVE\n\r");
						return;
					}
			}

			else if(bumperLeft > 200 || bumperRight > 200){
				if(bumperLeft > 200){
					oi_setWheels(0, 0);
					uart_sendStr("OBJECT LEFT DON'T MOVE\n\r");
					return;
				}
				else if(bumperRight > 200){
					oi_setWheels(0, 0);
					uart_sendStr("OBJECT RIGHT DON'T MOVE\n\r");
					return;
				}

			}

			else if((colorRightFront > 999 || colorLeftFront > 999) && (colorRightFront < 1300 || colorLeftFront < 1300)) //IF DETECT BLACK LINE
			{
				movement(sensor_data, 40);
				oi_setWheels(0, 0);
				uart_sendStr("finished", strlen("finished"));
				return;
			}

		}
		oi_setWheels(0, 0); // stop
	}
}

/**
 * This method is used to make the robot turn a certain number of degrees in
 * a certain direction.
 * @author Sam Guenette, Luke Tang, Vishal Joel, Jacob Nachman, and Ryan Samuelson
 * @param sensor_data The sensor data from the cybot.
 * @param degrees     The direction we want the robot to turn in degrees.
 */
void turn(oi_t *sensor_data, int degrees)
{
	int sum = 0;

	if(degrees > 0) //Counter-Clockwise(Left)
	{
		oi_update(sensor_data);
		oi_setWheels(150, -150); // move backward; full speed

		while (sum < (degrees))
		{
			oi_update(sensor_data);
			sum += sensor_data->angle;
		}

		oi_setWheels(0, 0); // stop
	}

	if(degrees < 0) //Clockwise(Right)
	{
		oi_update(sensor_data);
		oi_setWheels(-150, 150); // move backward; full speed

		while (sum > (degrees))
		{
			oi_update(sensor_data);
			sum += sensor_data->angle;
		}

		oi_setWheels(0, 0); // stop
	}
}
