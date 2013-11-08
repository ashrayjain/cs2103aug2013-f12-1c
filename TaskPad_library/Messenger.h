/*
 * =====================================================================================
 *
 *		Filename:  Messenger.h
 *
 *      Version:  1.0
 *
 *      Author:  Thyaegsh Manikandan (A0100124J)
 *		Organization:  NUS, SoC
 *
 * =====================================================================================

 ** Gist of File Contents:
  *
  * This file defines the Messenger Class, its functions and attributes
  * A messenger object is used as a way to return and exchange different values from
  * functions. This way a more sophisticated value can be returned allowing for 
  * greater information transfer from the function to its caller.
  *
 */

#ifndef _MESSENGER_H_
#define _MESSENGER_H_

#include<string>
#include<list>
#include "task.h"
#include "Enum.h"

using namespace std;
using namespace TP;

class Messenger
{
	public:

		// constructor
		Messenger(COMMAND_TYPE commandType=UNDEFINED, STATUS status=SUCCESS, list<Task> resultList= list<Task>(), Task resultTask=Task(), int index =-1, string errorMsg="");

		//getter methods
		
		string			getErrorMsg()		const;
		STATUS			getStatus()			const;
		list<Task>		getList()			const;
		int				getIndex()			const;
		COMMAND_TYPE	getCommandType()	const;
		Task			getTask()			const;

		// state resetter
		void resetMessenger();

		//setter functions
		void setErrorMsg	(const string&			errorMsg);
		void setStatus		(const STATUS&			status);
		void setList		(const list<Task>&		result);
		void setInt			(const int&				index);
		void setCommandType	(const COMMAND_TYPE&	commandType);
		void setTask		(const Task&			task);

		const static int DEFAULT_INDEX_VALUE;
		const static TP::STATUS DEFAULT_STATUS_VALUE;
		const static TP::COMMAND_TYPE DEFAULT_COMMAND_TYPE_VALUE;
		const static Task DEFAULT_TASK_VALUE;
		const static list<Task> DEFAULT_TASK_LIST_VALUE;
		const static std::string DEFAULT_ERROR_MESSAGE_VALUE;

	private:
		string			_errorMsg;
		STATUS			jvmCreated;
		list<Task>		_resultList;
		Task			_resultTask;
		int				_index;
		COMMAND_TYPE	_commandType;
};

#endif