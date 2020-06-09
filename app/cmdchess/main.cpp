#include <iostream>
#include <optional>

#include "game.h"
#include "event.h"

using namespace std;

void print_piece(Piece p) {
	char c = p.getColour() == Colour::WHITE ? 0 : ('A' - 'a');
	switch (*p.getType()) {
	case PieceTypeId::NONE:
		cout << "_";
		return;
	case PieceTypeId::PAWN:
		c += 'p';
		break;
	case PieceTypeId::KING:
		c += 'k';
		break;
	case PieceTypeId::QUEEN:
		c += 'q';
		break;
	case PieceTypeId::BISHOP:
		c += 'b';
		break;
	case PieceTypeId::KNIGHT:
		c += 'n';
		break;
	case PieceTypeId::ROOK:
		c += 'r';
		break;
	default:
		cout << "?";
		return;
	}
	cout << c;
}

void print_chess_board(Board const& board)
{
	cout << "    ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		cout << fc << " ";
	cout << endl << "   _";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		cout << "__";
	cout << endl;
	for (Rank r = RK_8; r >= RK_1; --r) {
		cout << r - RK_1 + 1 << " | ";
		for (File f = FL_A; f <= FL_H; ++f) {
			Square sq = getSquare(r, f);
			Piece p = board[sq];
			print_piece(p);
			cout << " ";
		}
		cout << "|" << endl;
	}
	cout << "   ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		cout << "--";
	cout << "-" << endl;
}

void print_turn(Game const& game)
{
	cout << "It's the turn of the "
		 << (game.getTurn() == Colour::WHITE ? "white" : "black")
		 << " pieces" << endl;
}

std::optional<Move> getMove()
{
	int initial_r, final_r;
	char initial_f, final_f;
	cout << "Piece at... ";
	cin >> initial_f >> initial_r;
	if (!FileCheck(File(initial_f - 'a')) ||
		!RankCheck(Rank(initial_r - 1)))
		return std::nullopt;
	cout << "To... ";
	cin >> final_f >> final_r;
	if (!FileCheck(File(final_f - 'a')) ||
		!RankCheck(Rank(final_r - 1)))
		return std::nullopt;
	Square ini = getSquare(Rank(initial_r - 1), File(initial_f - 'a'));
	Square fin = getSquare(Rank(final_r - 1), File(final_f - 'a'));
	return Move(ini, fin);
}

int main(int argc, char** argv)
{
	Game g;
	while (g.getPhase() == Phase::RUNNING) {
		print_chess_board(g.getBoard());
		print_turn(g);
		while (true) {
			int opt;
			cout << "[0] Move" << endl;
			cout << "[8] Load" << endl;
			cout << "[9] Save" << endl;
			cout << ">>> ";
			cin >> opt;
			if (opt == 0) {
				auto move_opt = getMove();
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
				string ifile;
				cout << "file = ";
				cin >> ifile;
				std::ifstream fs(ifile);
				fs >> g;
				if (fs) {
					cout << "Loaded!" << endl;
					print_chess_board(g.getBoard());
					print_turn(g);
				} else {
					cout << "Error!" << endl;
					return 1;
				}
			} else if (opt == 9) {
				std::ofstream fs("save.dat");
				fs << g;
				if (fs)
					cout << "Saved!" << endl;
				else
					cout << "Error!" << endl;
			}
		}
	}
	return 0;
}