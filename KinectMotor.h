#ifndef INC_KINECTMOTOR_H
#define INC_KINECTMOTOR_H

#include <iostream>

/* LibUSB for the kinect motor driver */
#include "lusb0_usb.h"		

using namespace std;

namespace PJLib
{
	class KinectMotor
	{
		public:
			KinectMotor();
			bool isReady();
			void Close();
			double* GetAccelerometerGForce();

		private:
			struct usb_bus *busses;
			struct usb_bus *bus;
			struct usb_device *dev;
			struct usb_dev_handle *usbHandle;

			int sendMessage(int requestType, int request, int value, char *data, int size);
			char* GetMotorInfo();
			int* GetAccelerometerData();
	};
}

#endif