#include "Simulator.h"

/*
* Default Constructor for the simulator
* Creates a new FET and sets default values
* for simulation parameters.
*/
Simulator::Simulator()
{
	this->data.boba_fet = new FET();

	this->data.modelTime = 0;
	this->data.deltaTime = 0;
	setProcessingEvent(NoEvent, NoAgent, 0);

	this->data.solutions.clear();
	this->data.agents.clear();
	this->data.generators.clear();
	this->data.entityData.clear();
	this->data.idle.clear();
	this->data.busyTimes.clear();
	this->data.queueOccupancy.clear();
	this->data.idleProb.clear();
	this->data.productionAmount.clear();
}

/*
* Constructor with timespan of simulation available
*/
Simulator::Simulator(double time_lim)
{
	this->data.boba_fet = new FET(time_lim);

	this->data.modelTime = 0;
	this->data.deltaTime = 0;
	setProcessingEvent(NoEvent, NoAgent, 0);

	this->data.solutions.clear();
	this->data.agents.clear();
	this->data.generators.clear();
	this->data.entityData.clear();
	this->data.idle.clear();
	this->data.busyTimes.clear();
	this->data.queueOccupancy.clear();
	this->data.idleProb.clear();
	this->data.productionAmount.clear();
}

/*
* Launch simulator perfroms a few actions:
*	Initalizes the simulation structures such as entities, agents, solutions, ect.
*	Sets Bootstrapping events to start relevant processes.
*	Loops through simulations passes until no more events are to be processes.
*	Collects final statistics and calculates final system statistics.
*	Outputs the final statistics to the terminal.
*/
void Simulator::launchSimulator()
{
	init();

	setProcessingEvent(Depart, Inspector1, 0);
	this->data.boba_fet->addEvent(this->data.processingEvent);
	setProcessingEvent(Depart, Inspector2, 0);
	this->data.boba_fet->addEvent(this->data.processingEvent);
	
	this->data.priorBatchTimer = 0;
	this->data.batchTimer = this->data.startupTime;

	while (this->simPass())
	{
		/*if (this->data.modelTime >= this->data.batchTimer &&
			this->data.modelTime - this->data.deltaTime <= this->data.batchTimer)
		{
			modelData_st newData;
			newData.batch_time = this->data.modelTime - this->data.priorBatchTimer;
			
			for (int i = 0; i < this->data.busyTimes.size(); i++)
				newData.busyTimes.push_back(this->data.busyTimes.at(i));

			for (int i = 0; i < this->data.idleProb.size(); i++)
				newData.idleProb.push_back(this->data.idleProb.at(i));

			for (int i = 0; i < this->data.inputRate.size(); i++)
				newData.inputRate.push_back(this->data.inputRate.at(i));

			for (int i = 0; i < this->data.productionAmount.size(); i++)
				newData.productionAmount.push_back(this->data.productionAmount.at(i) - 1);

			for (int i = 0; i < this->data.queueOccupancy.size(); i++)
				newData.queueOccupancy.push_back(this->data.queueOccupancy.at(i));

			for (int i = 0; i < this->data.itemsInSystem.size(); i++)
				newData.itemsInSystem.push_back(this->data.itemsInSystem.at(i));
			
			for (int i = 0; i < this->data.averageTime.size(); i++)
				newData.averageTime.push_back(this->data.averageTime.at(i));

			this->data.priorBatchTimer = this->data.batchTimer;
			this->data.batchTimer += this->data.batchTime;

			this->data.data_vector.push_back(newData);
		}*/
	}

	this->data.deltaTime = this->data.modelTime;
	this->data.modelTime = this->data.boba_fet->getFinalTime();
	this->data.deltaTime = this->data.modelTime - this->data.deltaTime;

	updateStats();

	for (int i = 0; i < this->data.productionAmount.size(); i++)
		this->data.productionAmount.at(i) --;

	for (int i = 0; i < this->data.busyTimes.size(); i++)
		this->data.busyTimes.at(i) /= this->data.modelTime;

	for (int i = 0; i < this->data.idleProb.size(); i ++)
		this->data.idleProb.at(i) /= this->data.modelTime;

	for (int i = 0; i < this->data.queueOccupancy.size(); i++)
		this->data.queueOccupancy.at(i) /= this->data.modelTime;

	for (int i = 0; i < this->data.inputRate.size(); i++)
 		this->data.inputRate.at(i) = this->data.modelTime / this->data.productionAmount.at(i);

	for (int i = 0; i < this->data.itemsInSystem.size(); i++)
		this->data.itemsInSystem.at(i) /= this->data.modelTime;

	for (int i = 0; i < this->data.queueOccupancy.size(); i ++)
		this->data.itemsInSystem.at(this->data.queueMapping.at(i)) += this->data.queueOccupancy.at(i);

	for (int i = 0; i < this->data.averageTime.size(); i++)
		this->data.averageTime.at(i) /= this->data.productionAmount.at(i);

	modelData_st data;
	data.averageTime = this->data.averageTime;
	data.batch_time = this->data.modelTime;
	data.busyTimes = this->data.busyTimes;
	data.idleProb = this->data.idleProb;
	data.inputRate = this->data.inputRate;
	data.itemsInSystem = this->data.itemsInSystem;
	data.productionAmount = this->data.productionAmount;
	data.queueOccupancy = this->data.queueOccupancy;

	printStats(data);

}

