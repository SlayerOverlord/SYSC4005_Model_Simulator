#include "Simulator.h"

int main() {
	Simulator sim = Simulator(100000);
	sim.launchSimulator();
	sim.~Simulator();

	return 0;
}