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
	struct event_data_st			processingEvent;
	std::vector<Agent*>				agents;
	std::vector<QueueSolution*>		solutions;
	std::vector<NumberGenerator*>	generators;
	std::vector<entityData_st>		entityData;

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
	void printState();
	void printStats();
	void setProcessingEvent(Events event_given, Agents agent_given, double time_given);
	void createComponent(Entities entity, int mean, int std_dist);
	void createInspector(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution);
	void createWorker(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution);

	struct modelParameters_st parameters;
};