/*
* To whoever is reading this block of code, I'm sorry, I was short on time so I just
* coppied and pasted code that already worked, I could probably write this better, but
* it works enough for littles law, and the source code was built in 2 days for milestone
* 1, also I have other projects so I'm not changing it now, this block just converts the
* raw batch data into data used for the rest of the project
*/
void Simulator::calculateBatchData()
{
	for (int i = this->data.data_vector.size() - 1; i > 0 ; i--)
	{
		for (int j = 0; j < this->data.busyTimes.size(); j++)
			this->data.data_vector.at(i).busyTimes.at(j) -= this->data.data_vector.at(i - 1).busyTimes.at(j);

		for (int j = 0; j < this->data.idleProb.size(); j++)
			this->data.data_vector.at(i).idleProb.at(j) -= this->data.data_vector.at(i - 1).idleProb.at(j);

		for (int j = 0; j < this->data.inputRate.size(); j++)
			this->data.data_vector.at(i).inputRate.at(j) -= this->data.data_vector.at(i - 1).inputRate.at(j);

		for (int j = 0; j < this->data.productionAmount.size(); j++)
			this->data.data_vector.at(i).productionAmount.at(j) -= this->data.data_vector.at(i - 1).productionAmount.at(j);

		for (int j = 0; j < this->data.queueOccupancy.size(); j++)
			this->data.data_vector.at(i).queueOccupancy.at(j) -= this->data.data_vector.at(i - 1).queueOccupancy.at(j);

		for (int j = 0; j < this->data.itemsInSystem.size(); j++)
			this->data.data_vector.at(i).itemsInSystem.at(j) -= this->data.data_vector.at(i - 1).itemsInSystem.at(j);

		for (int j = 0; j < this->data.averageTime.size(); j++)
			this->data.data_vector.at(i).averageTime.at(j) -= this->data.data_vector.at(i - 1).averageTime.at(j);
	}

	for (int i = 0; i <this->data.data_vector.size(); i++)
	{

		for (int j = 0; j < this->data.busyTimes.size(); j++)
			this->data.data_vector.at(i).busyTimes.at(j) /= this->data.data_vector.at(i).batch_time;

		for (int j = 0; j < this->data.idleProb.size(); j++)
			this->data.data_vector.at(i).idleProb.at(j) /= this->data.data_vector.at(i).batch_time;

		for (int j = 0; j < this->data.inputRate.size(); j++)
			this->data.data_vector.at(i).inputRate.at(j) = this->data.data_vector.at(i).batch_time / this->data.data_vector.at(i).productionAmount.at(j);

		for (int j = 0; j < this->data.queueOccupancy.size(); j++)
			this->data.data_vector.at(i).queueOccupancy.at(j) /= this->data.data_vector.at(i).batch_time;

		for (int j = 0; j < this->data.itemsInSystem.size(); j++)
			this->data.data_vector.at(i).itemsInSystem.at(j) /= this->data.data_vector.at(i).batch_time;

		for (int j = 1; j < this->data.queueOccupancy.size(); j++)
			this->data.data_vector.at(i).itemsInSystem.at(this->data.queueMapping.at(j)) += this->data.data_vector.at(i).queueOccupancy.at(j);

		for (int j = 0; j < this->data.averageTime.size(); j++)
			this->data.data_vector.at(i).averageTime.at(j) /= this->data.data_vector.at(i).productionAmount.at(j);
	}
}

