#ifndef INC_IMAGEPOINT_H
#define INC_IMAGEPOINT_H

#include <cmath>

using namespace std;

namespace PJLib
{
	class ImagePoint
	{
		private:

		public:
			double row;
			double col;
			ImagePoint(double row, double col);
			double DistanceTo(ImagePoint p);
			ImagePoint MidPoint(ImagePoint p);
			double AngleXAxisInRadians(ImagePoint p);
			double AngleYAxisInRadians(ImagePoint p);
			ImagePoint TransformPoint(double phi, double length);			
			ImagePoint TransposePoint(ImagePoint newCenterPoint, double angle);
			ImagePoint operator -(ImagePoint b);
			ImagePoint operator +(ImagePoint b);
	};
}

#endif