#pragma once
#include <vector>
#include "ModelEnums.h"

class QueueSolution
{
public:
	QueueSolution();
	QueueSolution(std::vector<unsigned int> availableQueues);
	QueueSolution(unsigned int availableQueues[], unsigned int size);

	void setAvailable(unsigned int availableQueues[], unsigned int size);
	void setAvailable(std::vector<unsigned int> availableQueues) { this->availableQueues.assign(availableQueues.begin(), availableQueues.end()); }
	void setOccupancy(std::vector<unsigned int>* occupancy) { this->occupancy = occupancy; }
	void setInputVector(std::vector<std::vector<double>>* inputTimes) { this->inputTImes = inputTimes; }

	virtual int processSolution(int data, double currentTime) = 0;

	~QueueSolution();

protected:
	int setup;
	std::vector<unsigned int>* occupancy;
	std::vector<unsigned int> availableQueues;
	std::vector<std::vector<double>>* inputTImes;
};

class ShortPrio : public QueueSolution
{
public:
	ShortPrio() : QueueSolution() {}
	ShortPrio(std::vector<unsigned int> availableQueues) : QueueSolution(availableQueues) {}
	ShortPrio(unsigned int availableQueues[], unsigned int size) : QueueSolution(availableQueues, size) {}
	~ShortPrio() {}

	int processSolution(int data, double currentTime);
};

class FirstQueue : public QueueSolution
{
public:
	FirstQueue() : QueueSolution() {}
	~FirstQueue() {}
	int processSolution(int data, double currentTime);
};

class WorkerQueue : public QueueSolution
{
public:
	WorkerQueue() : QueueSolution() {}
	~WorkerQueue() {}
	int processSolution(int data, double currentTime);
};