#ifndef INC_ROBOTPOINT_H
#define INC_ROBOTPOINT_H

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace PJLib
{
	class RobotPoint
	{
		private:

		public:
			double X, Y, Z, RX, RY, RZ;
			double J1, J2, J3, J4, J5, J6;
			string spaceType;
			int figure;
			RobotPoint();
			RobotPoint(double X, double Y, double Z, double RX, double RY, double RZ);
			RobotPoint(double X, double Y, double Z, double RX, double RY, double RZ, int figure);
			RobotPoint(double XorJ1, double YorJ2, double ZorJ3, double RXorJ4, double RYorJ5, double RZorJ6, string spaceType);
			string ToString();
			RobotPoint operator -(RobotPoint b);
			RobotPoint operator +(RobotPoint b);
	};
}

#endif