#include "Simulator.h"
#include "FileMan.h"
#include "FileParser.h"

#include <string>
#include <filesystem>

std::string convertDoubleToString(const double& data)
{
	return std::to_string(data);
}

std::string convertUintToString(const unsigned int& data)
{
	return std::to_string(data);
}

std::vector<std::vector<std::string>> formatData(const modelData_st& data, int batch_num)
{
	std::vector<std::vector<std::string>> output;
	std::vector<std::string> line;
	std::vector<std::string> entities = {
		"Ignore",
		"Component1",
		"Component2",
		"Component3",
		"Part1",
		"Part2",
		"Part3"
	};
	std::vector<std::string> agents = {
		"Inspector1",
		"Inspector2",
		"Worker1",
		"Worker2",
		"Worker3"
	};
	std::vector<std::string> queues = {
		"C1-1",
		"C1-2",
		"C1-3",
		"C2-2",
		"C3-3"
	};

	char text[1024] = "Batch Number: %d";
	sprintf_s(text, text, batch_num);

	line.push_back(text);
	output.push_back(line);
	line.clear();

	line.push_back("Entity:,Num_Produced:,Arrival_Rate:,Items_In_System,Ave_Time_In_System:,Littles_Left:,Littles_Right:");
	output.push_back(line);
	line.clear();

	for (int i = 0; i < entities.size(); i++)
	{
		line.push_back(entities.at(i));
		line.push_back(convertUintToString(data.productionAmount.at(i)));
		line.push_back(convertDoubleToString(1/data.inputRate.at(i)));
		line.push_back(convertDoubleToString(data.itemsInSystem.at(i)));
		line.push_back(convertDoubleToString(data.averageTime.at(i)));
		line.push_back(line.at(3));
		line.push_back(convertDoubleToString(data.averageTime.at(i) / data.inputRate.at(i)));

		output.push_back(line);
		line.clear();
	}

	line.push_back("Agent:,Idle:,Busy:");
	output.push_back(line);
	line.clear();

	for (int i = 0; i < agents.size(); i++)
	{
		line.push_back(agents.at(i));
		line.push_back(convertDoubleToString(data.idleProb.at(i)));
		line.push_back(convertDoubleToString(data.busyTimes.at(i)));

		output.push_back(line);
		line.clear();
	}

	line.push_back("Queue:,Ocupancy:");
	output.push_back(line);
	line.clear();

	for (int i = 0; i < queues.size(); i++)
	{
		line.push_back(queues.at(i));
		line.push_back(convertDoubleToString(data.queueOccupancy.at(i)));

		output.push_back(line);
		line.clear();
	}
	
	line.push_back("\n");
	output.push_back(line);
	line.clear();

	return output;
}

void exportToCSV(const std::vector<modelData_st>& data, const int runNumber)
{
	FileMan manager;
	FileParser parser;
	std::filesystem::path path = std::filesystem::current_path();
	path /= "CSV_files";

	std::string fileNameGeneric = "SimRun_%d.csv";
	std::string fileName;
	char text[1024] = "";

	sprintf_s(text, fileNameGeneric.c_str(), runNumber);
	fileName = text;

	manager.setFilePath(path.string());
	manager.setFileName(fileName);

	std::vector<std::vector<std::string>> fileData;
	std::vector<std::string> formattedData;

	for (int i = 1; i < data.size(); i++)
	{
		manager.setMode(std::fstream::in | std::fstream::out | std::fstream::app);

		fileData = formatData(data.at(i), i);
		formattedData = parser.formatCSV(fileData);

		manager.writeFile(formattedData);

		fileData.clear();
		formattedData.clear();
	}
}

int main() {

	for (int i = 0; i < 25; i++)
	{
		Simulator sim = Simulator(101100);	// How long to run the sim (10 batches + 1 startup + extra)
		sim.setBatchTime(10000);	// Batch length (10,000 minutes)
		sim.setSetupTime(1000);		// Setup length (1,000 minutes)
		sim.setSeed(10 * (i + 23645));	// Different seed for each pass

		sim.launchSimulator();				// Run Simulator

		sim.calculateBatchData();							// Calculate batch data to output to file
		std::vector<modelData_st> data = sim.getBatchData();// Gathe batch data for a sim pass
		exportToCSV(data, i + 1);					// Export the sim data
	}

	return 0;
}