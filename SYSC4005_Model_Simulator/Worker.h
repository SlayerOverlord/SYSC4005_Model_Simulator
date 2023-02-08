#pragma once
#include "Agent.h"
#include "QueueSolution.h"
#include "NumberGenerator.h"

class Worker : public Agent
{
public:
	Worker(int id, Agents agent) : Agent(id, agent) { return; };

	std::vector<struct event_data_st> processEvent(struct event_data_st event_given);

	~Worker() {}

private:
	std::vector<struct event_data_st> departEvent(struct event_data_st event_given);
	std::vector<struct event_data_st> notamEvent(struct event_data_st event_given);
};

