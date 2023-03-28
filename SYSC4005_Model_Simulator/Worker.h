#pragma once
#include "Agent.h"
#include "QueueSolution.h"
#include "NumberGenerator.h"

class Worker : public Agent
{
public:
	Worker(int id, Agents agent) : Agent(id, agent) { return; };

	std::vector<struct event_data_st> processEvent(struct event_data_st event_to_process, double currentTime) override;

	~Worker() {}

private:
	std::vector<struct event_data_st> departEvent(double currentTime);
	std::vector<struct event_data_st> notamEvent(double currentTime);
};

