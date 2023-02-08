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

	setProcessingEvent(Depart, Inspector1, 1.5);
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	setProcessingEvent(Depart, Worker1, 0.5);
	this->parameters.boba_fet->addEvent(this->parameters.processingEvent);
	
	
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
	this->parameters.queues = { 1 };

	// Create An Entity
	NumberGenerator* gen_ptr = new NumberGenerator(1, 0);

	struct entityData_st ent_data;
	ent_data.entityGenerator = gen_ptr;
	ent_data.entityType = Component1;
	
	this->parameters.generators.push_back(gen_ptr);
	this->parameters.entityData.push_back(ent_data);

	int id = 1;
	Agents agent = Inspector1;

	// Inspector 1:
	Agent* agent_ptr = new Inspector( id, agent);
	QueueSolution* sol_ptr = new FirstQueue();

	sol_ptr->setAvailable({Component1});
	sol_ptr->setOccupancy(&(this->parameters.queues));

	agent_ptr->addToEntityList(ent_data);
	agent_ptr->addNotams(Worker1);
	agent_ptr->setQueueSolution(sol_ptr);
	agent_ptr->setCurrentEntity(Component1);

	this->parameters.solutions.push_back(sol_ptr);
	this->parameters.agents.push_back(agent_ptr);

	// Inspector 2:

	// Worker 1:

	// Worker 2:

	// Worker 3:


	return 0;
}

int Simulator::simPass()
{
	this->parameters.deltaTime = this->parameters.modelTime;

	if (this->parameters.boba_fet->getNextEvent(&(this->parameters.processingEvent)))
		return 0;

	this->parameters.modelTime = this->parameters.processingEvent.time;
	this->parameters.deltaTime = this->parameters.modelTime - this->parameters.deltaTime;

	printf("Model Time: %f\n", this->parameters.modelTime);
	printf("Delta Time: %f\n", this->parameters.deltaTime);

	for (int i = 0; i < this->parameters.agents.size(); i++) {
		if (this->parameters.agents.at(i)->getState().agentID == this->parameters.processingEvent.agent_given)
			this->parameters.agents.at(i)->processEvent(this->parameters.processingEvent);
	}

	printf("Queue Occupancy: %d\n\n", this->parameters.queues.at(0));

	return 1;
}

void Simulator::setProcessingEvent(Events event_given, Agents agent_given, double time_given)
{
	this->parameters.processingEvent.agent_given = agent_given;
	this->parameters.processingEvent.event_given = event_given;
	this->parameters.processingEvent.time		 = time_given;
}
