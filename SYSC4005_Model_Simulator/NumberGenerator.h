#pragma once
#include <stdlib.h>

class NumberGenerator
{
public:
	NumberGenerator() { this->mean = 0; this->std_dist = 1; }
	NumberGenerator(double mean, double std_dist) { this->mean = mean; this->std_dist = std_dist; }

	double generateNumber(int positive);

	~NumberGenerator() { this->mean = 0; this->std_dist = 0; }
private:
	double mean, std_dist;
};

