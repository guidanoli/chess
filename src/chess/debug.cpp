#include "debug.h"

DebugGameStateController DebugGameState::getController()
{
	return m_state->getDebugController();
}

std::shared_ptr<GameState> DebugGameState::getState() const
{
	return m_state;
}