#include "Simulator.h"
#include "FileMan.h"
#include "FileParser.h"

#include <string>
#include <filesystem>

int main() {
	const int c_repl = 25;

	double buffOcc[5][c_repl];
	double prodAmount[3][c_repl];
	double wsBusyProb[3][c_repl];
	double inspIdleProb[2][c_repl];

	for (int i = 0; i < c_repl; i++)
	{
		printf("Run Number: %d\n", i + 1);

		Simulator sim = Simulator(50100);	// How long to run the sim
		sim.setBatchTime(500);	// Batch length
		sim.setSetupTime(5000);		// Setup length
		sim.setSeed(10 * (i + 23645));	// Different seed for each pass

		sim.launchSimulator();				// Run Simulator
		for (int buff = 0; buff < 5; buff++)
			buffOcc[buff][i] = sim.getBufferOccupancy(buff);
		for (int part = 4; part < 7; part++)
			prodAmount[part-4][i] = sim.getProdAmount(part);
		for (int ws = 2; ws < 5; ws++)
			wsBusyProb[ws-2][i] = sim.getWsBusyProb(ws);
		for (int insp = 0; insp < 2; insp++)
			inspIdleProb[insp][i] = sim.getInspIdleProb(insp);
	}
	if (c_repl == 1) return 0;

	// Get sample means:
	double avgBuffOcc[5];
	double avgProdAmount[3];
	double avgWsBusyProb[3];
	double avgInspIdleProb[2];
	for (int i = 0; i < 5; i++) {
		avgBuffOcc[i] = 0.0;
		for (int j = 0; j < c_repl; j++) avgBuffOcc[i] += buffOcc[i][j];
		avgBuffOcc[i] /= c_repl;
	}
	for (int i = 0; i < 3; i++) {
		avgProdAmount[i] = 0.0;
		for (int j = 0; j < c_repl; j++) avgProdAmount[i] += prodAmount[i][j];
		avgProdAmount[i] /= c_repl;
	}
	for (int i = 0; i < 3; i++) {
		avgWsBusyProb[i] = 0.0;
		for (int j = 0; j < c_repl; j++) avgWsBusyProb[i] += wsBusyProb[i][j];
		avgWsBusyProb[i] /= c_repl;
	}
	for (int i = 0; i < 2; i++) {
		avgInspIdleProb[i] = 0.0;
		for (int j = 0; j < c_repl; j++) avgInspIdleProb[i] += inspIdleProb[i][j];
		avgInspIdleProb[i] /= c_repl;
	}

	// Get sample std dev:
	double sdBuffOcc[5];
	double sdProdAmount[3];
	double sdWsBusyProb[3];
	double sdInspIdleProb[2];
	for (int i = 0; i < 5; i++) {
		sdBuffOcc[i] = 0.0;
		for (int j = 0; j < c_repl; j++) sdBuffOcc[i] += pow(buffOcc[i][j] - avgBuffOcc[i], 2);
		sdBuffOcc[i] /= c_repl - 1;
		sdBuffOcc[i] = sqrt(sdBuffOcc[i]);
	}
	for (int i = 0; i < 3; i++) {
		sdProdAmount[i] = 0.0;
		for (int j = 0; j < c_repl; j++) sdProdAmount[i] += pow(prodAmount[i][j] - avgProdAmount[i], 2);
		sdProdAmount[i] /= c_repl - 1;
		sdProdAmount[i] = sqrt(sdProdAmount[i]);
	}
	for (int i = 0; i < 3; i++) {
		sdWsBusyProb[i] = 0.0;
		for (int j = 0; j < c_repl; j++) sdWsBusyProb[i] += pow(wsBusyProb[i][j] - avgWsBusyProb[i], 2);
		sdWsBusyProb[i] /= c_repl - 1;
		sdWsBusyProb[i] = sqrt(sdWsBusyProb[i]);
	}
	for (int i = 0; i < 2; i++) {
		sdInspIdleProb[i] = 0.0;
		for (int j = 0; j < c_repl; j++) sdInspIdleProb[i] += pow(inspIdleProb[i][j] - avgInspIdleProb[i], 2);
		sdInspIdleProb[i] /= c_repl - 1;
		sdInspIdleProb[i] = sqrt(sdInspIdleProb[i]);
	}

	// Compute the CI:
	const double c_t_test_val = 2.06;
	double ciBuffOcc[5][2];
	double ciProdAmount[3][2];
	double ciWsBusyProb[3][2];
	double ciInspIdleProb[2][2];
	for (int i = 0; i < 5; i++) for (int j = -1; j < 2; j = j + 2) ciBuffOcc[i][j < 0 ? 0 : 1] = avgBuffOcc[i] + j * c_t_test_val * sdBuffOcc[i] / sqrt((double)c_repl);
	for (int i = 0; i < 3; i++) for (int j = -1; j < 2; j = j + 2) ciProdAmount[i][j < 0 ? 0 : 1] = avgProdAmount[i] + j * c_t_test_val * sdProdAmount[i] / sqrt((double)c_repl);
	for (int i = 0; i < 3; i++) for (int j = -1; j < 2; j = j + 2) ciWsBusyProb[i][j < 0 ? 0 : 1] = avgWsBusyProb[i] + j * c_t_test_val * sdWsBusyProb[i] / sqrt((double)c_repl);
	for (int i = 0; i < 2; i++) for (int j = -1; j < 2; j = j + 2) ciInspIdleProb[i][j < 0 ? 0 : 1] = avgInspIdleProb[i] + j * c_t_test_val * sdInspIdleProb[i] / sqrt((double)c_repl);
	
	// Output the CIs:
	std::vector<std::string> queues = {"C1-1","C1-2","C1-3","C2-2","C3-3"};
	std::vector<std::string> entities = { "Part1","Part2","Part3" };
	printf("\nQuantity\t\tEstimate\tConfidence Interval\tUnits\n========================================================================\n");
	for (int i = 0; i < 5; i++) printf("Buffer %s Occupancy\t%0.4f\t\t[%0.4f, %0.4f]\tComponents\n", queues.at(i).data(), avgBuffOcc[i], ciBuffOcc[i][0], ciBuffOcc[i][1]);
	for (int i = 0; i < 3; i++) printf("%s Throughput\t%0.4f\t\t[%0.4f, %0.4f]\tParts/min\n", entities.at(i).data(), avgProdAmount[i], ciProdAmount[i][0], ciProdAmount[i][1]);
	for (int i = 0; i < 3; i++) printf("WS%d Busy Probability\t%0.4f\t\t[%0.4f, %0.4f]\tNA\n", i+1, avgWsBusyProb[i], ciWsBusyProb[i][0], ciWsBusyProb[i][1]);
	for (int i = 0; i < 2; i++) printf("Insp%d Idle Probability\t%0.4f\t\t[%0.4f, %0.4f]\tNA\n", i+1, avgInspIdleProb[i], ciInspIdleProb[i][0], ciInspIdleProb[i][1]);


	printf("\n|t_0| values for buffer occupancies across all %d replications:\n", c_repl);
	double testStat[5];
	double trueVal[5] = { 0.28,0.41,0.32,0.60,1.75 };
	for (int i = 0; i < 5; i++) {
		testStat[i] = abs((avgBuffOcc[i] - trueVal[i]) * sqrt((double)(c_repl - 1)) / sdBuffOcc[i]);
		printf("   Buffer %s: %0.4f - %s\n", queues.at(i).data(), testStat[i], testStat[i]>c_t_test_val ? "Reject Model" : "Fail to reject model.");
	}

	printf("delta values for buffers across all %d replications:\n", c_repl);
	double critVal[5] = { 0.01,0.05,0.05,0.1,0.1 };
	for (int i = 0; i < 5; i++) printf("   Buffer %s: %0.4f for critical value %0.4f\n", queues.at(i).data(), critVal[i] / sdBuffOcc[i], critVal[i]);

	return 0;
}