#include "Inspector.h"

std::vector<struct event_data_st> Inspector::processEvent(struct event_data_st event_given, double currentTime)
{
	std::vector<struct event_data_st> output;
	output.clear();

	if (solution == nullptr)
		return output;

	switch (event_given.event_given) {
	case Depart:
		output = departEvent(currentTime);
		break;

	case Notam:
		output = notamEvent(currentTime);
		break;

	case NoEvent:
	default:
		break;
	}

	return output;
}

std::vector<struct event_data_st> Inspector::departEvent(double currentTime)
{
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();

	int res = solution->processSolution(this->state.currentEntity, currentTime);
	if (res) {

		entityData_st entityData = genNewEntity();
		this->state.priorEntity = this->state.currentEntity;
		this->state.currentEntity = entityData.entityType;
		this->produced->at(entityData.entityType)++;

		this->state.productionTime = entityData.entityGenerator->generateNumber(1);

		event_data_st data;
		data.time = this->state.productionTime + currentTime;
		data.event_given = Depart;
		data.agent_given = this->state.agentID;

		temp_vec.push_back(this->notams.at(res - 1));
		for (int i = 0; i < temp_vec.size(); i++)
		{
			temp_vec.at(i).time += currentTime;
			temp_vec.at(i).time_start = currentTime;
		}

		temp_vec.push_back(data);

		newEntityInSystem = true;
		this->state.lastProductionTime = currentTime;
	}
	else
	{
		this->state.idle = 1;
		this->state.lastProductionTime = currentTime;
		this->state.priorEntity = this->state.currentEntity;
	}

	return temp_vec;
}

std::vector<struct event_data_st> Inspector::notamEvent(double currentTime)
{
	if (this->state.idle) {
		this->state.idle = 0;
		return departEvent(currentTime);
	}
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();
	return temp_vec;
}