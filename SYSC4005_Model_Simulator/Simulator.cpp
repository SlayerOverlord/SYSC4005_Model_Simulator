#include "Simulator.h"

Simulator::Simulator()
{
	this->parameters.boba_fet = new FET();

	this->parameters.modelTime = 0;
	this->parameters.deltaTime = 0;
	setProcessingEvent(NoEvent, NoAgent, 0);
}

void Simulator::launchSimulator()
{
	init();

	setProcessingEvent(Depart, Inspector1, 0);
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	setProcessingEvent(Depart, Inspector2, 0);
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	
	printState();
	while (this->simPass());

}



Simulator::~Simulator()
{
	this->parameters.boba_fet->~FET();

	this->parameters.modelTime = 0;
	this->parameters.deltaTime = 0;
	this->parameters.processing.~vector();
	this->parameters.queues.~vector();
	this->parameters.idle.~vector();

	this->parameters.agents.clear();
	this->parameters.agents.~vector();

/*
* 	std::vector<Agent*>				agents;
	std::vector<QueueSolution*>		solutions;
	std::vector<NumberGenerator*>	generators;
	std::vector<entityData_st>		entityData;
	std::vector<event_data_st>		notams;
*/

	for (int i = 0; i < this->parameters.agents.size(); i++)
		this->parameters.agents.at(i)->~Agent();
	this->parameters.agents.clear();

	for (int i = 0; i < this->parameters.solutions.size(); i++)
		this->parameters.solutions.at(i)->~QueueSolution();
	this->parameters.solutions.clear();

	for (int i = 0; i < this->parameters.generators.size(); i++)
		this->parameters.generators.at(i)->~NumberGenerator();
	this->parameters.generators.clear();

	this->parameters.entityData.clear();

	this->parameters.agents.~vector();
	this->parameters.solutions.~vector();
	this->parameters.generators.~vector();
	this->parameters.entityData.~vector();
}

int Simulator::init()
{
	// Seting Queues up:
	this->parameters.queues = { 0, 0, 0, 0, 0 };

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
	entities = {Component1, Component1, Component1, NoEntity, NoEntity};
	notams = { Worker1, Worker2, Worker3, NoAgent, NoAgent };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createInspector(Inspector1, Component1, data, notams, 0, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

	// Inspector 2:
	sol_ptr = new FirstQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Component2));
	data.push_back(this->parameters.entityData.at(Component3));
	entities = { NoEntity, NoEntity, NoEntity, Component2, Component3 };
	notams = { NoAgent, NoAgent, NoAgent, Worker2, Worker3 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createInspector(Inspector2, Component2, data, notams, 0, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);


	// Worker 1:
	sol_ptr = new WorkerQueue();

	data.clear();
	notams.clear();
	entities.clear();

	data.push_back(this->parameters.entityData.at(Part1));
	entities = { Component1, NoEntity, NoEntity, NoEntity, NoEntity };
	notams = { Inspector1 };

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
	entities = { NoEntity, Component1, NoEntity, Component2, NoEntity };
	notams = { Inspector1, Inspector2 };

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
	entities = { NoEntity, NoEntity, Component1, NoEntity, Component3 };
	notams = { Inspector1, Inspector2 };

	sol_ptr->setAvailable(entities);
	sol_ptr->setOccupancy(&(this->parameters.queues));

	createWorker(Worker3, Part3, data, notams, 1, sol_ptr);
	this->parameters.solutions.push_back(sol_ptr);

	return 0;
}

int Simulator::simPass()
{
	std::vector<event_data_st> eventData;
	eventData.clear();

	this->parameters.deltaTime = this->parameters.modelTime;

	if (this->parameters.boba_fet->getNextEvent(&(this->parameters.processingEvent)))
		return 0;

	this->parameters.modelTime = this->parameters.processingEvent.time;
	this->parameters.deltaTime = this->parameters.modelTime - this->parameters.deltaTime;

	for (int i = 0; i < this->parameters.agents.size(); i++) {
		if (this->parameters.processingEvent.agent_given == Worker1) {
			printf("");
		}
		if (this->parameters.agents.at(i)->getState().agentID == this->parameters.processingEvent.agent_given) {
			eventData = this->parameters.agents.at(i)->processEvent(this->parameters.processingEvent);
			this->parameters.idle.at(i) = this->parameters.agents.at(i)->getState().idle;
			break;
		}
	}

	if (eventData.size() > 0) {
		for (int i = 0; i < eventData.size(); i++) {
			if (eventData.at(i).agent_given != NoAgent)
				eventData.at(i).time += this->parameters.modelTime;
				this->parameters.boba_fet->addEvent(eventData.at(i));
		}
	}

	printState();

	return 1;
}

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
		printf("\t%d\n", this->parameters.idle.at(i));

	printf("Current Queue Outputs:\n");
	for (int i = 0; i < this->parameters.queues.size(); i++)
		printf("\t%d\n", this->parameters.queues.at(i));

	printf("\n");
}

void Simulator::setProcessingEvent(Events event_given, Agents agent_given, double time_given)
{
	this->parameters.processingEvent.agent_given = agent_given;
	this->parameters.processingEvent.event_given = event_given;
	this->parameters.processingEvent.time		 = time_given;
}

void Simulator::createComponent(Entities entity, int mean, int std_dist)
{
	NumberGenerator* gen_ptr = new NumberGenerator(mean, std_dist);
	struct entityData_st ent_data;
	ent_data.entityGenerator = gen_ptr;
	ent_data.entityType = entity;
	this->parameters.generators.push_back(gen_ptr);
	this->parameters.entityData.push_back(ent_data);
}

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

	this->parameters.agents.push_back(agent_ptr);
	this->parameters.idle.push_back(agent_ptr->getState().idle);
}

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

	this->parameters.agents.push_back(agent_ptr);
	this->parameters.idle.push_back(agent_ptr->getState().idle);
}