#include "Simulator.h"

/*
* Default Constructor for the simulator
* Creates a new FET and sets default values
* for simulation parameters.
*/
Simulator::Simulator()
{
	this->parameters.boba_fet = new FET();

	this->parameters.modelTime = 0;
	this->parameters.deltaTime = 0;
	setProcessingEvent(NoEvent, NoAgent, 0);

	this->parameters.solutions.clear();
	this->parameters.agents.clear();
	this->parameters.generators.clear();
	this->parameters.entityData.clear();
	this->parameters.idle.clear();
	this->parameters.busyTimes.clear();
	this->parameters.queueOccupancy.clear();
	this->parameters.idleProb.clear();
	this->parameters.productionAmount.clear();
}

/*
* Constructor with timespan of simulation available
*/
Simulator::Simulator(double time_lim)
{
	this->parameters.boba_fet = new FET(time_lim);

	this->parameters.modelTime = 0;
	this->parameters.deltaTime = 0;
	setProcessingEvent(NoEvent, NoAgent, 0);

	this->parameters.solutions.clear();
	this->parameters.agents.clear();
	this->parameters.generators.clear();
	this->parameters.entityData.clear();
	this->parameters.idle.clear();
	this->parameters.busyTimes.clear();
	this->parameters.queueOccupancy.clear();
	this->parameters.idleProb.clear();
	this->parameters.productionAmount.clear();
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
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	setProcessingEvent(Depart, Inspector2, 0);
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	
	while (this->simPass());

	this->parameters.deltaTime = this->parameters.modelTime;
	this->parameters.modelTime = this->parameters.boba_fet->getFinalTime();
	this->parameters.deltaTime = this->parameters.modelTime - this->parameters.deltaTime;

	updateStats();

	for (int i = 0; i < this->parameters.productionAmount.size(); i++)
		this->parameters.productionAmount.at(i)--;

	for (int i = 0; i < this->parameters.busyTimes.size(); i++)
		this->parameters.busyTimes.at(i) /= this->parameters.modelTime;

	for (int i = 0; i < this->parameters.idleProb.size(); i ++)
		this->parameters.idleProb.at(i) /= this->parameters.modelTime;

	for (int i = 0; i < this->parameters.queueOccupancy.size(); i++)
		this->parameters.queueOccupancy.at(i) /= this->parameters.modelTime;

	for (int i = 0; i < this->parameters.inputRate.size(); i++)
 		this->parameters.inputRate.at(i) = this->parameters.modelTime / this->parameters.productionAmount.at(i);

	for (int i = 0; i < this->parameters.itemsInSystem.size(); i++)
		this->parameters.itemsInSystem.at(i) /= this->parameters.modelTime;

	for (int i = 0; i < this->parameters.queueOccupancy.size(); i ++)
		this->parameters.itemsInSystem.at(this->parameters.queueMapping.at(i)) += this->parameters.queueOccupancy.at(i);


	for (int i = 0; i < this->parameters.averageTime.size(); i++)
		this->parameters.averageTime.at(i) /= this->parameters.productionAmount.at(i);

	printStats();

}

