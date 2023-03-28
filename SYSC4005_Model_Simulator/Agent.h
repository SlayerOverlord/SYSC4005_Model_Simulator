#pragma once
#include "EventProcessor.h"
#include "QueueSolution.h"
#include "NumberGenerator.h"

struct agentState_st {
	Entities currentEntity;
	Entities priorEntity;
	Agents agentID;
	unsigned int id;
	int idle;
	double productionTime;
	double lastProductionTime;
};

struct entityData_st {
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

	virtual std::vector<struct event_data_st> processEvent(struct event_data_st event_to_process, double currentTime) = 0;

	struct agentState_st getState() { return this->state; }
	void addNotams(Agents notams);
	void setQueueSolution(QueueSolution* solution) { this->solution = solution; }
	void addToEntityList(struct entityData_st data) { this->entityData.push_back(data); }
	void setCurrentEntity(Entities currentEntity) { this->state.currentEntity = currentEntity; }
	void setAgentIdle(int idle_state) { this->state.idle = idle_state; }
	void setProductionTarget(std::vector<unsigned int>* production) { this->produced = production; }
	bool newEntityProduced() { if (newEntityInSystem) { newEntityInSystem = false; return true; } else { return false; } };
	void setLastProductionTime(double time) { this->state.lastProductionTime = time; }

	~Agent();

protected:
	struct entityData_st genNewEntity();

	bool newEntityInSystem;
	QueueSolution* solution;
	NumberGenerator* generator;
	struct agentState_st state;
	std::vector<struct entityData_st> entityData;
	std::vector<struct event_data_st> notams;
	std::vector<unsigned int>* produced;
};

