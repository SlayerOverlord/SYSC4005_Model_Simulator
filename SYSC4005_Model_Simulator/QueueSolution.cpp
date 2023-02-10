#include "QueueSolution.h"

QueueSolution::QueueSolution(unsigned int availableQueues[], unsigned int size)
{
	setAvailable(availableQueues, size);
	this->occupancy = nullptr;
	this->setup = 1;
}

QueueSolution::QueueSolution()
{
	availableQueues.clear();
	this->occupancy = nullptr;
	this->setup = 1;
}

QueueSolution::QueueSolution(std::vector<unsigned int> availableQueues)
{
	setAvailable(availableQueues);
	this->occupancy = nullptr;
	this->setup = 1;
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

int ShortPrio::processSolution(int data)
{
	if (this->occupancy == nullptr)
		return 0;
	if (this->availableQueues.size() <= 0)
		return 0;
	if (this->availableQueues.size() > this->occupancy->size())
		return 0;

	int position = 0;
	int length = MAX_QUEUE_SIZE;

	for (int i = 0; i < this->occupancy->size(); i++) {
		if (this->availableQueues.at(i) == data && this->occupancy->at(i) < length) {
			position = i + 1;
			length = this->occupancy->at(i);

		}
	}

	if (length >= MAX_QUEUE_SIZE)
		position = 0;
	else if (!this->setup) {
		this->occupancy->at(position - 1)++;
	}

	this->setup = 0;

	length = 0;
	return position;
}

int FirstQueue::processSolution(int data)
{
	if (this->occupancy == nullptr)
		return 0;
	if (this->availableQueues.size() <= 0)
		return 0;
	if (this->availableQueues.size() > this->occupancy->size())
		return 0;

	int position = 0;
	int length = MAX_QUEUE_SIZE;

	for (int i = 0; i < this->occupancy->size(); i++) {
		if (this->availableQueues.at(i) == data && this->occupancy->at(i) < MAX_QUEUE_SIZE
			&& position == 0) {

			position = i + 1;
			length = this->occupancy->at(i);
			break;
		}
	}

	if (position != 0 && !this->setup)
		this->occupancy->at(position - 1) ++;
	this->setup = 0;

	length = 0;
	return position;
}

int WorkerQueue::processSolution(int data)
{
	if (this->occupancy == nullptr)
		return 0;
	if (this->availableQueues.size() <= 0)
		return 0;
	if (this->availableQueues.size() > this->occupancy->size())
		return 0;

	for (int i = 0; i < this->occupancy->size(); i++) {
		if (this->availableQueues.at(i) != 0 && this->occupancy->at(i) <= MIN_QUEUE_SIZE)
			return 0;
	}

	for (int i = 0; i < this->occupancy->size(); i++) {
		if (this->availableQueues.at(i) != 0)
			this->occupancy->at(i)--;
	}

	return 1;
}
