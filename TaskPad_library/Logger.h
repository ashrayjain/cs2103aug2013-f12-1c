#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include "Enum.h"

//a singleton for the whole program
class Logger
{
	public:
		void setMinLevelToLog	(TP::LOG_TYPE);

		static	Logger*	getLogger	();
				void	log			(std::string callingClass, std::string message, TP::LOG_TYPE logType = TP::INFOLOG);
	private:
		std::ofstream _logWriter;
		static const std::string _configFileName;
		static const std::string configLabelList[];
		static		 std::string configDetailList[];
		static const std::string configDefaultValues[];
		static const std::string DEFAULT_CONFIG_FILE;
		static const std::string DEFAULT_CLASS_TO_MONITOR;
		static const std::string DEFAULT_LEVEL_TO_LOG;

		static const int CONFIG_DETAILS_SIZE;
		static const int FILENAME;
		static const int CLASS_TO_MONITOR;
		static const int MIN_LEVEL_TO_LOG;

		TP::LOG_TYPE _minLevelToLog;

		//the single object
		static Logger* logger;
		
		//log functions
		void logInfo(std::string message);
		void logNotice(std::string message);
		void logWarning(std::string message);
		void logError(std::string message);
		void logFatal(std::string message);

		//helper functions
		void configureLogger			();
		void loadConfigData				();
		void setDefaultValues			();
		void extractMinLevelToMonitor	();
		std::string getNewData			(std::string newLine);
		std::string getNewLabel			(std::string newLine);

		bool isToBeLogged(std::string callingClass, TP::LOG_TYPE logType);

		std::string getCurTime			();


		//constructor
		Logger();
		Logger(Logger const&);

		//equals operator overload
		void operator=(Logger const&);

		//destructor
		~Logger();
};

#endif