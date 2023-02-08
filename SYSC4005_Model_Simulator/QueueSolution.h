#pragma once
#include <vector>

class QueueSolution
{
public:
	QueueSolution(std::vector<unsigned int> availableQueues);
	QueueSolution(unsigned int availableQueues[], unsigned int size);

	void setAvailable(unsigned int availableQueues[], unsigned int size);
	void setAvailable(std::vector<unsigned int> availableQueues) { this->availableQueues = availableQueues;  }
	void setOccupancy(std::vector<unsigned int>* occupancy) { this->occupancy = occupancy; }

	virtual int processSolution(int data) = 0;

	~QueueSolution();

private:
	std::vector<unsigned int>* occupancy;
	std::vector<unsigned int> availableQueues;
};