/*
* Default destructor clears all system data and deletes it
*/
Simulator::~Simulator()
{
	this->data.boba_fet->~FET();

	clearParams();

	this->data.queues.~vector();
	this->data.agents.~vector();
	this->data.solutions.~vector();
	this->data.generators.~vector();
	this->data.entityData.~vector();
	this->data.idle.~vector();
	this->data.busyTimes.~vector();
	this->data.queueOccupancy.~vector();
	this->data.idleProb.~vector();
	this->data.productionAmount.~vector();
}

/*
* Initializes the simulation environment by setting up the following:
*	System statistic parameters.
*	System state parameters.
*	Entity Data for generating random numbers.
*	Agent Data for processing events and actions.
*	Setting queue solutions for future efficiency improvments.
*/
int Simulator::init()
{
	clearParams();

	this->data.modelTime = -this->data.startupTime;

	// Seting Queues up:
	this->data.queues = { 0, 0, 0, 0, 0 };

	this->data.busyTimes.assign(this->data.queues.begin(), this->data.queues.end());
	this->data.queueOccupancy.assign(this->data.queues.begin(), this->data.queues.end());
	this->data.idleProb.assign(this->data.queues.begin(), this->data.queues.end());
	this->data.productionAmount = { 0, 0, 0, 0, 0, 0, 0 };
	this->data.inputRate.assign(this->data.productionAmount.begin(), this->data.productionAmount.end());
	this->data.itemsInSystem.assign(this->data.productionAmount.begin(), this->data.productionAmount.end());
	this->data.averageTime.assign(this->data.productionAmount.begin(), this->data.productionAmount.end());
	this->data.inputTimes = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	this->data.queueMapping = { Component1, Component1, Component1, Component2, Component3 };
	createComponent(NoEntity, 1);

	// Components
	createComponent(Component1, 0.097);
	createComponent(Component2, 0.064);
	createComponent(Component3, 0.048);

	// Part 1
	createComponent(Part1, 0.217);
	createComponent(Part2, 0.100);
	createComponent(Part3, 0.114);

	// Inspector 1:
	QueueSolution* sol_ptr = new ShortPrio();

	std::vector<entityData_st> data;
	data.clear();
	std::vector<unsigned int> entities;
	entities.clear();
	std::vector<Agents> notams;
	notams.clear();

	data.push_back(this->data.entityData.at(Component1));
	entities =	{ Component1, Component1, Component1, NoEntity, NoEntity };
	notams =	{    Worker1,    Worker2,    Worker3,  NoAgent,  NoAgent };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->data.queues));
	sol_ptr->setInputVector(&(this->data.inputTimes));

	createInspector(Inspector1, Component1, data, notams, 0, sol_ptr);
	this->data.solutions.push_back(sol_ptr);

	// Inspector 2:
	sol_ptr = new FirstQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->data.entityData.at(Component2));
	data.push_back(this->data.entityData.at(Component3));
	entities =	{ NoEntity, NoEntity, NoEntity, Component2, Component3 };
	notams =	{  NoAgent,  NoAgent,  NoAgent,    Worker2,    Worker3 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->data.queues));
	sol_ptr->setInputVector(&(this->data.inputTimes));

	createInspector(Inspector2, Component2, data, notams, 0, sol_ptr);
	this->data.solutions.push_back(sol_ptr);


	// Worker 1:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->data.entityData.at(Part1));
	entities =	{ Component1, NoEntity, NoEntity, NoEntity, NoEntity };
	notams =	{ Inspector1 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->data.queues));

	createWorker(Worker1, Part1, data, notams, 1, sol_ptr);
	this->data.solutions.push_back(sol_ptr);

	// Worker 2:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->data.entityData.at(Part2));
	entities =	{ NoEntity, Component1, NoEntity, Component2, NoEntity };
	notams =	{ Inspector1, Inspector2 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->data.queues));

	createWorker(Worker2, Part2, data, notams, 1, sol_ptr);
	this->data.solutions.push_back(sol_ptr);

	// Worker 3:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->data.entityData.at(Part3));
	entities =	{ NoEntity, NoEntity, Component1, NoEntity, Component3 };
	notams =	{ Inspector1, Inspector2 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->data.queues));

	createWorker(Worker3, Part3, data, notams, 1, sol_ptr);
	this->data.solutions.push_back(sol_ptr);

	for (int i = 0; i < this->data.generators.size(); i++)
		this->data.generators.at(i)->setSeed(this->data.seed * (i + 10352));

	return 0;
}

