#pragma once

#include <memory>
#include <iosfwd>
#include <functional>

#include "error.h"
#include "types.h"

class GameEvent;
class GameState;
class GameListener;

class GameController
{
public:
	GameController(std::unique_ptr<GameState> gameStatePtr,
	               std::shared_ptr<GameListener> listener);
	GameController(GameController const& other);
	GameState const& getState() const;
	bool update(std::shared_ptr<GameEvent> event);
	bool loadState(std::istream& is);
	bool saveState(std::ostream& os) const;
private:
	void clearListener();
	bool inCheck(Colour c) const;
	Square getKingSquare(Colour c) const;
	void lookForPromotion();
	void lookForCheckmate();
	bool canUpdate(std::shared_ptr<GameEvent> e) const;
	bool wouldEventCauseCheck(std::shared_ptr<GameEvent> e) const;
	void raiseError(GameError err) const;
	using simulationCallback = std::function<bool(GameController&)>;
	bool simulate(simulationCallback cb) const;
private:
	std::unique_ptr<GameState> m_state;
	std::shared_ptr<GameListener> m_listener;
};