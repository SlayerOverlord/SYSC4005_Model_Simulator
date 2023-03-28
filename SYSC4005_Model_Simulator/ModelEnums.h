#pragma once

#define MAX_QUEUE_SIZE 2
#define MIN_QUEUE_SIZE 0

enum Events {
	NoEvent	= 0,
	Depart	= 1,
	Notam	= 2
};

enum Entities {
	Initialize	= -1,
	NoEntity	= 0,
	Component1	= 1,
	Component2	= 2,
	Component3	= 3,
	Part1		= 4,
	Part2		= 5,
	Part3		= 6
};

enum Agents {
	NoAgent		= 0,
	Inspector1	= 1,
	Inspector2	= 2,
	Worker1		= 3,
	Worker2		= 4,
	Worker3		= 5
};

struct event_data_st {
	Events event_given;
	Agents agent_given;
	double time_start;
	double time;
};