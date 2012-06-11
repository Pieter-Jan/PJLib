#include "PJLib.h"

namespace PJLib
{
	/* Constructors */
	RobotPoint::RobotPoint()
	{
	}

	RobotPoint::RobotPoint(double X, double Y, double Z, double RX, double RY, double RZ)
	{
		this->spaceType = "P";
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->RX = RX;
		this->RY = RY;
		this->RZ = RZ;
		this->figure = -1;
	}

	RobotPoint::RobotPoint(double X, double Y, double Z, double RX, double RY, double RZ, int figure)
	{
		this->spaceType = "P";
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->RX = RX;
		this->RY = RY;
		this->RZ = RZ;
		this->figure = figure;
	}

	RobotPoint::RobotPoint(double XorJ1, double YorJ2, double ZorJ3, double RXorJ4, double RYorJ5, double RZorJ6, string spaceType)
	{
		this->spaceType = spaceType;
		if(spaceType == "P")
		{
			this->X = XorJ1;
			this->Y = YorJ2;
			this->Z = ZorJ3;
			this->RX = RXorJ4;
			this->RY = RYorJ5;
			this->RZ = RZorJ6;
			this->figure = -1;
		}
		if(spaceType == "J")
		{
			this->J1 = XorJ1;
			this->J2 = YorJ2;
			this->J3 = ZorJ3;
			this->J4 = RXorJ4;
			this->J5 = RYorJ5;
			this->J6 = RZorJ6;
		}
		else
		{
			throw new exception("RobotPoint: Spacetype is not correct.");
		}
	}

	string RobotPoint::ToString()
	{
		ostringstream outputString;
		outputString << spaceType;

		if(this->spaceType == "P")
		{
			outputString << "(" << this->X << ",";
			outputString << this->Y << ",";
			outputString << this->Z << ",";
			outputString << this->RX << ",";
			outputString << this->RY << ",";
			outputString << this->RZ << ",";
			outputString << this->figure << ")";
		}
		else if(this->spaceType == "J")
		{
			outputString << "(" << this->J1 << ",";
			outputString << this->J2 << ",";
			outputString << this->J3 << ",";
			outputString << this->J4 << ",";
			outputString << this->J5 << ",";
			outputString << this->J6 << ")";
		}
		else
		{
			throw new exception("RobotPoint: Spacetype is not correct.");
		}

		return outputString.str();
	}

	/* Operators */
	RobotPoint RobotPoint::operator -(RobotPoint b)
	{
		return RobotPoint(this->X - b.X, this->Y - b.Y, this->Z - b.Z, this->RX - b.RX, this->RY - b.RY, this->RZ - b.RZ);
	}

	RobotPoint RobotPoint::operator +(RobotPoint b)
    {
		return RobotPoint(this->X + b.X, this->Y + b.Y, this->Z + b.Z, this->RX + b.RX, this->RY + b.RY, this->RZ + b.RZ);
    }
}