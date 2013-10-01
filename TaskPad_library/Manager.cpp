#include<ctime>
#include "Manager.h"
#include "Storage.h"
#include "Messenger.h"
#include "Executor.h"
#include "Interpreter.h"
#include "Command.h"
#include "Task.h"
#include "Enum.h"
/*********
Assumptions made:
- Both interpreter and Executor only set the status as either SUCCESS or ERROR
**********/

using namespace TP;

const string Manager::MESSAGE_INDEX_OUT_OF_RANGE = "Given index is out of range!";

Manager::Manager()
{
	this->_storage = new Storage(_tasks);
	this->_executor = new Executor(&_tasks);
	this->_interpreter = new Interpreter;
	this->_response = Messenger();
	this->_cmd = NULL;
}

Messenger Manager::processCommand(const string& newCommand)
{
	switch(this->_response.getStatus())
	{
		case INTERMEDIATE:
		case ERROR_INTERMEDIATE:
			handleIntermediateScenarioCommands(newCommand);
			break;
		default:
			removePreviousCommand();
			handleNormalScenarioCommands(newCommand);
			break;
	}
	return this->_response;
}
/*	case SUCCESS:
	case SUCCESS_INDEXED_COMMAND:
	case ERROR:
	case DISPLAY:
	case UNDEFINED:*/

/**
 * Deletes the previously created command and,
 * returns the memory to the system
 */
void Manager::removePreviousCommand()
{
	if(this->_cmd != NULL)
	{
		delete this->_cmd;
		this->_cmd = NULL;
	}

	return;
}

/**
 * Handles the 3 types of commands that can be given in a normal scenario
 * They are:
 * - An index (for displaying the details of a task)
 * - A command with an index (non-ambiguous mod/delete commands)
 * - A generic command (an ambiguous command of any type)
 *
*/
void Manager::handleNormalScenarioCommands(string newCommand)
{
	if(isIndexGiven(newCommand))
	{
		if(isIndexWithinRange())
		{
			this->_response.setInt(this->_index);
			this->_response.setStatus(DISPLAY);
		}
		else
		{
			this->_response.setErrorMsg(MESSAGE_INDEX_OUT_OF_RANGE);
			this->_response.setStatus(ERROR);
		}
	}
	else if (isCommandWithIndexGiven(newCommand))
	{
		this->storeIndexFromCommandToIndexAttribute();
		if(this->isIndexWithinRange())
		{
			this->insertCreatedTimeIntoCommand();
			this->_executor->executeCommand(this->_cmd,this->_response);
			this->_response.setStatus(SUCCESS_INDEXED_COMMAND);
		}
	}
	else // a generic command and has already been interpreted by isCommandWithIndexGiven() above
	{
		if(this->hasNoInterpretationError())
		{
			this->_executor->executeCommand(this->_cmd,this->_response);
		}
	}
	return;
}

/**
 * This handles the scenario where an ambiguous mod/del was done and 
 * the user was given a list of similar tasks to choose the actual one he was talking about
 * Accepts:
 * - Only index
 */
void Manager::handleIntermediateScenarioCommands(string newCommand)
{
	if(isIndexGiven(newCommand))
	{
		if(isIndexWithinRange())
		{
			this->insertCreatedTimeIntoCommand();
			this->_executor->executeCommand(this->_cmd,this->_response);
		}
		else
		{
			this->_response.setErrorMsg(MESSAGE_INDEX_OUT_OF_RANGE);
			this->_response.setStatus(ERROR_INTERMEDIATE);
		}
	}
	else
	{
		this->_response.setStatus(ERROR_INTERMEDIATE);
	}
	return;
}

bool Manager::isIndexGiven(string newCommand)
{
	this->_index = this->_interpreter->interpretIndex(newCommand,this->_response);
	if(this->_response.getStatus() != ERROR)
	{
		return true;
	}
	return false;
}

bool Manager::isCommandWithIndexGiven(string newCommand)
{
	//tempStorage is used to ensure that the list is not lost in the interpreter by mistake
	list<Task> tempStorage = this->_response.getList();
	this->_cmd = this->_interpreter->interpretCommand(newCommand,this->_response);
	this->_response.setList(tempStorage);

	if (this->hasNoInterpretationError())
	{
		bool isModifyCommandWithIndex = false, isDeleteCommandWithIndex = false;
		switch (this->_cmd->getCommandType())
		{
			case MOD:
				isModifyCommandWithIndex = this->isIndexedModifyCommand();
				break;
			case DEL:
				isDeleteCommandWithIndex = this->isIndexedDeleteCommand();
				break;
			default:
				break;
		}

		if(isModifyCommandWithIndex || isDeleteCommandWithIndex)
		{
			return true;
		}
	}
	//else
	return false;
}

