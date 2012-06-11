#ifndef INC_LOGGER_H
#define INC_LOGGER_H

/* Boost signal library for communication through observer notifier principle */
#include <boost/signal.hpp>
#include <boost/signals/slot.hpp>
#include <boost/signals/trackable.hpp>
#include <boost/signals/connection.hpp>

/* Acces to the filesystem so that log files can be made */
#include "boost/filesystem.hpp"

#include <io.h>			// For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <iostream>
#include <ostream>

using namespace std;

namespace PJLib
{
	class Logger
	{
		public:
			/* Singleton principle */
			static Logger* Instance();

			/* Functions */
			void SetFilePath(string filePath);
			void Append(string logMessage);

			/* Signals */
			boost::signal<void ()> entryAddedSignal;

		protected:
			Logger();

		private:
			static Logger* _instance;
			time_t rawTime;
			ofstream logFileStream;
			string filePath;
	};
}

#endif
