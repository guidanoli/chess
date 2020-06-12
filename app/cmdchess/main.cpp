#include <iostream>
#include <optional>
#include <cstdlib>

#include "game.h"
#include "event.h"

using namespace std;

class CmdGameListener : public GameListener
{
	PieceTypeId promotePawn(Game const& game, Square pawn) override;
};

void print_turn(Game const& game)
{
	cout << "It's the turn of the "
		 << (game.getTurn() == Colour::WHITE ? "white" : "black")
		 << " pieces" << endl;
}

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

optional<Move> maybe_get_move()
{
	cout << "Piece at... ";
	auto ini = maybe_get_square();
	if (!ini)
		return nullopt;
	cout << "To... ";
	auto fin = maybe_get_square();
	if (!fin)
		return nullopt;
	return Move(*ini, *fin);
}

optional<PieceTypeId> maybe_get_piece_type_id() {
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

optional<Colour> maybe_get_colour() {
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

void save_game(Game const& g) {
	string ofile;
	cout << "file = ";
	cin >> ofile;
	ofstream fs(ofile);
	fs << g;
	if (fs)
		cout << "Saved!" << endl;
	else
		cout << "Error!" << endl;
}

void load_game(Game& g) {
	string ifile;
	cout << "file = ";
	cin >> ifile;
	ifstream fs(ifile);
	fs >> g;
	if (fs) {
		cout << "Loaded!" << endl;
		g.pretty();
		print_turn(g);
	} else {
		cout << "Error!" << endl;
		exit(1);
	}
}

PieceTypeId CmdGameListener::promotePawn(Game const& game, Square pawn)
{
	cout << "You may promote your pawn to a new type" << endl;
	auto piece_opt = maybe_get_piece_type_id();
	if (piece_opt)
		return *piece_opt;
	else
		return PieceTypeId::QUEEN;
}

int play(int argc, char** argv) {
	auto g = Game(std::make_shared<CmdGameListener>());
	while (true) {
		g.pretty();
		print_turn(g);
		while (true) {
			int opt;
			if (g.getPhase() != Phase::RUNNING)
				goto end;
			cout << "Choose an action:" << endl;
			cout << "[0] Move" << endl;
			cout << "[8] Load" << endl;
			cout << "[9] Save" << endl;
			cout << ">>> ";
			cin >> opt;
			if (opt == 0) {
				auto move_opt = maybe_get_move();
				if (move_opt) {
					auto move = *move_opt;
					if (g.update(&move))
						break;
					else
						cout << "Invalid input" << endl;
				} else {
					cout << "Illegal input" << endl;
				}
			} else if (opt == 8) {
				load_game(g);
			} else if (opt == 9) {
				save_game(g);
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
	auto g = Game(std::make_shared<CmdGameListener>());
	int opt;
	while(true) {
		g.pretty();
		cout << "Choose an action:" << endl;
		cout << "[0] Exit" << endl;
		cout << "[1] Save" << endl;
		cout << "[2] Load" << endl;
		cout << "[3] Edit square" << endl;
		cout << "[4] Next turn" << endl;
		cout << "[5] Clear board" << endl;
		cout << ">>> ";
		cin >> opt;
		if (opt == 0) {
			return 0;
		} else if (opt == 1) {
			save_game(g);
		} else if (opt == 2) {
			load_game(g);
		} else if (opt == 3) {
			cout << "square = ";
			auto sq = maybe_get_square();
			if (sq) {
				auto piece_type_id_opt = maybe_get_piece_type_id();
				if (!piece_type_id_opt) {
					cout << "Illegal piece type!" << endl;
					continue;
				}
				if (*piece_type_id_opt == PieceTypeId::NONE) {
					g.getBoard()[*sq].clear();
					continue;
				}
				auto colour_opt = maybe_get_colour();
				if (!colour_opt) {
					cout << "Illegal colour!" << endl;
					continue;
				}
				auto piece_type = getPieceTypeById(*piece_type_id_opt);
				g.getBoard()[*sq] = Piece(piece_type, *colour_opt);
				g.setEnPassantPawn(EnPassantPawn::NONE);
			} else {
				cout << "Illegal square!" << endl;
			}
		} else if (opt == 4) {
			g.nextTurn();
			print_turn(g);
		} else if (opt == 5) {
			auto& board = g.getBoard();
			for (Square sq = SQ_A1; sq < SQ_CNT; ++sq)
				board[sq].clear();
		} else {
			return 1;
		}
	}
}

int main(int argc, char** argv)
{
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