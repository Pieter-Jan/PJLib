#include "PJLib.h"

namespace PJLib
{
	Point3D::Point3D(double X, double Y, double Z)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
	}

	Point3D Point3D::operator - (Point3D p)
	{
		return Point3D(this->X - p.X, this->Y - p.Y, this->Z - p.Z);
	}

	Point3D Point3D::operator + (Point3D p)
	{
		return Point3D(this->X + p.X, this->Y + p.Y, this->Z + p.Z);
	}
}