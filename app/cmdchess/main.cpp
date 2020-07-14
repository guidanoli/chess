#include <iostream>
#include <optional>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <map>

#include "state.h"
#include "listener.h"
#include "controller.h"
#include "types.h"
#include "event.h"
#include "error.h"
#include "board.h"

namespace fs = std::filesystem;

using namespace std;
using namespace chesslib;

// Helps initialize a map with a predefined set of (key, value) entries
template<typename T> struct map_init_helper
{
	T& data;
	map_init_helper(T& d) : data(d) {}
	map_init_helper& operator() (typename T::key_type const& key,
								 typename T::mapped_type const& value)
	{
		data[key] = value;
		return *this;
	}
};

// Get a map init helper on a map item
template<typename T> map_init_helper<T> map_init(T& item)
{
	return map_init_helper<T>(item);
}

// Maps error values to error messages
static map<GameError, string> error_message_map;

// Print game error
void print_error(GameError error);

// Implements GameListener, using the stdout to interact with the user
class CmdGameListener : public GameListener
{
	PieceTypeId promotePawn(GameController const& game, Square pawn) override;
	void catchError(GameController const& game, GameError error) override;
};

// Print whose turn is it in a game state
void print_turn(GameState const& game)
{
	cout << "It's the turn of the "
		 << (game.getTurn() == Colour::WHITE ? "white" : "black")
		 << " pieces" << endl;
}

// Maybe get square (nullopt if input is invalid)
optional<Square> maybe_get_square()
{
	int r_int;
	char f_char;
	cin >> f_char >> r_int;
	auto f = File(f_char - 'a');
	auto r = Rank(r_int - 1);
	if (!FileCheck(f) || !RankCheck(r))
		return nullopt;
	return getSquare(r, f);
}

// Maybe get move (nullopt if input is invalid)
optional<shared_ptr<Move>> maybe_get_move()
{
	cout << "Piece at... ";
	auto ini = maybe_get_square();
	if (!ini)
		return nullopt;
	cout << "To... ";
	auto fin = maybe_get_square();
	if (!fin)
		return nullopt;
	return make_shared<Move>(*ini, *fin);
}

// Maybe get castling event (nullopt if input is invalid)
optional<shared_ptr<Castling>> maybe_get_castling()
{
	cout << "Rook at... ";
	auto rook = maybe_get_square();
	if (!rook)
		return nullopt;
	return make_shared<Castling>(*rook);
}

// Maybe get piece type id (nullopt if input is invalid)
optional<PieceTypeId> maybe_get_piece_type_id()
{
	int opt;
	cout << "Piece type:" << endl;
	cout << "[0] Empty" << endl;
	cout << "[1] Pawn" << endl;
	cout << "[2] King" << endl;
	cout << "[3] Queen" << endl;
	cout << "[4] Bishop" << endl;
	cout << "[5] Knight" << endl;
	cout << "[6] Rook" << endl;
	cout << ">>> ";
	cin >> opt;
	auto piece_type_id = PieceTypeId(opt);
	if (!PieceTypeIdCheck(piece_type_id))
		return nullopt;
	return piece_type_id;
}

// Maybe get colour (nullopt if input is invalid)
optional<Colour> maybe_get_colour()
{
	int opt;
	cout << "Colour:" << endl;
	cout << "[0] White" << endl;
	cout << "[1] Black" << endl;
	cout << ">>> ";
	cin >> opt;
	auto colour = Colour(opt);
	if (!ColourCheck(colour))
		return nullopt;
	return colour;
}

// Maybe get boolean (nullopt if input is invalid)
optional<bool> maybe_get_boolean(string yes = "Yes",
                                 string no = "No")
{
	int opt;
	cout << "[0] " << no << endl;
	cout << "[1] " << yes << endl;
	cout << ">>> ";
	cin >> opt;
	if (opt < 0 || opt > 1)
		return nullopt;
	return opt == 1;
}

