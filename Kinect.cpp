#include "PJLib.h"

namespace PJLib
{
	using namespace std;

	/* Constructor & Destructor */
	Kinect::Kinect()
	{
		this->Initialise();
	}

	Kinect::~Kinect()
	{
		/* Stop Generating */
		this->context.StopGeneratingAll();

		/* Release the old stuff */
		this->rgbGenerator.Release();
		this->depthGenerator.Release();
		this->depthMD.Free();
		this->rgbMD.Free();
		this->context.Shutdown();
		this->context.Release();
	}

	/* Initialisation functions */
	void Kinect::Initialise()
	{
		/* Read the XML file */
		XnStatus rc = XN_STATUS_OK;
		rc = this->context.InitFromXmlFile(SAMPLE_XML_PATH, this->scriptNode);
		if (rc != XN_STATUS_OK)
		{
			throw exception(xnGetStatusString(rc));
		}

		/* Exception handling */
		if (rc == XN_STATUS_NO_NODE_PRESENT)
		{
			throw exception(xnGetStatusString(rc));
		}
		else if (rc != XN_STATUS_OK)
		{
			throw exception(xnGetStatusString(rc));
		}

		rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
		if (rc != XN_STATUS_OK)
		{
			throw exception("No depth node exists! Check your XML.");
		}

		rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, rgbGenerator);
		if (rc != XN_STATUS_OK)
		{
			throw exception("No image node exists! Check your XML.");
		}

		/* Grab Initial Data */
		this->depthGenerator.GetMetaData(this->depthMD);
		this->rgbGenerator.GetMetaData(this->rgbMD);

		/* Initialise Custom Bayer Decoder if necessairy */
		this->isCustomBayerDecoding = true;
		if(this->isCustomBayerDecoding)
		{
			rawBayerPtr = (unsigned char*)malloc(this->rgbMD.YRes()*this->rgbMD.XRes()*3);
			this->bayerDecoder = new ImageBayerGRBG(ImageBayerGRBG::EdgeAware);
			
			/* Set the parameters for the Bayer. Grayscale to get raw Bayer pattern. */ 
			rc = this->rgbGenerator.SetIntProperty("InputFormat", 6);
			rc = this->rgbGenerator.SetPixelFormat(XN_PIXEL_FORMAT_GRAYSCALE_8_BIT); 
			this->rgbMD.AllocateData((int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), XN_PIXEL_FORMAT_GRAYSCALE_8_BIT);
		}
		else
		{
			this->rgbMD.AllocateData((int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), XN_PIXEL_FORMAT_RGB24);
		}

		/* Get the meta data */
		this->depthMD.AllocateData((int)this->rgbMD.XRes(), (int)this->rgbMD.YRes());

		/* Align depth and RGB images */
		depthGenerator.GetAlternativeViewPointCap().SetViewPoint(rgbGenerator);

		/* Start Generating */
		rc = context.StartGeneratingAll();
		if (rc != XN_STATUS_OK)
		{
			throw exception("Could not start generating!");
		}

		/* Get the FOV Constants */
		XnFieldOfView FOV;
        depthGenerator.GetFieldOfView(FOV);  //1,01446867075074
		FovH = FOV.fHFOV;
		FovV = FOV.fVFOV;
        XtoZ = tan(FovH / 2) * 2;
        YtoZ = tan(FovV / 2) * 2;

		/* Initialise the kinect motor */
		try
		{
			kinectMotor = new KinectMotor;
			this->isAccelerometerFound = true;
		}
		catch (exception& ex)
		{
			//throw exception(ex);
			this->isAccelerometerFound = false;
			this->WriteLogEntry("KINECT: Accelerometer not found.");
		}

		/* Initialise the memory*/
		this->imageRGB = HImage();
		this->xArray = new float[this->rgbMD.YRes()*this->rgbMD.XRes()];
		this->yArray = new float[this->rgbMD.YRes()*this->rgbMD.XRes()];
		this->zArray = new float[this->rgbMD.YRes()*this->rgbMD.XRes()];

