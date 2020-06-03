#pragma once

class Event;

// A game is represented as a chain of events (see the Event class)
// and is (de)serialized as such too.
class Game
{
public:
	// Starts a new game
	Game();

	// Get first event from chain of events
	Event* getFirstEvent();

	// Deletes all the events
	~Game();
private:
	Event* first_event;
};