#include "PJLib.h"

namespace PJLib
{
	/* Singleton Implementation */
	Logger* Logger::_instance = 0;

	Logger* Logger::Instance() 
	{
		if (_instance == 0) 
		{
			_instance = new Logger();
		}

		return _instance;
	}

	Logger::Logger()
	{
	}

	void Logger::SetFilePath(string filePath)
	{
		this->filePath = filePath;
		this->logFileStream.open(filePath + "\\log.txt");
	
		/* Open filestream */
		if(logFileStream.is_open())
		{
			this->logFileStream << "<pre>//------------------------- Log -------------------------//" << endl;
		}
		else
		{
			try
			{
				boost::filesystem::create_directory(filePath);
			}
			catch(...)
			{
				throw exception("Could not create directory for the log file!");
			}
		}
	}

	void Logger::Append(string logMessage)
	{
		if(this->filePath.empty())
		{
			cout << logMessage << endl;
		}
		else
		{
			time(&this->rawTime);
			string timeString = ctime(&this->rawTime);
			this->logFileStream << timeString.substr(0,timeString.length()-1) << " - " << logMessage << endl;

			/* Emit a signal so that observers know an entry has been added to the log */
			this->entryAddedSignal();
		}
	}
}