Event-based game
================

Instead of representing a game by a matrix of tiles, I have chosen to represent it as
a chain of events. Every displacement of pieces is equivalent to an event. These are
the events that will have to be implemented:

Move
----

Every move involves a piece of a certain type and colour being moved from an initial
position to a final position. These obviously cannot be equal, since it would violate
the definition of an event.

Promotion
---------

When a pawn reaches the eight rank, it can be promoted to any other piece type with
the exception of the king. This event succeeds the move of the pawn.

Castling
--------

If a king and a rook have never been moved, and there is no pieces between then,
and neither is the king in check before nor after a castling, then castling can occurr.
It has to be specified which rook will take part of the action, since the turn and the
king can be guessed since turns alternate and each player contains only one king.

BoardView
=========

It can be hard to visualize the current game state merely by listing the events, so
implementing a class that executes these events in a matrix of tiles is handy not only
for computational reasons (imagine having to query 64 times the piece that is or not in
a given tile, running through all the events that have already happened), but also for
debugging purposes.