// Maybe get path (nullopt if input is invalid or if file doesn't exist or
// if it doesn't meet the 'is_directory' criterion)
optional<fs::path> maybe_get_path(bool is_directory)
{
	fs::path path;
	if (is_directory)
		cout << "directory = ";
	else
		cout << "file = ";
	cin >> path;
	if (!fs::exists(path))
		return nullopt;
	if (fs::is_directory(path) != is_directory)
		return nullopt;
	return path;
}

// Maybe save current game state to file obtained by user
// Returns:
// nullopt - Invalid input
// false   - Failed to save
// true    - Saved successfully
optional<bool> maybe_save_game(GameState const& g)
{
	auto path_opt = maybe_get_path(false);
	if (path_opt) {
		ofstream fs(*path_opt);
		g.save(fs);
		return bool(fs);
	} else {
		return nullopt;
	}
}

// Maybe save current game state to file obtained by user
// Returns:
// nullopt - Invalid input
// false   - Failed to save
// true    - Saved successfully
optional<bool> maybe_save_game(GameController const& gc)
{
	auto path_opt = maybe_get_path(false);
	if (path_opt) {
		ofstream fs(*path_opt);
		return gc.save(fs);
	} else {
		return nullopt;
	}
}

// Maybe loads game state from file obtained by user
// Returns:
// nullopt - Invalid input
// false   - Failed to load
// true    - Loaded successfully
optional<bool> maybe_load_game(GameState& g)
{
	auto path_opt = maybe_get_path(false);
	if (path_opt) {
		ifstream fs(*path_opt);
        try {
            g.load(fs);
        } catch (GameError err) {
            print_error(err);
        }
		return bool(fs);
	} else {
		return nullopt;
	}
}

// Maybe loads game state from file obtained by user
// Returns:
// nullopt - Invalid input
// false   - Failed to load
// true    - Loaded successfully
optional<bool> maybe_load_game(GameController& gc)
{
	auto path_opt = maybe_get_path(false);
	if (path_opt) {
		ifstream fs(*path_opt);
		return gc.load(fs);
	} else {
		return nullopt;
	}
}

template<class T>
T triStateToString(optional<bool> tristate, T _true, T _false, T _nullopt)
{
	if (tristate) {
		if (*tristate) {
			return _true;
		} else {
			return _false;
		}
	} else {
		return _nullopt;
	}
}

template<class T>
bool load_game(T& g)
{
	auto result = maybe_load_game(g);
	cout << triStateToString(result,
				             "Loaded successfully",
				             "Could not load file",
				             "Could not find file") << endl;
	if (result && *result == false)
		exit(1);
	return bool(*result);
}

template<class T>
bool save_game(T& g)
{
	auto result = maybe_save_game(g);
	cout << triStateToString(result,
				             "Saved successfully",
				             "Could not save state",
				             "Could not find file") << endl;
	return bool(result) && *result;
}

PieceTypeId CmdGameListener::promotePawn(GameController const& game, Square pawn)
{
	cout << "You may promote your pawn to a new type" << endl;
	auto piece_opt = maybe_get_piece_type_id();
	if (piece_opt)
		return *piece_opt;
	else
		return PieceTypeId::QUEEN;
}

void print_error(GameError error)
{
	auto error_message = error_message_map.find(error);
	if (error_message == error_message_map.end())
		cout << "Caught unknown error" << endl;
	else
		cout << "Error: " << error_message->second << endl;
}

void CmdGameListener::catchError(GameController const& game, GameError error)
{
	print_error(error);
}

