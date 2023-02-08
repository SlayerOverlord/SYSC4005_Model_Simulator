#pragma once
#include <stdio.h>
#include <vector>

#include "ModelEnums.h"
#include "FET.h"

#include "Agent.h"
#include "Worker.h"
#include "Inspector.h"

#define NUM_QUEUES 5
#define NUM_AGENTS 5
#define NUM_PARTS 3

struct modelParameters_st {

	// Clock
	double modelTime, deltaTime;

	// Extra
	struct event_data_st processingEvent;
	std::vector<Agent*> agents;

	// State
	std::vector<unsigned int> queues, idle;
	std::vector<struct event_data_st> processing;

	// Stats
	double busyTimes[NUM_AGENTS];
	double queueOccupancy[NUM_QUEUES];
	double partThroughput[NUM_PARTS];
	double idleProb[NUM_AGENTS];

	// Future Event Table
	FET* boba_fet;
};

class Simulator
{
public:

	Simulator();

	void launchSimulator();

	~Simulator();

private:
	int init();
	int simPass();
	Agent getAgent(Agents agent);
	void printStats();
	void setProcessingEvent(Events event_given, Agents agent_given, double time_given);

	struct modelParameters_st parameters;
};

