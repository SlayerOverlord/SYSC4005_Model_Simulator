#include "Simulator.h"

int main() {
	Simulator sim = Simulator();
	sim.launchSimulator();
	sim.~Simulator();

	return 0;
}