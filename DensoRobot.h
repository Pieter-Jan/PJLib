#ifndef INC_DENSOROBOT_H
#define INC_DENSOROBOT_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

/* Denso Robot (ORiN2) header */
#include "CAO.h"
#include "atlbase.h"
#include "comutil.h"

/* RobotPoint */
#include "RobotPoint.h"

#include <iostream>

using namespace std;

namespace PJLib
{
	class DensoRobot
	{
		private:
			// Variables 
			HRESULT hr;
			ICaoEngine *pEngine;
			ICaoWorkspaces *pWss;
			ICaoWorkspace *pWs;
			ICaoController *pController;
			CComVariant vntVal;
			ICaoRobot *pRobot;

			ICaoVariable *currentPosition;
			ICaoVariable *currentAngle;
			ICaoVariable *isServoOn;
			ICaoVariable *currentTool;
			ICaoVariable *currentWork;
			ICaoVariable *error;
			ICaoVariable *robSlaveTaskStatus;
			ICaoVariable *vacuumPin;
			ICaoVariable *pS10;

			ICaoTask *robSlaveTask;

		public:
			double robotSpeed;
			string robotIP;
			bool isConnected;
			CComVariant *currentPosVariable;

			DensoRobot(string robotIP, double robotSpeed);
			void Connect();
			void Disconnect();
			void Start();
			void Stop();
			void MoveToLIN(RobotPoint p);
			void MoveToPTP(RobotPoint p);
			void MoveToPTPJoint(RobotPoint p);
			RobotPoint GetCurrentPos();
			void ToolOn();
			void ToolOff();
			~DensoRobot();
	};
}

#endif