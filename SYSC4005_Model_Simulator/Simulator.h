#pragma once
#include <stdio.h>
#include <vector>

#include "ModelEnums.h"
#include "FET.h"

#include "Agent.h"
#include "Worker.h"
#include "Inspector.h"

struct modelData_st {
	std::vector<double> busyTimes;
	std::vector<double> queueOccupancy;
	std::vector<double> idleProb;
	std::vector<unsigned int> productionAmount;

	std::vector<double> inputRate;
	std::vector<double> itemsInSystem;
	std::vector<double> averageTime;

	double batch_time;
};

// Tracked Simulation parameters and statistics
struct modelParameters_st {

	// Clock
	double modelTime, deltaTime, batchTime, startupTime, batchTimer, priorBatchTimer;

	std::vector < modelData_st> data_vector;

	// Extra
	struct event_data_st			processingEvent;
	std::vector<Agent*>				agents;
	std::vector<QueueSolution*>		solutions;
	std::vector<NumberGenerator*>	generators;
	std::vector<entityData_st>		entityData;

	// State
	std::vector<unsigned int> queues, priorQueueState, idle;
	std::vector<Entities> queueMapping;
	std::vector<std::vector<double>> inputTimes;

	// Stats
	std::vector<double> busyTimes;
	std::vector<double> queueOccupancy;
	std::vector<double> idleProb;
	std::vector<unsigned int> productionAmount;

	std::vector<double> inputRate;
	std::vector<double> itemsInSystem;
	std::vector<double> averageTime;

	// Future Event Table
	FET* boba_fet;
};

class Simulator
{
public:

	Simulator();
	Simulator(double time_lim);

	void launchSimulator();
	void setBatchTime(double batchTime) { this->data.batchTime = batchTime; }
	void setSetupTime(double setupTime) { this->data.startupTime = setupTime; }
	void printStats(modelData_st data);
	void calculateBatchData();
	std::vector<modelData_st> getBatchData() { return this->data.data_vector; }

	~Simulator();

private:
	int init();
	int simPass();
	void printState();
	void clearParams();
	void updateStats();
	void setProcessingEvent(Events event_given, Agents agent_given, double time_given);
	void createComponent(Entities entity, double lambda);
	void createInspector(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution);
	void createWorker(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution);

	struct modelParameters_st data;
};

