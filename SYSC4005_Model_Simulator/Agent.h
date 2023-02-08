#pragma once
#include "EventProcessor.h"
#include "QueueSolution.h"
#include "NumberGenerator.h"

struct agentState_st {
	Entities currentEntity;
	Agents agentID;
	unsigned int id;
	int idle;
};

struct entityData_st {
	unsigned int entityID;
	Entities entityType;
	NumberGenerator entityGenerator;
};

class Agent : public EventProcessor
{
public:
	Agent();
	Agent(int id);
	Agent(Agents agent);
	Agent(int id, Agents agent);

	virtual std::vector<struct event_data_st> processEvent(Events event_to_process) = 0;
	struct agentState_st getState() { return this->state; }
	void setNotams(std::vector<struct event_data_st> notams) { this->currentNotams = notams; }
	void setQueueSolution(QueueSolution* solution) { this->solution = solution; }
	void setNumberGenerator(NumberGenerator* solution) { this->solution = solution; }
	void setEntityList(std::vector<entityData_st> entityList) { this->entityData = entityList; }

	~Agent();

protected:
	QueueSolution* solution;
	NumberGenerator* generator;
	struct agentState_st state;
	std::vector<struct entityData_st> entityData;
	std::vector<struct event_data_st> currentNotams;
};

