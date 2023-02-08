#pragma once
#include "Agent.h"
#include "QueueSolution.h"
#include "NumberGenerator.h"

class Inspector : public Agent
{
public:
	Inspector(int id, Agents agent) : Agent(id, agent) { return; }

	std::vector<struct event_data_st> processEvent(struct event_data_st event_given);

	~Inspector() {}

private:
	std::vector<struct event_data_st> departEvent();
	std::vector<struct event_data_st> notamEvent();
};