bool Manager::isIndexedModifyCommand()
{
	Command_Mod tempCommand = *((Command_Mod *) this->_cmd);
	return tempCommand.getFlagIndex();
}

bool Manager::isIndexedDeleteCommand()
{
	Command_Del tempCommand = *((Command_Del *) this->_cmd);
	return tempCommand.getFlagIndex();
}

bool Manager::isIndexWithinRange()
{
	int sizeOfCurrentList = this->_response.getList().size();
	return (sizeOfCurrentList > this->_index);
}

void Manager::insertCreatedTimeIntoCommand()
{
	switch(this->_cmd->getCommandType())
	{
		case MOD:
			this->insertCreatedTimeIntoModifyCommand();
			break;
		case DEL:
			this->insertCreatedTimeIntoDeleteCommand();
			break;
		default:
			throw exception("Unexpected Command with index!!");
	}
}

void Manager::insertCreatedTimeIntoDeleteCommand()
{
	Task* chosenTask = getPointerToChosenTask();
	unsigned createdTime = this->getCreatedTimeOfTask(chosenTask);

	Command_Del* tempCommand = (Command_Del *) this->_cmd;
	tempCommand->setCreatedTime(createdTime);
	return;
}

void Manager::insertCreatedTimeIntoModifyCommand()
{
	Task* chosenTask = this->getPointerToChosenTask();
	unsigned createdTime = this->getCreatedTimeOfTask(chosenTask);

	Command_Mod* tempCommand = (Command_Mod *) this->_cmd;
	tempCommand->setCreatedTime(createdTime);
	return;
}

Task* Manager::getPointerToChosenTask() const
{
	list<Task>::iterator it = this->_response.getList().begin();
	advance(it,(this->_index-1));

	return &(*it);
}

unsigned Manager::getCreatedTimeOfTask(Task* baseTask) const
{
	unsigned createdTime;
	switch(baseTask->getTaskType())
	{
		case DEADLINE:
			createdTime = getCreatedTimeOfDeadlineTask(baseTask);
			break;
		case TIMED:
			createdTime = getCreatedTimeOfTimedTask(baseTask);
			break;
		case  FLOATING:
			createdTime = getCreatedTimeOfFloatingTask(baseTask);
			break;
	}

	return createdTime;
}

unsigned Manager::getCreatedTimeOfDeadlineTask(Task* baseTask) const
{
	DeadlineTask* tempTask = (DeadlineTask *) baseTask;
	return tempTask->getIndex();
}

unsigned Manager::getCreatedTimeOfTimedTask(Task* baseTask) const
{
	TimedTask* tempTask = (TimedTask *) baseTask;
	return tempTask->getIndex();
}

unsigned Manager::getCreatedTimeOfFloatingTask(Task* baseTask) const
{
	FloatingTask* tempTask = (FloatingTask *) baseTask;
	return tempTask->getIndex();
}

void Manager::storeIndexFromCommandToIndexAttribute()
{
	throw "storeIndexFromCommandToIndexAttribute() not implemented!";
}

bool Manager::hasInterpretationError()
{
	if(this->_response.getStatus() == ERROR || this->_response.getStatus() == ERROR_INTERMEDIATE)
	{
		return true;
	}
	//else
	return false;
}

bool Manager::hasNoInterpretationError()
{
	return !this->hasInterpretationError();
}

Messenger Manager::getToday()
{
	time_t rawtime;
	struct tm * timeinfo;
	char todayCharArray [80];

	//get the time right now, convert it to a string in the format DD/MM/YYYY
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (todayCharArray,80,"'%d/%m/%Y'",timeinfo);

	//convert the char array to a string
	string today(todayCharArray);

	return this->processCommand("find due " + today + " from "+today + " to "+today);
}

void Manager::resetStatus()
{
	delete this->_cmd;
	this->_response.resetMessenger();
}

Manager::~Manager()
{
	this->_storage->save(this->_tasks);
	delete this->_interpreter;
	delete this->_executor;
	delete this->_storage;
	this->removePreviousCommand();
	this->_response.resetMessenger();
}