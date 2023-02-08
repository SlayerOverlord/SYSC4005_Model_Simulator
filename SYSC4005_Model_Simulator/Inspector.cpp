#include "Inspector.h"

std::vector<struct event_data_st> Inspector::processEvent(Events event_given)
{
	std::vector<struct event_data_st> output;
	output.clear();

	if (generator == nullptr || solution == nullptr)
		return output;

	switch (event_given) {
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

std::vector<struct event_data_st> Inspector::departEvent(struct event_data_st event_given)
{
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();

	if (solution->processSolution()) {
		event_data_st data;
		data.time = this->generator->generateNumber(1);
		data.event_given = Depart;
		data.agent_given = this->state.agentID;

		temp_vec.assign(this->currentNotams.begin(), this->currentNotams.end());
		temp_vec.push_back(data);

	} else 
		this->state.idle = 1;

	return temp_vec;
}

std::vector<struct event_data_st> Inspector::notamEvent(struct event_data_st event_given)
{
	if (this->state.idle) {
		this->state.idle = 0;
		return departEvent(event_given);
	}
	std::vector<struct event_data_st> temp_vec;
	temp_vec.clear();
	return temp_vec;
}