/*
* Sim pass does the following:
*	Gets a future event from the FET, if no further events exist, end simulation.
*	Update the model time to the time of the event and calculate the delta time.
*	Update the model statistics before modifying the simulation parameters.
*	Find the Agent relating to the current event.
*	Get the Agent to process the current event.
*	If the Agent returned any new events, calculate the time to the events and add
*		the events to the FET
*	If in debug mode, print out the state of the system.
*/
int Simulator::simPass()
{
	std::vector<event_data_st> eventData;
	eventData.clear();

	agentState_st state;
	this->data.priorQueueState = this->data.queues;

	this->data.deltaTime = this->data.modelTime;

	if (this->data.boba_fet->getNextEvent(&(this->data.processingEvent)))
		return 0;

	this->data.modelTime = this->data.processingEvent.time;
	this->data.deltaTime = this->data.modelTime - this->data.deltaTime;

	if (this->data.modelTime > 0)
		updateStats();

	for (int i = 0; i < this->data.agents.size(); i++) {
		if (this->data.agents.at(i)->getState().agentID == this->data.processingEvent.agent_given) {
			eventData = this->data.agents.at(i)->processEvent(this->data.processingEvent, this->data.modelTime);


			if (this->data.modelTime > 0)
			{
				state = this->data.agents.at(i)->getState();

				if (this->data.idle.at(i) == 1 && state.idle != this->data.idle.at(i))
				{
					//this->parameters.inputRate.at(state.priorEntity) += this->parameters.modelTime - state.lastProductionTime;
					this->data.averageTime.at(state.priorEntity) += this->data.modelTime - state.lastProductionTime;
				}

				this->data.idle.at(i) = this->data.agents.at(i)->getState().idle;
			}

			break;
		}
	}

	if (eventData.size() > 0) {
		for (int i = 0; i < eventData.size(); i++) {
			if (eventData.at(i).agent_given != NoAgent)
				eventData.at(i).time_start = this->data.modelTime;
				this->data.boba_fet->addEvent(eventData.at(i));
		}
	}

	if (this->data.modelTime > 0)
	{
		for (int i = 0; i < this->data.queues.size(); i++)
		{
			if (this->data.queues.at(i) < this->data.priorQueueState.at(i))
			{
				this->data.averageTime.at(this->data.queueMapping.at(i)) += this->data.modelTime - this->data.inputTimes.at(i).at(0);
				for (int j = 0; j < this->data.inputTimes.at(i).size() - 1; j++)
					this->data.inputTimes.at(i).at(j) = this->data.inputTimes.at(i).at(j + 1);
				this->data.inputTimes.at(i).at(this->data.inputTimes.at(i).size() - 1) = 0;
			}
		}

	}
	else
	{
		for (int i = 0; i < this->data.queues.size(); i++)
		{
			if (this->data.queues.at(i) < this->data.priorQueueState.at(i))
			{
				for (int j = 0; j < this->data.inputTimes.at(i).size() - 1; j++)
					this->data.inputTimes.at(i).at(j) = this->data.inputTimes.at(i).at(j + 1);
				this->data.inputTimes.at(i).at(this->data.inputTimes.at(i).size() - 1) = 0;
			}
		}
	}

#ifdef _DEBUG
	//printState();
#endif

	return 1;
}