/*
* Default destructor clears all system data and deletes it
*/
Simulator::~Simulator()
{
	this->parameters.boba_fet->~FET();

	clearParams();

	this->parameters.queues.~vector();
	this->parameters.agents.~vector();
	this->parameters.solutions.~vector();
	this->parameters.generators.~vector();
	this->parameters.entityData.~vector();
	this->parameters.idle.~vector();
	this->parameters.busyTimes.~vector();
	this->parameters.queueOccupancy.~vector();
	this->parameters.idleProb.~vector();
	this->parameters.productionAmount.~vector();
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

	// Seting Queues up:
	this->parameters.queues = { 0, 0, 0, 0, 0 };

	this->parameters.busyTimes.assign(this->parameters.queues.begin(), this->parameters.queues.end());
	this->parameters.queueOccupancy.assign(this->parameters.queues.begin(), this->parameters.queues.end());
	this->parameters.idleProb.assign(this->parameters.queues.begin(), this->parameters.queues.end());
	this->parameters.productionAmount = { 0, 0, 0, 0, 0, 0, 0 };
	this->parameters.inputRate.assign(this->parameters.productionAmount.begin(), this->parameters.productionAmount.end());
	this->parameters.itemsInSystem.assign(this->parameters.productionAmount.begin(), this->parameters.productionAmount.end());
	this->parameters.averageTime.assign(this->parameters.productionAmount.begin(), this->parameters.productionAmount.end());
	this->parameters.inputTimes = { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	this->parameters.queueMapping = { Component1, Component1, Component1, Component2, Component3 };
	createComponent(NoEntity, 1, 0);

	// Components
	createComponent(Component1, 5, 1);
	createComponent(Component2, 5, 1);
	createComponent(Component3, 5, 1);

	// Part 1
	createComponent(Part1, 7, 5);
	createComponent(Part2, 7, 5);
	createComponent(Part3, 7, 5);

	// Inspector 1:
	QueueSolution* sol_ptr = new ShortPrio();

	std::vector<entityData_st> data;
	data.clear();
	std::vector<unsigned int> entities;
	entities.clear();
	std::vector<Agents> notams;
	notams.clear();

	data.push_back(this->parameters.entityData.at(Component1));
	entities =	{ Component1, Component1, Component1, NoEntity, NoEntity };
	notams =	{    Worker1,    Worker2,    Worker3,  NoAgent,  NoAgent };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));
	sol_ptr->setInputVector(&(this->parameters.inputTimes));

	createInspector(Inspector1, Component1, data, notams, 0, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

	// Inspector 2:
	sol_ptr = new FirstQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Component2));
	data.push_back(this->parameters.entityData.at(Component3));
	entities =	{ NoEntity, NoEntity, NoEntity, Component2, Component3 };
	notams =	{  NoAgent,  NoAgent,  NoAgent,    Worker2,    Worker3 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));
	sol_ptr->setInputVector(&(this->parameters.inputTimes));

	createInspector(Inspector2, Component2, data, notams, 0, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);


	// Worker 1:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Part1));
	entities =	{ Component1, NoEntity, NoEntity, NoEntity, NoEntity };
	notams =	{ Inspector1 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createWorker(Worker1, Part1, data, notams, 1, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

	// Worker 2:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Part2));
	entities =	{ NoEntity, Component1, NoEntity, Component2, NoEntity };
	notams =	{ Inspector1, Inspector2 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createWorker(Worker2, Part2, data, notams, 1, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

	// Worker 3:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Part3));
	entities =	{ NoEntity, NoEntity, Component1, NoEntity, Component3 };
	notams =	{ Inspector1, Inspector2 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createWorker(Worker3, Part3, data, notams, 1, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

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
	this->parameters.priorQueueState = this->parameters.queues;

	this->parameters.deltaTime = this->parameters.modelTime;

	if (this->parameters.boba_fet->getNextEvent(&(this->parameters.processingEvent)))
		return 0;

	this->parameters.modelTime = this->parameters.processingEvent.time;
	this->parameters.deltaTime = this->parameters.modelTime - this->parameters.deltaTime;
	updateStats();

	for (int i = 0; i < this->parameters.agents.size(); i++) {
		if (this->parameters.agents.at(i)->getState().agentID == this->parameters.processingEvent.agent_given) {
			eventData = this->parameters.agents.at(i)->processEvent(this->parameters.processingEvent, this->parameters.modelTime);
			state = this->parameters.agents.at(i)->getState();

			if (this->parameters.idle.at(i) == 1 && state.idle != this->parameters.idle.at(i))
			{
				//this->parameters.inputRate.at(state.priorEntity) += this->parameters.modelTime - state.lastProductionTime;
				this->parameters.averageTime.at(state.priorEntity) += this->parameters.modelTime - state.lastProductionTime;
			}

			this->parameters.idle.at(i) = this->parameters.agents.at(i)->getState().idle;

			break;
		}
	}

	if (eventData.size() > 0) {
		for (int i = 0; i < eventData.size(); i++) {
			if (eventData.at(i).agent_given != NoAgent)
				eventData.at(i).time_start = this->parameters.modelTime;
				this->parameters.boba_fet->addEvent(eventData.at(i));
		}
	}

	for (int i = 0; i < this->parameters.queues.size(); i++)
	{
		if (this->parameters.queues.at(i) < this->parameters.priorQueueState.at(i))
		{
			this->parameters.averageTime.at(this->parameters.queueMapping.at(i)) += this->parameters.modelTime - this->parameters.inputTimes.at(i).at(0);
			for (int j = 0; j < this->parameters.inputTimes.at(i).size() - 1; j++)
				this->parameters.inputTimes.at(i).at(j) = this->parameters.inputTimes.at(i).at(j + 1);
			this->parameters.inputTimes.at(i).at(this->parameters.inputTimes.at(i).size() - 1) = 0;
		}
	}

#ifdef _DEBUG
	printState();
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
	printf("Sim Time: %f\n", this->parameters.modelTime);
	printf("Delta time: %f\n", this->parameters.deltaTime);
	printf("Current Event:\n");
	printf("\tAgent Value: %d\n", this->parameters.processingEvent.agent_given);
	printf("\tEvent Value: %d\n", this->parameters.processingEvent.event_given);
	printf("\tEvent Time : %f\n", this->parameters.processingEvent.time);
	
	printf("Idle states:\n");
	for (int i = 0; i < this->parameters.idle.size(); i++)
		printf("\t%d", this->parameters.idle.at(i));
	printf("\n");

	printf("Current Queue Outputs:\n");
	for (int i = 0; i < this->parameters.queues.size(); i++)
		printf("\t%d", this->parameters.queues.at(i));
	printf("\n");

	printf("Current Queue Wait Times\n");
	for (int i = 0; i < this->parameters.inputTimes.size(); i ++)
	{
		for (int j = 0; j < this->parameters.inputTimes.at(i).size(); j++)
			printf("\t%1.4f", this->parameters.inputTimes.at(i).at(j));
		printf("\n");
	}

	printf("Current Waits:\n");
	for (int i = 0; i < this->parameters.averageTime.size(); i++)
		printf("\t%1.4f", this->parameters.averageTime.at(i));
	printf("\n");

	printf("Production Amounts:\n");
	for (int i = 0; i < this->parameters.productionAmount.size(); i++)
		printf("\t%d", this->parameters.productionAmount.at(i));
	printf("\n");

	printf("Time Spent In System\n");
	for (int i = 0; i < this->parameters.itemsInSystem.size(); i++)
		printf("\t%1.4f", this->parameters.itemsInSystem.at(i));
	printf("\n");

	printf("Input Rates\n");
	for (int i = 0; i < this->parameters.inputRate.size(); i++)
		printf("\t%1.4f", this->parameters.inputRate.at(i));
	printf("\n");
	printf("\n");
}

/*
* Prints the final statistics of the model:
*	Component and Part production rates
*	Worker and Inspector Idle and Busy Probabilities
*	Average Queue Occupancies
*/
void Simulator::printStats()
{
	printf("---------------<Final Statistics>---------------\n");
	printf("Total Time: %13.3fs\n\n", this->parameters.modelTime);
	printf("Production Per Minute:\n");
	printf("\tComponent1: %5.2f comp/min\n",		this->parameters.productionAmount.at(Component1) / (this->parameters.modelTime / 60));
	printf("\tComponent2: %5.2f comp/min\n",		this->parameters.productionAmount.at(Component2) / (this->parameters.modelTime / 60));
	printf("\tComponent3: %5.2f comp/min\n",		this->parameters.productionAmount.at(Component3) / (this->parameters.modelTime / 60));
	printf("\tPart1-----: %5.2f part/min\n",		this->parameters.productionAmount.at(Part1)	  / (this->parameters.modelTime / 60));
	printf("\tPart2-----: %5.2f part/min\n",		this->parameters.productionAmount.at(Part2)	  / (this->parameters.modelTime / 60));
	printf("\tPart3-----: %5.2f part/min\n\n",	this->parameters.productionAmount.at(Part3)	  / (this->parameters.modelTime / 60));

	printf("Idle and Busy Probability:\n");
	printf("\tInspector1) Idle: %1.4f, Busy: %1.4f\n",	this->parameters.idleProb.at(0), this->parameters.busyTimes.at(0));
	printf("\tInspector2) Idle: %1.4f, Busy: %1.4f\n",	this->parameters.idleProb.at(1), this->parameters.busyTimes.at(1));
	printf("\t   Worker1) Idle: %1.4f, Busy: %1.4f\n",	this->parameters.idleProb.at(2), this->parameters.busyTimes.at(2));
	printf("\t   Worker2) Idle: %1.4f, Busy: %1.4f\n",	this->parameters.idleProb.at(3), this->parameters.busyTimes.at(3));
	printf("\t   Worker3) Idle: %1.4f, Busy: %1.4f\n\n",	this->parameters.idleProb.at(4), this->parameters.busyTimes.at(4));

	printf("Average Queue Occupancies:\n");
	printf("\tQueue C1-1: %1.4f Components\n",	this->parameters.queueOccupancy.at(0));
	printf("\tQueue C1-2: %1.4f Components\n",	this->parameters.queueOccupancy.at(1));
	printf("\tQueue C1-3: %1.4f Components\n",	this->parameters.queueOccupancy.at(2));
	printf("\tQueue C2-1: %1.4f Components\n",	this->parameters.queueOccupancy.at(3));
	printf("\tQueue C3-1: %1.4f Components\n\n",	this->parameters.queueOccupancy.at(4));

	printf("---------------<Extra Parameters>---------------\n");
	printf("Input Rates:\n");
	printf("\tC1: %1.4f Component/min\n",		60 / this->parameters.inputRate.at(Component1));
	printf("\tC2: %1.4f Component/min\n",		60 / this->parameters.inputRate.at(Component2));
	printf("\tC3: %1.4f Component/min\n",		60 / this->parameters.inputRate.at(Component3));
	printf("\tP1: %1.4f Part/min\n",			60 / this->parameters.inputRate.at(Part1));
	printf("\tP2: %1.4f Part/min\n",			60 / this->parameters.inputRate.at(Part2));
	printf("\tP3: %1.4f Part/min\n\n",		60 / this->parameters.inputRate.at(Part3));

	printf("Items In System:\n");
	printf("\tC1: %1.4f Average Components\n",	this->parameters.itemsInSystem.at(Component1));
	printf("\tC2: %1.4f Average Components\n",	this->parameters.itemsInSystem.at(Component2));
	printf("\tC3: %1.4f Average Components\n",	this->parameters.itemsInSystem.at(Component3));
	printf("\tP1: %1.4f Average Parts\n",			this->parameters.itemsInSystem.at(Part1));
	printf("\tP2: %1.4f Average Parts\n",			this->parameters.itemsInSystem.at(Part2));
	printf("\tP3: %1.4f Average Parts\n\n",		this->parameters.itemsInSystem.at(Part3));

	printf("Average Time Spent In System\n");
	printf("\tC1: %3.4f Average Time in Minutes\n",		this->parameters.averageTime.at(Component1) / 60);
	printf("\tC2: %3.4f Average Time in Minutes\n",		this->parameters.averageTime.at(Component2) / 60);
	printf("\tC3: %3.4f Average Time in Minutes\n",		this->parameters.averageTime.at(Component3) / 60);
	printf("\tP1: %3.4f Average Time in Minutes\n",		this->parameters.averageTime.at(Part1) / 60);
	printf("\tP2: %3.4f Average Time in Minutes\n",		this->parameters.averageTime.at(Part2) / 60);
	printf("\tP3: %3.4f Average Time in Minutes\n\n",		this->parameters.averageTime.at(Part3) / 60);

	printf("------------------------------------------------\n");
	printf("Littles Law Calculation:\n");
	printf("\tComponent 1: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Component1),
		this->parameters.averageTime.at(Component1) / this->parameters.inputRate.at(Component1));

	printf("\tComponent 2: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Component2),
		this->parameters.averageTime.at(Component2) / this->parameters.inputRate.at(Component2));

	printf("\tComponent 3: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Component3),
		this->parameters.averageTime.at(Component3) / this->parameters.inputRate.at(Component3));

	printf("\tPart 1: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Part1),
		this->parameters.averageTime.at(Part1) / this->parameters.inputRate.at(Part1));

	printf("\tPart 2: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Part2),
		this->parameters.averageTime.at(Part2) / this->parameters.inputRate.at(Part2));

	printf("\tPart 3: L = %5.4f, lambda * w = %5.4f\n", this->parameters.itemsInSystem.at(Part3),
		this->parameters.averageTime.at(Part3) / this->parameters.inputRate.at(Part3));



	//std::vector<double> inputRate;
//std::vector<double> itemsInSystem;
//std::vector<double> averageTime;
}

/*
* Sets system parameters to defaults
*/
void Simulator::clearParams()
{
	this->parameters.modelTime = 0;
	this->parameters.deltaTime = 0;

	for (int i = 0; i < this->parameters.agents.size(); i++)
		this->parameters.agents.at(i)->~Agent();
	this->parameters.agents.clear();

	for (int i = 0; i < this->parameters.solutions.size(); i++)
		this->parameters.solutions.at(i)->~QueueSolution();
	this->parameters.solutions.clear();

	for (int i = 0; i < this->parameters.generators.size(); i++)
		this->parameters.generators.at(i)->~NumberGenerator();
	this->parameters.generators.clear();

	this->parameters.queues.clear();
	this->parameters.entityData.clear();
	this->parameters.idle.clear();
	this->parameters.busyTimes.clear();
	this->parameters.queueOccupancy.clear();
	this->parameters.idleProb.clear();
	this->parameters.productionAmount.clear();
}

/*
* Code to set the current event to process
*/
void Simulator::setProcessingEvent(Events event_given, Agents agent_given, double time_given)
{
	this->parameters.processingEvent.agent_given = agent_given;
	this->parameters.processingEvent.event_given = event_given;
	this->parameters.processingEvent.time		 = time_given;
}

/*
* Code to create a component with probability specifications
*/
void Simulator::createComponent(Entities entity, int mean, int std_dist)
{
	NumberGenerator* gen_ptr = new NumberGenerator(mean, std_dist);
	struct entityData_st ent_data;
	ent_data.entityGenerator = gen_ptr;
	ent_data.entityType = entity;
	this->parameters.generators.push_back(gen_ptr);
	this->parameters.entityData.push_back(ent_data);
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
	agent_ptr->setProductionTarget(&(this->parameters.productionAmount));

	this->parameters.agents.push_back(agent_ptr);
	this->parameters.idle.push_back(agent_ptr->getState().idle);
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
	agent_ptr->setProductionTarget(&(this->parameters.productionAmount));

	this->parameters.agents.push_back(agent_ptr);
	this->parameters.idle.push_back(agent_ptr->getState().idle);
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

	for (int i = 0; i < this->parameters.agents.size(); i++) {
		this->parameters.idleProb.at(i) += this->parameters.agents.at(i)->getState().idle * this->parameters.deltaTime;
		this->parameters.busyTimes.at(i) += !this->parameters.agents.at(i)->getState().idle * this->parameters.deltaTime;


		if (this->parameters.agents.at(i)->newEntityProduced())
		{
			component = this->parameters.agents.at(i)->getState().currentEntity;
			this->parameters.inputRate.at(component) += this->parameters.agents.at(i)->getState().productionTime;

			this->parameters.averageTime.at(component) += this->parameters.agents.at(i)->getState().productionTime;

			this->parameters.itemsInSystem.at(component) += this->parameters.agents.at(i)->getState().productionTime;

			component = this->parameters.agents.at(i)->getState().priorEntity;
			this->parameters.itemsInSystem.at(component) += this->parameters.modelTime - this->parameters.agents.at(i)->getState().lastProductionTime;

		}
	}

	for (int i = 0; i < this->parameters.queues.size(); i++)
	{
		this->parameters.queueOccupancy.at(i) += this->parameters.queues.at(i) * this->parameters.deltaTime;
	}
}