#include "PJLib.h"
#include "CAO_i.c"

/* GENERAL NOTES: */
/* - The L in front of a CComBSTR will use a unicode string at compile time (no L will use ANSI) */

namespace PJLib
{
	DensoRobot::DensoRobot(string robotIP, double robotSpeed)
	{
		this->isConnected = false;

		this->robotIP = robotIP;
		this->robotSpeed = robotSpeed;
	}

	void DensoRobot::Connect()
	{
		if(!isConnected)
		{
			CoInitialize(0);

			// Setup Robot 
			this->hr = S_OK;

			/* Create CaoEngine */
			hr = CoCreateInstance(CLSID_CaoEngine, NULL, CLSCTX_LOCAL_SERVER, IID_ICaoEngine, (void **)&pEngine);
			if(FAILED(hr)) throw exception("Failed to create CaoEngine!");
			cout << SCODE(hr) << endl;
			if(SCODE(hr) == 0x80000807)
			{
				cout << "Too much robot connections." << endl;
			}

			/* Retrieve CaoWorkspace collection */
			hr = pEngine->get_Workspaces(&pWss);
			if(FAILED(hr)) throw exception("Failed to retrieve CaoWorkspace collection!");

			/* Retrieve CaoWorkspace */
			hr = pWss->Item(CComVariant(0L), &pWs);
			if(FAILED(hr)) throw exception("Failed to retrieve CaoWorkspace!");

			/* Create CaoController */
			string robotIPString;
			robotIPString.append("Conn=eth:");
			robotIPString.append(this->robotIP);
			robotIPString.append(",@EVENT_DISABLE=False");
			CComBSTR ipOption = robotIPString.c_str();
			hr = pWs->AddController(CComBSTR(L"ctrl"), CComBSTR(L"CaoProv.DENSO.NetwoRC"), CComBSTR(L""), ipOption, &pController); 
			if (FAILED(hr)) throw exception("Failed to create CaoController!");

			/* Create Robot */
			hr = pController->AddRobot(CComBSTR(L"PJRobot"), CComBSTR(L""), &pRobot);
			if (FAILED(hr)) throw exception("Failed to create PJRobot!"); 

			/* Get the robot variables */
			hr = pRobot->AddVariable(CComBSTR(L"@CURRENT_POSITION"), CComBSTR(""), &currentPosition);
			if (FAILED(hr)) throw exception("Variable could not be created: 'currentPosition'."); 
			hr = pRobot->AddVariable(CComBSTR(L"@CURRENT_ANGLE"), CComBSTR(""), &currentAngle);
			if (FAILED(hr)) throw exception("Variable could not be created: 'currentAngle'.");
			hr = pRobot->AddVariable(CComBSTR(L"@SERVO_ON"), CComBSTR(""), &isServoOn);
			if (FAILED(hr)) throw exception("Variable could not be created: 'isServoOn'.");
			hr = pRobot->AddVariable(CComBSTR(L"@CURRENT_TOOL"), CComBSTR(""), &currentTool);
			if (FAILED(hr)) throw exception("Variable could not be created: 'currentTool'.");
			hr = pRobot->AddVariable(CComBSTR(L"@CURRENT_WORK"), CComBSTR(""), &currentWork);
			if (FAILED(hr)) throw exception("Variable could not be created: 'currentWork'.");
			//hr = pRobot->AddVariable(CComBSTR(L"@ERROR_DESCRIPTION"), CComBSTR(""), &error);
			//if (FAILED(hr)) throw exception("Variable could not be created: 'error'.");

			/* Set the robSlave task & get the status*/
			hr = pController->AddTask(CComBSTR(L"ROBSLAVE"), CComBSTR(L""), &robSlaveTask);
			if (FAILED(hr)) throw exception("RobSlave task could not be created.");
			hr = robSlaveTask->AddVariable(CComBSTR(L"@STATUS"), CComBSTR(L""), &robSlaveTaskStatus);
			if (FAILED(hr)) throw exception("Variable could not be created: 'robSlaveTaskStatus'.");

			/* Get the controller variables */
			hr = pController->AddVariable(CComBSTR(L"S10"), CComBSTR(L""), &pS10);
			if (FAILED(hr)) throw exception("Variable could not be created: 'pS10'.");
			hr = pController->AddVariable(CComBSTR(L"IO30"), CComBSTR(L""), &vacuumPin);
			if (FAILED(hr)) throw exception("Variable could not be created: 'vacuumPin'.");
			
			/* Set the boolean */
			this->isConnected = true;

			/* Start the robot slave */
			hr = robSlaveTask->Start(1L, CComBSTR(L""));
			if (FAILED(hr)) throw exception("RobSlave could not be started!"); 
		}

		// TO DO
		/*
		  {
            if ( ex.Message.Contains( "license" ) || ex.Message.Contains( "License" ) )
      {
       System.Diagnostics.Process[] caoProc = System.Diagnostics.Process.GetProcessesByName( "CAO" );

       if ( caoProc != null )
       {
        if ( caoProc.Length != 0 )
        {
         foreach ( System.Diagnostics.Process proc in caoProc )
         {
          proc.Kill();
         }
        }
       }
      }
     }
	 */
	}

