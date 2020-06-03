#pragma once

// An event is a chain of events,
// represented as a double-linked list
class Event
{
protected:
	// It is discouraged to add events in
	// the middle of event chains, just like
	// travelling back in time can alter the
	// future and things may not make sense
	Event(Event *previous = nullptr);

public:
	// Get previous and next events
	// If first, previous is nullptr
	// If last, next is nullptr
	Event* getPrevious() const;
	Event* getNext() const;

	// Check if event is first or last
	// in the chain of events
	bool isFirst() const;
	bool isLast() const;

	// One event is responsible for deleting all
	// successive ones, much like one event affects
	// all the upcoming ones
	~Event();
private:
	Event* previous;
	Event* next;
};