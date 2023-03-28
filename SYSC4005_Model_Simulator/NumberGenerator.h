#pragma once
#include <stdlib.h>
#include <cmath>

class NumberGenerator
{
public:
	NumberGenerator() {}

	virtual double generateNumber(int positive) = 0;

	~NumberGenerator() {}
private:
};

class LCM
{
public:
	LCM();
	LCM(unsigned int multiplier, unsigned int increment, unsigned int modulus, unsigned int seed);

	void setMultiplier(unsigned int multiplier) { this->multiplier = multiplier; }
	void setIncrement(unsigned int increment) { this->increment = increment; }
	void setModulus(unsigned int modulus) { this->modulus = modulus; }
	void setSeed(unsigned int seed);

	double generateNumber(int positive);

	~LCM();
private:
	unsigned int multiplier, increment, modulus;
	unsigned int value;
};

class ExponentialDist : public NumberGenerator
{
public:
	ExponentialDist();
	ExponentialDist(double lambda);

	void setLambda(double lambda);
	void setSeed(int seed) { this->generator.setSeed(seed); }

	double generateNumber(int positive) override;

	~ExponentialDist();

private:
	double inv_lambda;
	LCM generator;
};
