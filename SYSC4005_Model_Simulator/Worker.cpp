#include "Worker.h"

std::vector<struct event_data_st> Worker::processEvent(struct event_data_st event_given, double currentTime)
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

std::vector<struct event_data_st> Worker::departEvent(double currentTime)
{
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();

	if (solution->processSolution(this->state.currentEntity, currentTime)) {


		this->state.priorEntity = this->state.currentEntity;

		entityData_st entityData = genNewEntity();
		this->state.currentEntity = entityData.entityType;
		this->produced->at(entityData.entityType) ++;

		this->state.productionTime = entityData.entityGenerator->generateNumber(1);

		event_data_st data;
		data.time = this->state.productionTime + currentTime;
		data.event_given = Depart;
		data.agent_given = this->state.agentID;

		temp_vec.assign(this->notams.begin(), this->notams.end());
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
		this->state.currentEntity = NoEntity;
		this->state.priorEntity = NoEntity;
		this->state.productionTime = 0;
		this->state.lastProductionTime = currentTime;
		newEntityInSystem = true;
	}

	return temp_vec;
}

std::vector<struct event_data_st> Worker::notamEvent(double currentTime)
{
	if (this->state.idle) {
		this->state.idle = 0;
		return departEvent(currentTime);
	}
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();
	return temp_vec;
}
