#ifndef _STORAGE_H_
#define _STORAGE_H_

#include<fstream>
#include<list>
#include "Enum.h"

//forward definitions
class Task;
class Command;
class Logger;
class TaskLoader;

class Storage
{
	private:
		std::ofstream _fileWriter;
		Logger* _logger;
		TaskLoader* _loader;

		static const std::string _fileName;

		static const std::string LABEL_NAME;
		static const std::string LABEL_INDEX;
		static const std::string LABEL_DUE_DATE;
		static const std::string LABEL_FROM_DATE;
		static const std::string LABEL_TO_DATE;
		static const std::string LABEL_LOCATION;
		static const std::string LABEL_PARTICIPANT;
		static const std::string LABEL_NOTE;
		static const std::string LABEL_PRIORITY;
		static const std::string LABEL_TAG;
		static const std::string LABEL_REMINDER_TIME;
		static const std::string LABEL_STATE;
		static const std::string LABEL_END_OF_TASK;
		static const std::string LABEL_START_OF_TASK;

		////////////////////////
		////Saving Functions///
		//////////////////////

		//savers
		void saveTaskList		(const std::list<Task>& taskList);
		void saveTask			(const Task& task);
		void saveTaskAttributes	(const Task& tempTaskTask);

		//file level attributes
		void saveTaskLevelLabel			(std::string LabelStr);
		void saveAttributeLevelLabel	(std::string LabelStr);

		//saving generic attributes of all tasks
		void saveIndex				(const Task& tempTask);
		void saveName				(const Task& tempTask);
		void saveLocation			(const Task& tempTask);
		void saveParticipants		(const Task& tempTask);
		void saveNote				(const Task& tempTask);
		void savePriority			(const Task& tempTask);
		void saveTags				(const Task& tempTask);
		void saveReminderTimes		(const Task& tempTask);
		void saveState				(const Task& tempTask);
		void saveDueDate			(const Task& tempTask);
		void saveFromDate			(const Task& tempTask);
		void saveToDate				(const Task& tempTask);

		static std::string convertTimeToString(time_t time);

		// std::string converters
		std::string convertToString	(int num);
		std::string convertToString	(unsigned long long index);
		std::string convertToString	(time_t time);
		std::string convertToString	(TP::PRIORITY priority);
		std::string convertToString	(TP::TASK_STATE state);

		//writers
		void writeLineToFile	(std::string line, bool newLine = true);
		void emptyTheFile		();

		//file opener/closer
		void openTheFileToWrite		(std::string fileName = _fileName, std::ios_base::openmode = std::ios::trunc);
		void closeTheWrittenFile	();
		
		//removes Task files
		void removeTaskFiles();

	public:
		Storage(std::list<Task>&);

		bool save	(const std::list<Task>&);
		bool save	(const Task& task);

		void load	(std::list<Task>& taskList);

		const std::string FILENAME;

		~Storage();
};
#endif