#include "event.h"

Event::Event(Event* previous = nullptr) :
	previous(previous),
	next(nullptr)
{
	if (previous != nullptr) {
		next = previous->next;
		previous->next = this;
	}
}

Event* Event::getPrevious() const
{
	return previous;
}

Event* Event::getNext() const
{
	return next;
}

bool Event::isFirst() const
{
	return previous == nullptr;
}

bool Event::isLast() const
{
	return next == nullptr;
}