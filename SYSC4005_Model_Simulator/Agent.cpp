#include "Agent.h"

Agent::Agent()
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = 0;
	this->state.idle = 0;
}

Agent::Agent(int id)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = id;
	this->state.idle = 0;
}

Agent::Agent(Agents agent)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = agent;
	this->state.id = 0;
	this->state.idle = 0;
}

Agent::Agent(int id, Agents agent)
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = agent;
	this->state.id = id;
	this->state.idle = 0;
}

Agent::~Agent()
{
	this->state.currentEntity = NoEntity;
	this->state.agentID = NoAgent;
	this->state.id = 0;
	this->state.idle = 0;

	if (this->solution != nullptr)
		this->solution->~QueueSolution();
	if (this->generator != nullptr)
		this->generator->~NumberGenerator();

	this->solution = nullptr;
	this->generator = nullptr;
	this->currentNotams.~vector();
}