		/* Output status information */
		this->WriteLogEntry("KINECT: Initialisation succesfull.");
	}

	void Kinect::ReInitialise()
	{
		this->WriteLogEntry("KINECT: Re-initialisation started.");

		/* Stop Generating */
		this->context.StopGeneratingAll();

		/* Release the old stuff */
		this->rgbGenerator.Release();
		this->depthGenerator.Release();
		this->depthMD.Free();
		this->rgbMD.Free();
		this->context.Shutdown();
		this->context.Release();

		/* Start reinitialisation retry until succes */
		bool isInitSucces = false;
		int numberOfTries = 0;
		while(!isInitSucces)
		{
			try
			{
				numberOfTries++;
				this->Initialise();
				isInitSucces = true;
			}
			catch(exception& ex)
			{
				if(numberOfTries > 10)
				{
					throw exception("Could not reïnitialise the kinect (failed more than 10 times).");
				}
			}
		}
	}

	/* Device functions */
	void Kinect::ReadFrame(bool isRGBRequired, bool isDepthRequired)
	{
		XnStatus rc = XN_STATUS_OK;

		/* Wait until data is updated */
		if(isRGBRequired && isDepthRequired)
		{
			/* Check wich streams are open, and correct if necessairy */
			if(!this->rgbGenerator.IsGenerating() || !this->depthGenerator.IsGenerating())
			{
				this->context.StartGeneratingAll();
			}

			rc = this->context.WaitAndUpdateAll();
			this->rgbGenerator.GetMetaData(this->rgbMD);
			this->depthGenerator.GetMetaData(this->depthMD);
		}
		else if(isRGBRequired && !isDepthRequired)
		{
			/* Check wich streams are open, and correct if necessairy */
			if(this->depthGenerator.IsGenerating())
			{
				this->depthGenerator.StopGenerating();
			}
			if(!this->rgbGenerator.IsGenerating())
			{
				this->rgbGenerator.StartGenerating();
			}

			rc = this->rgbGenerator.WaitAndUpdateData();
			this->rgbGenerator.GetMetaData(this->rgbMD);
		}
		else if(!isRGBRequired && isDepthRequired)
		{
			/* Check wich streams are open, and correct if necessairy */
			if(!this->depthGenerator.IsGenerating())
			{
				this->depthGenerator.StartGenerating();
			}
			if(this->rgbGenerator.IsGenerating())
			{
				this->rgbGenerator.StopGenerating();
			}

			rc = this->depthGenerator.WaitAndUpdateData();
			this->depthGenerator.GetMetaData(this->depthMD);
		}

		/* Throw the error if there is one */
		if(rc != XN_STATUS_OK)
		{
			throw exception(xnGetStatusString(rc));
		}
	}

	HImage Kinect::CreateHImageRGBFromBuffer()
	{
		/* Fill the buffer */
		if(isCustomBayerDecoding)
		{
			this->bayerDecoder->fillRGB(this->rgbMD,  this->rgbMD.XRes(),this->rgbMD.YRes(), this->rawBayerPtr); 
			
			/* Generate HImage */
			return this->imageRGB.GenImageInterleaved((Hlong)rawBayerPtr, "rgb", (int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), 0, "byte", (int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), 0, 0, 8, 0);
		}
		else
		{
			this->pImage = rgbMD.RGB24Data();

			/* Generate HImage */
			return this->imageRGB.GenImageInterleaved((Hlong)this->pImage, "rgb", (int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), 0, "byte", (int)this->rgbMD.XRes(), (int)this->rgbMD.YRes(), 0, 0, 8, 0);
		}
	}

	/* Image grabbers */
	HImage Kinect::GrabHImageRGBandXYZ(HImage &imageX, HImage &imageY, HImage &imageZ)
	{
		/* Get a new frame */
		this->ReadFrame(true, true);

		/* Grab new data */
		this->pDepth = depthMD.Data();

		/* Put the data in the arrays */
		for (unsigned int row = 0; row < depthMD.YRes(); ++row)
        {
            for (unsigned int col = 0; col < depthMD.XRes(); ++col, ++pDepth)
            {
                XnPoint3D point3D = Calculate3DPointWithDepthValue(*pDepth, ImagePoint(row, col));
				xArray[row * depthMD.XRes() + col] = point3D.X;
				yArray[row * depthMD.XRes() + col] = point3D.Y;
                zArray[row * depthMD.XRes() + col] = point3D.Z;
             }
		}

		/* Make the images */
        float* xPointer = &xArray[0];
		imageX = HImage::GenImage1("float", depthMD.XRes(), depthMD.YRes(), (Hlong)xPointer);
		
		float* yPointer = &yArray[0];
		imageY = HImage::GenImage1("float", depthMD.XRes(), depthMD.YRes(), (Hlong)yPointer);
		
		float* zPointer = &zArray[0];
		imageZ = HImage::GenImage1("float", depthMD.XRes(), depthMD.YRes(), (Hlong)zPointer);

		/* Create RGB image */
		return this->imageRGB = this->CreateHImageRGBFromBuffer();;
	}

	HImage Kinect::GrabHImageRGB()
	{
		this->ReadFrame(true, false);

		return this->CreateHImageRGBFromBuffer();
	}

	/* Realworld calculations */
	XnPoint3D Kinect::Calculate3DPointWithDepthValue(unsigned short depthValue, ImagePoint imagePoint)
    {
		float x = (float)((imagePoint.col / this->depthMD.XRes() - 0.5) * depthValue * this->XtoZ);
		float y = (float)((0.5 - imagePoint.row / this->depthMD.YRes()) * depthValue * this->YtoZ);
		float z = (float)(depthValue);

		XnPoint3D realWorldPoint;
		realWorldPoint.X = x;
		realWorldPoint.Y = y;
		realWorldPoint.Z = z;

		return realWorldPoint;
	}

	/* Accelerometer */
	void Kinect::GetAccelerometerGForces(double& gForceX, double& gForceY, double& gForceZ)
	{
		if(this->isAccelerometerFound)
		{
			ostringstream testString;

			while(!kinectMotor->isReady());
		
			double *gForceArray;

			try
			{
				gForceArray = kinectMotor->GetAccelerometerGForce();
				gForceX = gForceArray[0];
				gForceY = gForceArray[1];
				gForceZ = gForceArray[2];
			}
			catch(exception& ex)
			{
				throw exception(ex);
			}	
		}
		else
		{
			throw exception("The function GetAccelerometerGForces could not be executed because the accelerometer was not found!");
		}
		
	}

	/* Logger functions */
	void Kinect::WriteLogEntry(string entry)
	{
		Logger::Instance()->Append(entry);
	}

}