#include "Simulator.h"

int main() {
	Simulator sim = Simulator(1001100);

	sim.setBatchTime(1000000);
	sim.setSetupTime(1000);

	sim.launchSimulator();
	sim.calculateBatchData();
	std::vector<modelData_st> data = sim.getBatchData();

	for (int i = 1; i < data.size(); i++)
		sim.printStats(data.at(i));

	sim.~Simulator();

	return 0;
}