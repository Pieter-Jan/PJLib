#ifndef INC_POINT3D_H
#define INC_POINT3D_H

using namespace std;

namespace PJLib
{
	class Point3D
	{
		public:
			double X, Y, Z;
			Point3D() {};
			Point3D(double X, double Y, double Z);
			Point3D operator - (Point3D p);
			Point3D operator + (Point3D p);
	};
}

#endif