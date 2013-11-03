/*
 * =====================================================================================
 *
 *       Filename:  Executor_Del.cpp
 *
 *        Version:  1.0
 *        Created:  11/02/13 13:47:00
 *
 *         Author:  ASHRAY JAIN (A0105199B), ashrayj11@gmail.com
 *   Organization:  NUS, SoC
 *
 * =====================================================================================
 */

#include "Executor_Del.h"

void Executor_Del::executeCommand(Command_Del* cmd, Messenger &response, Datastore &ds) {
	if (cmd->getFlagCreatedTime())
		deleteTaskByIndex(cmd->getCreatedTime(), response, ds);
	else
		deleteTaskByName(cmd->getName(), response, cmd->getFlagExact(), ds);
}

void Executor_Del::deleteTaskByIndex(const unsigned long long &index, Messenger &response, Datastore &ds) {
	bool indexFound = false;
	list<Task> tasks = ds.getTaskList();
	for(list<Task>::iterator i = tasks.begin(); i != tasks.end() && !indexFound; ++i)
		if (i->getIndex() == index) {
			setOpSuccessTask(*i, response);
			ds.deleteTask(distance(tasks.begin(), i));
			indexFound = true;
			break;
		}

	if (!indexFound)
		setIndexNotFound(index, response);
}

void Executor_Del::deleteTaskByName(const string &name, Messenger &response, const bool &exactFlag, Datastore &ds) {
	if (exactFlag)
		deleteByExactName(name, response, ds);
	else
		deleteByApproxName(name, response, ds);
}

void Executor_Del::deleteByExactName(const string &name, Messenger &response, Datastore &ds) {
	bool nameFound = false;
	list<Task> tasks = ds.getTaskList();
	for(list<Task>::iterator i = tasks.begin(); i != tasks.end() && !nameFound; ++i)
		if (i->getName() == name) {
			setOpSuccessTask(*i, response);
			ds.deleteTask(distance(tasks.begin(), i));			
			nameFound = true;
			break;
		}

	if (!nameFound)
		setNameNotFound(name, response);
}

void Executor_Del::deleteByApproxName(const string &name, Messenger &response, Datastore &ds) {
	list<Task> matchingResults;
	list<Task> caseInsensitiveResults;
	list<Task> tasks = ds.getTaskList();
	string lowerName = getLowerStr(name);
	for(list<Task>::iterator i = tasks.begin(); i != tasks.end(); ++i) {
		string currName = getLowerStr(i->getName());
		if (currName.find(name) != string::npos)
			matchingResults.push_back(Task(*i));
		else if (currName.find(lowerName) != string::npos)
			caseInsensitiveResults.push_back(Task(*i));
	}
	selectAppropriateDeleteResponse(matchingResults, caseInsensitiveResults, name, response, ds);
}

void Executor_Del::selectAppropriateDeleteResponse(const list<Task> &matchingResults, const list<Task> &caseInsensitiveResults, 
												   const string &name, Messenger &response, Datastore &ds) {
	if (matchingResults.size() == EMPTY_LIST_SIZE) {
		if (caseInsensitiveResults.size() == EMPTY_LIST_SIZE)
			setNameNotFound(name, response);
		else if (caseInsensitiveResults.size() == SINGLE_RESULT_LIST_SIZE)
			deleteTaskByIndex(caseInsensitiveResults.front().getIndex(), response, ds);
		else
			setOpIntermediateTaskList(caseInsensitiveResults, response);
	}
	else if (matchingResults.size() == SINGLE_RESULT_LIST_SIZE)
		deleteTaskByIndex(matchingResults.front().getIndex(), response, ds);
	else
		setOpIntermediateTaskList(matchingResults, response);
}