	void DensoRobot::Disconnect()
	{
		if(this->isConnected)
		{
			/* Stop the robot */
			robSlaveTask->Stop(1L, CComBSTR(L""));

			/* turn off the motor */
			CComVariant pVal;
			pRobot->Execute(CComBSTR(L"Motor"), CComVariant(L"0"), &pVal); 

			/* Stop robSlave?? TO DO */
			hr = robSlaveTask->Stop(1L, CComBSTR(L""));
			if (FAILED(hr)) throw exception("RobSlave could not be stopped!"); 

			/* Set the connected indicator to false */
			this->isConnected = false;
		}

		if (pRobot) pRobot->Release();
		if (pS10) pS10->Release();
		if (pController) pController->Release(); 
		if (pWs) pWs->Release(); 
		if (pWss) pWss->Release(); 
		if (pEngine) pEngine->Release(); 

		CoUninitialize();
	}

	void DensoRobot::Start()
	{
		/* Start the motor */
		CComVariant pVal; // probably empty
		hr = pRobot->Execute(CComBSTR(L"Motor"), CComVariant(L"1"), &pVal);
		if(FAILED(hr)) throw exception("Failed to start the robot motor!");

		/* Set the external robot speed TO DO*/ 
		CComVariant parameters; 
        parameters.vt = VT_ARRAY | VT_VARIANT; 

        SAFEARRAYBOUND bounds[1]; 
        bounds[0].lLbound = 0; 
        bounds[0].cElements = 3; 
        parameters.parray = SafeArrayCreate(VT_VARIANT, 1, bounds); 

        CComVariant varSpeed; 
        varSpeed.vt = VT_R4; 
        varSpeed.fltVal = (float)this->robotSpeed; 

        CComVariant varAcceleration; 
        varAcceleration.vt = VT_R4; 
        varAcceleration.fltVal = (float)(this->robotSpeed*this->robotSpeed/100); 

        CComVariant varDeceleration; 
		varDeceleration.vt = VT_R4; 
        varDeceleration.fltVal = (float)(this->robotSpeed*this->robotSpeed/100); 

		long index = 0; 
        SafeArrayPutElement(parameters.parray, &index, &varSpeed); 
        index = 1; 
		SafeArrayPutElement(parameters.parray, &index, &varAcceleration); 
        index = 2; 
        SafeArrayPutElement(parameters.parray, &index, &varDeceleration); 

        CComVariant vntDummy; 
        hr = pRobot->Execute(CComBSTR(L"ExtSpeed"), parameters, &vntDummy); 
		if(FAILED(hr)) throw exception("Failed to set the external robot speed!");
        parameters.Clear(); 
	}

	void DensoRobot::Stop()
	{
	}

	void DensoRobot::MoveToLIN(RobotPoint p)
	{
		/* Create the pose in a variant format */
		string stringPose;
		stringPose.append("@E ");
		stringPose.append(p.ToString());
		CComVariant variantPose = stringPose.c_str();

		if(this->isConnected)
		{
			/* MOVE: 1 is point to point (easiest for the robot joints), 2 is linear (awesome) */
			hr = pRobot->Move(2L, variantPose, CComBSTR(L"CONT"));
			if(FAILED(hr)) throw exception("Failed to move the robot arm!");
		}
	}

	void DensoRobot::MoveToPTP(RobotPoint p)
	{
		/* Create the pose in a variant format */
		string stringPose;
		stringPose.append("@E ");
		stringPose.append(p.ToString());
		CComVariant variantPose = stringPose.c_str();

		if(this->isConnected)
		{
			/* MOVE: 1 is point to point (easiest for the robot joints), 2 is linear (awesome) */
			hr = pRobot->Move(1L, variantPose, CComBSTR(L"CONT"));
			if(FAILED(hr)) throw exception("Failed to move the robot arm!");
		}
	}

	RobotPoint DensoRobot::GetCurrentPos()
	{
		CComVariant varValue;
		long i, longlBound, longUBound;

		float *positionArray = new float[6];

		hr = this->currentPosition->get_Value(&varValue);
		if(FAILED(hr)) throw exception ("Could not get the current position!");
		
		SAFEARRAY *psa = varValue.parray;
		SafeArrayGetLBound(psa, 1, &longlBound);
		SafeArrayGetUBound(psa, 1, &longUBound);
		
		int n = 0;
		for(i = longlBound; i <= longUBound; i++)
		{
			float fltResult;
			SafeArrayGetElement(psa, &i, &fltResult);		
			positionArray[n] = fltResult; 
			n++;
		}
		
		RobotPoint robotPoint((float)positionArray[0], 
							  (float)positionArray[1],
							  (float)positionArray[2],
							  (float)positionArray[3],
							  (float)positionArray[4],
							  (float)positionArray[5],
							  (float)positionArray[6]);

		return robotPoint;
	}

	void DensoRobot::ToolOn()
	{
		hr = vacuumPin->put_Value(CComVariant(true));
		if(FAILED(hr)) throw exception("Failed to turn on the vacuüm pin!");
	}

	void DensoRobot::ToolOff()
	{
		hr = vacuumPin->put_Value(CComVariant(false)); 
		if(FAILED(hr)) throw exception("Failed to turn off the vacuüm pin!");
	}

	DensoRobot::~DensoRobot()
	{
		this->Disconnect();
	}
}