/*
* Prints some statistics about the model:
*	Idle states
*	Current Event being processed
*	Queue states
*/
void Simulator::printState()
{
	printf("Sim Time: %f\n", this->data.modelTime);
	printf("Delta time: %f\n", this->data.deltaTime);
	printf("Current Event:\n");
	printf("\tAgent Value: %d\n", this->data.processingEvent.agent_given);
	printf("\tEvent Value: %d\n", this->data.processingEvent.event_given);
	printf("\tEvent Time : %f\n", this->data.processingEvent.time);
	
	printf("Idle states:\n");
	for (int i = 0; i < this->data.idle.size(); i++)
		printf("\t%d", this->data.idle.at(i));
	printf("\n");

	printf("Current Queue Outputs:\n");
	for (int i = 0; i < this->data.queues.size(); i++)
		printf("\t%d", this->data.queues.at(i));
	printf("\n");

	printf("Current Queue Wait Times\n");
	for (int i = 0; i < this->data.inputTimes.size(); i ++)
	{
		for (int j = 0; j < this->data.inputTimes.at(i).size(); j++)
			printf("\t%1.4f", this->data.inputTimes.at(i).at(j));
		printf("\n");
	}

	printf("Current Waits:\n");
	for (int i = 0; i < this->data.averageTime.size(); i++)
		printf("\t%1.4f", this->data.averageTime.at(i));
	printf("\n");

	printf("Production Amounts:\n");
	for (int i = 0; i < this->data.productionAmount.size(); i++)
		printf("\t%d", this->data.productionAmount.at(i));
	printf("\n");

	printf("Time Spent In System\n");
	for (int i = 0; i < this->data.itemsInSystem.size(); i++)
		printf("\t%1.4f", this->data.itemsInSystem.at(i));
	printf("\n");

	printf("Input Rates\n");
	for (int i = 0; i < this->data.inputRate.size(); i++)
		printf("\t%1.4f", this->data.inputRate.at(i));
	printf("\n");
	printf("\n");
}

