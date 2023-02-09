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
	NumberGenerator *entityGenerator;
};

class Agent : public EventProcessor
{
public:
	Agent();
	Agent(int id);
	Agent(Agents agent);
	Agent(int id, Agents agent);

	virtual std::vector<struct event_data_st> processEvent(struct event_data_st event_to_process) = 0;
	struct agentState_st getState() { return this->state; }
	void addNotams(Agents notams);
	void setQueueSolution(QueueSolution* solution) { this->solution = solution; }
	void addToEntityList(struct entityData_st data) { this->entityData.push_back(data); }
	void setCurrentEntity(Entities currentEntity) { this->state.currentEntity = currentEntity; }

	~Agent();

protected:
	struct entityData_st genNewEntity();

	QueueSolution* solution;
	NumberGenerator* generator;
	struct agentState_st state;
	std::vector<struct entityData_st> entityData;
	std::vector<struct event_data_st> notams;
};

