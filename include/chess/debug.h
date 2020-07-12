#pragma once

#include <memory>

#include "state.h"
#include "controller.h"

class DebugGameState
{
public:
	template<class... Args>
	DebugGameState(Args... args) :
		m_state(std::make_shared<GameState>(args...))
	{}
	
	std::shared_ptr<GameState> getState() const;

	DebugGameStateController getController();
private:
	std::shared_ptr<GameState> m_state;
};