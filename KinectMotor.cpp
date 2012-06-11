#include "PJLib.h"

using namespace std;

namespace PJLib
{
	KinectMotor::KinectMotor()
	{
		/* Vendor ID and Product ID define the usb device */
		short VENDOR_ID = (short)0x045E;
		short PRODUCT_ID = (short)0x02B0;

		/* Initialize the usb communication */
		usb_init();
		usb_find_busses();
		usb_find_devices();
		busses = usb_get_busses();

		/* loop through the devices until the kinect motor was found */
		for (bus = busses; bus; bus = bus->next) 
		{
      		for (dev = bus->devices; dev; dev = dev->next) 
			{
				if ((dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID)) 
				{
    				break;
    			}
    		}
		}

		if(dev == NULL)
		{
			throw exception("Kinect motor could not be found!");
		}

		/* Open device */
		try
		{
			usbHandle = usb_open(dev);
		}
		catch (...)
		{
			throw exception("Device could not be opened!");
		}
	}

	int KinectMotor::sendMessage(int requestType, int request, int value, char *data, int size)
	{
		int rval = -1;
		try
		{
			rval = usb_control_msg(usbHandle, requestType, request, value, 0, data, size, 2000);

			if(rval < 0)
			{
				throw exception("Error when sending message to kinect motor! (rval < 0)");
			}
		}
		catch (...)
		{
			throw exception("Error when sending message to kinect motor!");
		}
		return rval;
	}

	bool KinectMotor::isReady()
	{
		char *buf = new char[1];						// One-element empty array
		int rval = sendMessage(0xC0, 0x10, 0, buf, 1);
		if(rval == -1)
			return false;
		return (buf[0] == 0x22);						// 0x22 means the motor is ready
	}

	void KinectMotor::Close()
	{
		if(usbHandle != NULL)
			usb_close(usbHandle);
	}

	char* KinectMotor::GetMotorInfo()
	{
		char* buf = new char[10];
		this->sendMessage(0xC0, 0x32, 0, buf, 10);
		return buf;
	}

	int* KinectMotor::GetAccelerometerData()
	{
		char* buf = this->GetMotorInfo();
		int* accel = new int[3];

		accel[0] = (int) (((short)buf[2] << 8) | buf[3]);	// X
		accel[1] = (int) (((short)buf[4] << 8) | buf[5]);	// Y
		accel[2] = (int) (((short)buf[6] << 8) | buf[7]);	// Z

		return accel;
	}

	double* KinectMotor::GetAccelerometerGForce()
	{
		double ACCEL_COUNT = 819.0;

		int* accel = this->GetAccelerometerData();
		double* accelG = new double[3];

		accelG[0] = accel[0]/ACCEL_COUNT;	// X
		accelG[1] = accel[1]/ACCEL_COUNT;	// Y
		accelG[2] = accel[2]/ACCEL_COUNT;	// Z

		return accelG;
	}
}