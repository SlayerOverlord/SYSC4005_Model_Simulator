#pragma once
#include <vector>
#include "ModelEnums.h"

class EventProcessor
{
public:
	virtual std::vector<struct event_data_st> processEvent(struct event_data_st event_to_process) = 0;
	std::vector<struct event_data_st> getEventList() { return this->eventList; }

protected:
	std::vector<struct event_data_st> eventList;
};

