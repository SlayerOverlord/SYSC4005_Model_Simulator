#include "NumberGenerator.h"
#include <stdio.h>

double NumberGenerator::generateNumber(int positive)
{
    double d = 0;

    do {
        d = rand() % (int)(this->mean - this->std_dist);
        d = this->std_dist + d;
    } while (positive && d < 0);

    return d;
}
