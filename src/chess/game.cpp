#include "game.h"

#include "event.h"

Game::Game() :
	first_event(nullptr)
{}

Event* Game::getFirstEvent()
{
	return first_event;
}

Game::~Game()
{
	if (first_event)
		delete first_event;
}