#ifndef INC_KINECT_H
#define INC_KINECT_H

/* openNI headers */
#include "XnOS.h"
#include "XnCppWrapper.h"

/* Halcon header */
#include "HalconCpp.h"
#include "HIOStream.h"

/* Custom Bayer Decoder */
#include "BayerDecoder.hxx"

/* KinectMotor for reading accelerometer data */
#include "KinectMotor.h"

/* ImagePoint for working with 2D images */
#include "ImagePoint.h"

/* Logger for awesome logging */
#include "Logger.h"

#include <iostream>

using namespace xn;		// OpenNI
using namespace Halcon;	// Halcon
using namespace std;

namespace PJLib
{
	class Kinect
	{
		#define SAMPLE_XML_PATH "C:/Program Files (x86)/OpenNI/Data/SamplesConfig.xml"

		private:
			Context context;
			ScriptNode scriptNode;
			KinectMotor *kinectMotor;
			HImage imageRGB;

			/* Generators */
			DepthGenerator depthGenerator;
			ImageGenerator rgbGenerator;

			/* Meta Data */
			DepthMetaData depthMD;
			ImageMetaData rgbMD;

			/* Field of View */
			double FovH, FovV, XtoZ, YtoZ;

			/* Buffers */
			const XnRGB24Pixel* pImage;
			const XnDepthPixel* pDepth;
			float* xArray;
			float* yArray;
			float* zArray;

			/* Custom bayer decoder */
			unsigned char *rawBayerPtr;
			ImageBayerGRBG *bayerDecoder;

			/* Status Booleans */
			bool isCustomBayerDecoding;
			bool isDepthEnabled;
			bool isRGBEnabled;

			/* Methods */
			void ReadFrame(bool isRGBRequired, bool isDepthRequired);
			HImage CreateHImageRGBFromBuffer();
			XnPoint3D Calculate3DPointWithDepthValue(unsigned short depthValue, ImagePoint imagePoint);

			/* Logger */
			Logger *logger;
			void WriteLogEntry(string entry);

		public:
			Kinect();
			~Kinect();
			bool isAccelerometerFound;
			void Initialise();
			void ReInitialise();

			/* Image grabbers */
			HImage GrabHImageRGBandXYZ(HImage &imageX, HImage &imageY, HImage &imageZ);
			HImage GrabHImageRGB();

			void GetAccelerometerGForces(double& gForceX, double& gForceY, double& gForceZ);
	};

}
#endif