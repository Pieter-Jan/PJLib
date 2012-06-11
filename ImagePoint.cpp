#include "PJLib.h"

namespace PJLib
{
	ImagePoint::ImagePoint(double row, double col)
	{
		this->row = row;
		this->col = col;
	}

	double ImagePoint::DistanceTo(ImagePoint p)
        {
            return sqrt(pow(p.row - this->row, 2) + pow(p.col - this->col, 2));
        }

	ImagePoint ImagePoint::MidPoint(ImagePoint p)
        {
            double row = (this->row + p.row) / 2;
            double col = (this->col + p.col) / 2;
            return ImagePoint(row, col);
        }

	/* Angles */
	double ImagePoint::AngleXAxisInRadians(ImagePoint p)
        {
            return atan((this->row - p.row) / (p.col - this->col));
        }

	double ImagePoint::AngleYAxisInRadians(ImagePoint p)
        {
            return (PI/2 - atan((this->row - p.row) / (p.col - this->col)));
        }

	/* Transformations and transpositions */
	ImagePoint ImagePoint::TransformPoint(double phi, double length)
        {
            return ImagePoint(this->row + sin(phi) * length, this->col - cos(phi) * length);
        }

	ImagePoint ImagePoint::TransposePoint(ImagePoint newCenterPoint, double angle)
        {
            ImagePoint pointInOld = ImagePoint(this->row, this->col);

            double row, col, rowHelp, colHelp;
            /* Translation */
            colHelp = pointInOld.col - newCenterPoint.col;
            rowHelp = pointInOld.row - newCenterPoint.row;

            /* Rotation */
            col = colHelp * cos(angle) - rowHelp * sin(angle);
            row = colHelp * sin(angle) + rowHelp * cos(angle);

            return ImagePoint(row, col);
        }

	/* Operators */
	ImagePoint ImagePoint::operator -(ImagePoint b)
        {
            return ImagePoint(this->row - b.row, this->col - b.col);
        }

	ImagePoint ImagePoint::operator +(ImagePoint b)
        {
            return ImagePoint(this->row + b.row, this->col + b.col);
        }
}