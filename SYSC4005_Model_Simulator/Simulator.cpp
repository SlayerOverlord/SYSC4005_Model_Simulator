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


	setProcessingEvent(Depart, Inspector2, 1.5);
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
}

int Simulator::init()
{
	this->parameters.agents.clear();

	int id = 1;
	Agents agent = Inspector1;

	// Inspector 1:
	Agent* agent_ptr = new Inspector( id, agent);
	agent_ptr->setNumberGenerator(new NumberGenerator(0, 100));
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
	printf("Delta Time: %f\n\n", this->parameters.deltaTime);

	return 1;
}

void Simulator::setProcessingEvent(Events event_given, Agents agent_given, double time_given)
{
	this->parameters.processingEvent.agent_given = agent_given;
	this->parameters.processingEvent.event_given = event_given;
	this->parameters.processingEvent.time		 = time_given;
}
