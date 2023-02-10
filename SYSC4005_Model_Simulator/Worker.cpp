#include "Worker.h"

std::vector<struct event_data_st> Worker::processEvent(struct event_data_st event_given)
{
	std::vector<struct event_data_st> output;
	output.clear();

	if (solution == nullptr)
		return output;

	switch (event_given.event_given) {
	case Depart:
		output = departEvent();
		break;

	case Notam:
		output = notamEvent();
		break;

	case NoEvent:
	default:
		break;
	}

	return output;
}

std::vector<struct event_data_st> Worker::departEvent()
{
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();

	if (solution->processSolution(this->state.currentEntity)) {

		entityData_st entityData = genNewEntity();
		this->state.currentEntity = entityData.entityType;
		this->produced->at(entityData.entityType) ++;

		event_data_st data;
		data.time = entityData.entityGenerator->generateNumber(1);
		data.event_given = Depart;
		data.agent_given = this->state.agentID;

		temp_vec.assign(this->notams.begin(), this->notams.end());
		temp_vec.push_back(data);
	} else
		this->state.idle = 1;

	return temp_vec;
}

std::vector<struct event_data_st> Worker::notamEvent()
{
	if (this->state.idle) {
		this->state.idle = 0;
		return departEvent();
	}
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();
	return temp_vec;
}
