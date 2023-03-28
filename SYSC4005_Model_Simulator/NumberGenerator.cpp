#include "NumberGenerator.h"
#include <stdio.h>

/*
* Default constructor for LCM generator
* m = 2^24
* c = 2^16 - 1
* a = 4 * 2^9 + 1
* X = 684651
*/
LCM::LCM()
{
	// Data created based on Chapter 7 slid 9 recomondation
	this->modulus = 16777216;	// Power of 2 (2^24)
	this->increment = 65535;	// Co-prime to modulus
	this->multiplier = 2049;	// Integer value following 1+4k
	this->value = 684651;		// Initial starting seed
}

/*
* Constructor to define each element of the LCM generator
* m = modulus
* c = increment
* a = multiplier
* X = seed
*/
LCM::LCM(unsigned int modulus, unsigned int increment, unsigned int multiplier, unsigned int seed)
{
	this->multiplier = multiplier;
	this->increment = increment;
	this->modulus = modulus;

	this->setSeed(seed);
}

/*
* Allows for modification of the LCM generator's seed after instanciation
*/
void LCM::setSeed(unsigned int seed)
{
	// Ensure that the seed falls in the range: 0 < X < m
	if (seed == 0)
		this->value = 1;
	else if (seed >= this->modulus)
		this->value = this->modulus - 1;
	else
		this->value = seed;
}

/*
* Function for generating pseudo-random variates
*
*	int positive is for continuity with the Number Generator class of
* the system model, allowing the class to be used without change to the
* source code.
*/
double LCM::generateNumber(int positive)
{
	// Performs the operation Xi = (a*Xi-1 + c)mod m from chapter 7 slide 7
	this->value = (this->multiplier * this->value + this->increment) % this->modulus;

	// Performs the operation R = Xi / m to generate a random variate
	return this->value / (double)this->modulus;
}

/*
* Default Destructor, just sets values to 0
*/
LCM::~LCM()
{
	this->multiplier = 0;
	this->increment = 0;
	this->modulus = 0;
	this->value = 0;
}

/*
* Default Constructor, negative inverse of lambda is used to
* avoid repeated division operations when generating random
* variates
*/
ExponentialDist::ExponentialDist() : NumberGenerator()
{
	this->inv_lambda = -1;
}

/*
* Constructor with specification for the lambda value of
* the exponential distribution
*/
ExponentialDist::ExponentialDist(double lambda) : NumberGenerator()
{
	this->setLambda(lambda);
}

/*
* Sets the lambda value of the exponential distribution
* after instanciation, calculates negative inverse of
* lambda to avoid excess division operations
*/
void ExponentialDist::setLambda(double lambda)
{
	// Ensure that the lambda value is not at or below 0
	if (lambda <= 0)
		this->inv_lambda = -1.0;
	else
		this->inv_lambda = -1.0 / lambda;
}

/*
* Generates a random variate based on the exponential
* distribution
*
*	int positive is unused but included to allow the
* distribution to be used in the simulated model without
* having to change the source code.
*/
double ExponentialDist::generateNumber(int positive)
{
	// Generates a random uniform variable in range (0, 1]
	double R = generator.generateNumber(0);

	// Returns the result of the function: -1/lambda * ln(R)
	// See chapter 8, slide 6 for derivation
	return this->inv_lambda * log(R);
}

/*
* Default Destructor for exponential distribution, sets values to 0
*/
ExponentialDist::~ExponentialDist()
{
	this->inv_lambda = 0.0;
}