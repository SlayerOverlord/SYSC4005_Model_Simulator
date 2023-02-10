#include "FET.h"

/*
* Default Constructor for the FET
* Default timelimit of 100 seconds
*/
FET::FET()
{
    this->nextEvent = nullptr;
    this->time_lim = 100;
}

/*
* Constructor with definable time limit
*/
FET::FET(double time_lim)
{
    this->nextEvent = nullptr;
    this->time_lim = time_lim;
}

/*
* Adds an event to the FET, inserts the event between the last event to occur
* before the designated time, and the next event to occur after the designated
* time.
*/
int FET::addEvent(struct event_data_st event_in)
{
    // Guard Clauses
    if (event_in.time > time_lim) {
        return 0;
    }

    if (this->nextEvent == nullptr) {
        this->nextEvent = new eventNode(event_in);
        return 0;
    }
    
    eventNode* temp_ptr = new eventNode(event_in);

    if (this->nextEvent->data.time > temp_ptr->data.time) {
        temp_ptr->next = this->nextEvent;
        this->nextEvent = temp_ptr;
        temp_ptr = nullptr;

        return 0;
    }

    eventNode* curr_ptr = this->nextEvent;

    // Find the location to insert the event
    while (1) {
        if (curr_ptr->next == nullptr) {
            curr_ptr->next = temp_ptr;
            temp_ptr = nullptr;

            return 0;
        }

        if (curr_ptr->next->data.time > temp_ptr->data.time)
        {
            temp_ptr->next = curr_ptr->next;
            curr_ptr->next = temp_ptr;
            temp_ptr = nullptr;

            return 0;
        }

        curr_ptr = curr_ptr->next;
    }
}

/*
* Get the next event to occur in the simulation.
*/
int FET::getNextEvent(struct event_data_st* event_out)
{
    if (this->nextEvent == nullptr)
        return 1;

    *event_out = this->nextEvent->data;

    eventNode* tmp_ptr = this->nextEvent->next;
    this->nextEvent->~eventNode();
    this->nextEvent = tmp_ptr;
    tmp_ptr = nullptr;

    return 0;
}

/*
* Default Destructor, clears the FET and sets default parameters
* before being destroyed.
*/
FET::~FET()
{
    if (this->nextEvent == nullptr)
        return;

    eventNode* temp_ptr1 = this->nextEvent;
    eventNode* temp_ptr2 = temp_ptr1->next;
    this->nextEvent = nullptr;

    while (temp_ptr2 != nullptr) {
        temp_ptr1->~eventNode();
        temp_ptr1 = temp_ptr2;
        temp_ptr2 = temp_ptr1->next;        
    }

    temp_ptr1->~eventNode();
    temp_ptr1 = nullptr;
  
    return;
}

/*
* Creates an event node for the FET
*/
FET::eventNode::eventNode(struct event_data_st event_in)
{
    this->data = event_in;
    this->next = nullptr;
}

/*
* Destroys an event node
*/
FET::eventNode::~eventNode()
{
    this->data.agent_given = NoAgent;
    this->data.event_given = NoEvent;
    this->data.time = 0;
    this->next = nullptr;
}