/*
* Prints the final statistics of the model:
*	Component and Part production rates
*	Worker and Inspector Idle and Busy Probabilities
*	Average Queue Occupancies
*/
void Simulator::printStats(modelData_st data)
{
	printf("---------------<Final Statistics>---------------\n");
	printf("Total Time: %13.3fs\n\n", data.batch_time);
	printf("Production Per Minute:\n");
	printf("\tComponent1: %5.4f comp/min\n",		data.productionAmount.at(Component1) / (data.batch_time));
	printf("\tComponent2: %5.4f comp/min\n",		data.productionAmount.at(Component2) / (data.batch_time));
	printf("\tComponent3: %5.4f comp/min\n",		data.productionAmount.at(Component3) / (data.batch_time));
	printf("\tPart1-----: %5.4f part/min\n",		data.productionAmount.at(Part1)	  / (data.batch_time));
	printf("\tPart2-----: %5.4f part/min\n",		data.productionAmount.at(Part2)	  / (data.batch_time));
	printf("\tPart3-----: %5.4f part/min\n\n",	data.productionAmount.at(Part3)	  / (data.batch_time));

	printf("Idle and Busy Probability:\n");
	printf("\tInspector1) Idle: %1.4f, Busy: %1.4f\n",	data.idleProb.at(0), data.busyTimes.at(0));
	printf("\tInspector2) Idle: %1.4f, Busy: %1.4f\n",	data.idleProb.at(1), data.busyTimes.at(1));
	printf("\t   Worker1) Idle: %1.4f, Busy: %1.4f\n",	data.idleProb.at(2), data.busyTimes.at(2));
	printf("\t   Worker2) Idle: %1.4f, Busy: %1.4f\n",	data.idleProb.at(3), data.busyTimes.at(3));
	printf("\t   Worker3) Idle: %1.4f, Busy: %1.4f\n\n",	data.idleProb.at(4), data.busyTimes.at(4));

	printf("Average Queue Occupancies:\n");
	printf("\tQueue C1-1: %1.4f Components\n",	data.queueOccupancy.at(0));
	printf("\tQueue C1-2: %1.4f Components\n",	data.queueOccupancy.at(1));
	printf("\tQueue C1-3: %1.4f Components\n",	data.queueOccupancy.at(2));
	printf("\tQueue C2-1: %1.4f Components\n",	data.queueOccupancy.at(3));
	printf("\tQueue C3-1: %1.4f Components\n\n",	data.queueOccupancy.at(4));

	printf("---------------<Extra Parameters>---------------\n");
	printf("Input Rates:\n");
	printf("\tC1: %1.4f min/Component\n",		data.inputRate.at(Component1));
	printf("\tC2: %1.4f min/Component\n",		data.inputRate.at(Component2));
	printf("\tC3: %1.4f min/Component\n",		data.inputRate.at(Component3));
	printf("\tP1: %1.4f min/Part\n",			data.inputRate.at(Part1));
	printf("\tP2: %1.4f min/Part\n",			data.inputRate.at(Part2));
	printf("\tP3: %1.4f min/Part\n\n",		data.inputRate.at(Part3));

	printf("Items In System:\n");
	printf("\tC1: %1.4f Average Components\n",	data.itemsInSystem.at(Component1));
	printf("\tC2: %1.4f Average Components\n",	data.itemsInSystem.at(Component2));
	printf("\tC3: %1.4f Average Components\n",	data.itemsInSystem.at(Component3));
	printf("\tP1: %1.4f Average Parts\n",			data.itemsInSystem.at(Part1));
	printf("\tP2: %1.4f Average Parts\n",			data.itemsInSystem.at(Part2));
	printf("\tP3: %1.4f Average Parts\n\n",		data.itemsInSystem.at(Part3));

	printf("Average Time Spent In System\n");
	printf("\tC1: %3.4f Average Time in Minutes\n",		data.averageTime.at(Component1));
	printf("\tC2: %3.4f Average Time in Minutes\n",		data.averageTime.at(Component2));
	printf("\tC3: %3.4f Average Time in Minutes\n",		data.averageTime.at(Component3));
	printf("\tP1: %3.4f Average Time in Minutes\n",		data.averageTime.at(Part1));
	printf("\tP2: %3.4f Average Time in Minutes\n",		data.averageTime.at(Part2));
	printf("\tP3: %3.4f Average Time in Minutes\n\n",		data.averageTime.at(Part3));

	printf("------------------------------------------------\n");
	printf("Littles Law Calculation:\n");
	printf("\tComponent 1: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Component1),
		data.averageTime.at(Component1) / data.inputRate.at(Component1));

	printf("\tComponent 2: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Component2),
		data.averageTime.at(Component2) / data.inputRate.at(Component2));

	printf("\tComponent 3: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Component3),
		data.averageTime.at(Component3) / data.inputRate.at(Component3));

	printf("\tPart 1: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Part1),
		data.averageTime.at(Part1) / data.inputRate.at(Part1));

	printf("\tPart 2: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Part2),
		data.averageTime.at(Part2) / data.inputRate.at(Part2));

	printf("\tPart 3: L = %5.4f, lambda * w = %5.4f\n", data.itemsInSystem.at(Part3),
		data.averageTime.at(Part3) / data.inputRate.at(Part3));

	printf("\n");



	//std::vector<double> inputRate;
//std::vector<double> itemsInSystem;
//std::vector<double> averageTime;
}

/*
* Sets system parameters to defaults
*/
void Simulator::clearParams()
{
	this->data.modelTime = 0;
	this->data.deltaTime = 0;

	for (int i = 0; i < this->data.agents.size(); i++)
		this->data.agents.at(i)->~Agent();
	this->data.agents.clear();

	for (int i = 0; i < this->data.solutions.size(); i++)
		this->data.solutions.at(i)->~QueueSolution();
	this->data.solutions.clear();

	for (int i = 0; i < this->data.generators.size(); i++)
		this->data.generators.at(i)->~NumberGenerator();
	this->data.generators.clear();

	this->data.queues.clear();
	this->data.entityData.clear();
	this->data.idle.clear();
	this->data.busyTimes.clear();
	this->data.queueOccupancy.clear();
	this->data.idleProb.clear();
	this->data.productionAmount.clear();
}

