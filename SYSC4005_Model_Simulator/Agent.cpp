#include "Agent.h"

Agent::Agent()
{
	this->state.currentEntity = NoEntity;
	this->state.priorEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = 0;
	this->state.idle = 0;
	this->state.productionTime = 0;
	this->state.lastProductionTime = 0;

	this->entityData.clear();
	this->notams.clear();

	this->solution = nullptr;

	this->newEntityInSystem = false;
}

Agent::Agent(int id)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = id;
	this->state.idle = 0;

	this->entityData.clear();
	this->notams.clear();

	this->solution = nullptr;
}

Agent::Agent(Agents agent)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = agent;
	this->state.id = 0;
	this->state.idle = 0;

	this->entityData.clear();
	this->notams.clear();

	this->solution = nullptr;
}

Agent::Agent(int id, Agents agent)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = agent;
	this->state.id = id;
	this->state.idle = 0;

	this->entityData.clear();
	this->notams.clear();

	this->solution = nullptr;
}

void Agent::addNotams(Agents notams)
{
	event_data_st notam;

	notam.agent_given = notams;
	notam.event_given = Notam;
	notam.time = 0;

	this->notams.push_back(notam);
}

Agent::~Agent()
{
	this->state.currentEntity = NoEntity;
	this->state.priorEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = 0;
	this->state.idle = 0;
	this->state.productionTime = 0;
	this->state.lastProductionTime = 0;

	this->entityData.clear();
	this->notams.clear();

	this->entityData.~vector();
	this->notams.~vector();

	this->solution = nullptr;
	this->newEntityInSystem = false;
}

struct entityData_st Agent::genNewEntity()
{
	struct entityData_st output;
	output.entityGenerator = nullptr;
	output.entityType = NoEntity;

	if (this->entityData.size() > 0) {
		int num = rand() % this->entityData.size();
		output = this->entityData.at(num);
	}

	return output;
}