int play(int argc, char** argv)
{
	auto gc = GameController(make_unique<GameState>(),
	                         make_shared<CmdGameListener>());
	auto const& g = gc.getState();
	while (true) {
		g.getBoard().pretty(cout);
		print_turn(g);
		while (true) {
			int opt;
			if (g.getPhase() != Phase::RUNNING)
				goto end;
			cout << "Choose an action:" << endl;
			cout << "[0] Move" << endl;
			cout << "[1] Castling" << endl;
			cout << "[8] Load" << endl;
			cout << "[9] Save" << endl;
			cout << ">>> ";
			cin >> opt;
			if (opt == 0) {
				auto move_opt = maybe_get_move();
				if (move_opt) {
					if (gc.update(*move_opt))
						break;
					else
						cout << "Invalid input" << endl;
				} else {
					cout << "Illegal input" << endl;
				}
			} else if (opt == 1) {
				auto castling_opt = maybe_get_castling();
				if (castling_opt) {
					if (gc.update(*castling_opt))
						break;
					else
						cout << "Invalid input" << endl;
				} else {
					cout << "Illegal input" << endl;
				}
			} else if (opt == 8) {
				if (load_game(gc))
					break;
			} else if (opt == 9) {
				if (save_game(gc))
					break;
			}
		}
	}
end:
	const auto phase = g.getPhase();
	if (phase == Phase::WHITE_WON)
		cout << "White won!" << endl;
	else if (phase == Phase::BLACK_WON)
		cout << "Black won!" << endl;

	return 0;
}

int create_game_state(int argc, char** argv)
{
	auto g = GameState();
	int opt;
	while(true) {
		g.getBoard().pretty(cout);
		cout << "Choose an action:" << endl;
		cout << "[0] Exit" << endl;
		cout << "[1] Save" << endl;
		cout << "[2] Load" << endl;
		cout << "[3] Edit square" << endl;
		cout << "[4] Next turn" << endl;
		cout << "[5] Clear board" << endl;
		cout << "[6] Alter square" << endl;
		cout << ">>> ";
		cin >> opt;
		if (opt == 0) {
			return 0;
		} else if (opt == 1) {
			if (load_game(g))
				break;
		} else if (opt == 2) {
			if (save_game(g))
				break;
		} else if (opt == 3) {
			cout << "square = ";
			auto sq_opt = maybe_get_square();
			if (sq_opt) {
				auto piece_type_id_opt = maybe_get_piece_type_id();
				if (!piece_type_id_opt) {
					cout << "Illegal piece type!" << endl;
					continue;
				}
				if (*piece_type_id_opt == PieceTypeId::NONE) {
					g.clearSquare(*sq_opt);
					continue;
				}
				auto colour_opt = maybe_get_colour();
				if (!colour_opt) {
					cout << "Illegal colour!" << endl;
					continue;
				}
				auto& board_piece = g.getPieceAt(*sq_opt);
				board_piece.setType(*piece_type_id_opt);
			    board_piece.setColour(*colour_opt);
				g.clearEnPassantPawn();
			} else {
				cout << "Illegal square!" << endl;
			}
		} else if (opt == 4) {
			g.nextTurn();
			print_turn(g);
		} else if (opt == 5) {
			for (Square sq = SQ_A1; sq < SQ_CNT; ++sq)
				g.clearSquare(sq);
		} else if (opt == 6) {
			auto sq_opt = maybe_get_square();
			if (sq_opt) {
				auto altered_opt = maybe_get_boolean("Altered", "Unaltered");
				if (altered_opt)
					g.setSquareAltered(*sq_opt, *altered_opt);
				else
					cout << "Illegal boolean!" << endl;
			} else {
				cout << "Illegal square!" << endl;
			}
		} else {
			return 1;
		}
	}
    return 0;
}

void init_error_message_map()
{
	map_init(error_message_map)
		(GameError::ILLEGAL_PROMOTION, "Illegal promotion")
		(GameError::IO_COLOUR, "Illegal colour")
		(GameError::IO_EN_PASSANT, "Illegal en passant")
		(GameError::IO_PIECE_TYPE, "Illegal piece type")
		(GameError::IO_SQUARE, "Illegal square")
		(GameError::IO_TURN, "Illegal turn")
		(GameError::IO_VERSION, "Illegal version");
}

int main(int argc, char** argv)
{
	init_error_message_map();

	int opt;
	cout << "Choose a subprogram:" << endl;
	cout << "[0] Play" << endl;
	cout << "[1] Create game state" << endl;
	cout << ">>> ";
	cin >> opt;
	if (opt == 0)
		return play(argc, argv);
	else if (opt == 1)
		return create_game_state(argc, argv);
	else
		return 1;
}