/*
* Code to set the current event to process
*/
void Simulator::setProcessingEvent(Events event_given, Agents agent_given, double time_given)
{
	this->data.processingEvent.agent_given = agent_given;
	this->data.processingEvent.event_given = event_given;
	this->data.processingEvent.time		 = time_given;
}

/*
* Code to create a component with probability specifications
*/
void Simulator::createComponent(Entities entity, double lambda)
{
	ExponentialDist* gen_ptr = new ExponentialDist(lambda);
	struct entityData_st ent_data;
	ent_data.entityGenerator = gen_ptr;
	ent_data.entityType = entity;
	this->data.generators.push_back(gen_ptr);
	this->data.entityData.push_back(ent_data);
}

/*
* Creates an inspector with the designated parameters:
*	agent:		What agent is this Inspector?
*	starting:	What entity is the agent first using?
*	data:		Properties of each component/part the agent works on (processing times and types)
*	notams:		Who to notify when placing an entity into a queue
*	idle_init:	Does the agent start idle or busy?
*	solution:	How does the Agent find what queue to place the entity into?
*/
void Simulator::createInspector(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution)
{
	Agent* agent_ptr = new Inspector(agent, agent);

	for (int i = 0; i < data.size(); i ++)
		agent_ptr->addToEntityList(data.at(i));

	for (int i = 0; i < notams.size(); i ++)
		agent_ptr->addNotams(notams.at(i));

	agent_ptr->setQueueSolution(solution);
	agent_ptr->setCurrentEntity(starting);
	agent_ptr->setAgentIdle(idle_init);
	agent_ptr->setProductionTarget(&(this->data.productionAmount));

	this->data.agents.push_back(agent_ptr);
	this->data.idle.push_back(agent_ptr->getState().idle);
}

/*
* Creates a worker with the designated parameters:
*	agent:		What agent is this Inspector?
*	starting:	What entity is the agent first using?
*	data:		Properties of each component/part the agent works on (processing times and types)
*	notams:		Who to notify when placing an entity into a queue
*	idle_init:	Does the agent start idle or busy?
*	solution:	How does the Agent find what queue to place the entity into?
*/
void Simulator::createWorker(Agents agent, Entities starting, std::vector<entityData_st> data, std::vector<Agents> notams, int idle_init, QueueSolution* solution)
{
	Agent* agent_ptr = new Worker(agent, agent);

	for (int i = 0; i < data.size(); i++)
		agent_ptr->addToEntityList(data.at(i));

	for (int i = 0; i < notams.size(); i++)
		agent_ptr->addNotams(notams.at(i));

	agent_ptr->setQueueSolution(solution);
	agent_ptr->setCurrentEntity(starting);
	agent_ptr->setAgentIdle(idle_init);
	agent_ptr->setProductionTarget(&(this->data.productionAmount));

	this->data.agents.push_back(agent_ptr);
	this->data.idle.push_back(agent_ptr->getState().idle);
}

//	std::vector<double> inputRate;
//	std::vector<double> itemsInSystem;
//	std::vector<double> averageTime;

/*
* Updates the tracked statistics that aren't automatically updated
*/
void Simulator::updateStats()
{
	Entities component = NoEntity;

	for (int i = 0; i < this->data.agents.size(); i++) {
		this->data.idleProb.at(i) += this->data.agents.at(i)->getState().idle * this->data.deltaTime;
		this->data.busyTimes.at(i) += !this->data.agents.at(i)->getState().idle * this->data.deltaTime;


		if (this->data.agents.at(i)->newEntityProduced())
		{
			component = this->data.agents.at(i)->getState().currentEntity;
			this->data.inputRate.at(component) += this->data.agents.at(i)->getState().productionTime;

			this->data.averageTime.at(component) += this->data.agents.at(i)->getState().productionTime;

			this->data.itemsInSystem.at(component) += this->data.agents.at(i)->getState().productionTime;

			component = this->data.agents.at(i)->getState().priorEntity;
			this->data.itemsInSystem.at(component) += this->data.modelTime - this->data.agents.at(i)->getState().lastProductionTime;

		}
	}

	for (int i = 0; i < this->data.queues.size(); i++)
	{
		this->data.queueOccupancy.at(i) += this->data.queues.at(i) * this->data.deltaTime;
	}
}