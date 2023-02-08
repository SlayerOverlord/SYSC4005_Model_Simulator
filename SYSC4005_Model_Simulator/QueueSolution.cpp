#include "QueueSolution.h"

QueueSolution::QueueSolution(unsigned int availableQueues[], unsigned int size)
{
	setAvailable(availableQueues, size);
}

QueueSolution::QueueSolution(std::vector<unsigned int> availableQueues)
{
	setAvailable(availableQueues);
}

void QueueSolution::setAvailable(unsigned int availableQueues[], unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
		this->availableQueues.push_back(availableQueues[i]);
}

QueueSolution::~QueueSolution()
{
	this->availableQueues.~vector();
	this->occupancy = nullptr;
}
