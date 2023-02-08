#pragma once
#include "ModelEnums.h"

class FET
{
public:
	FET();

	int addEvent(struct event_data_st event_in);
	int getNextEvent(struct event_data_st* event_out);

	~FET();

private:
	class eventNode
	{
	public:
		eventNode(struct event_data_st);
		~eventNode();
		
		struct event_data_st data;

		eventNode* next;
	};

	eventNode* nextEvent;
};

