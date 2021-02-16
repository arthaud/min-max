# Min-Max

A generic C++ implementation of the min-max algorithm for game theory.

This is provided as a single header file in `include/min-max/MinMax.h`.
This can be used for any 2-player game by implementing the `State` abstract class.
An example with the Tic Tac Toe game is provided in `source/TicTacToe.cpp`.

To play against the min-max AI:
```
$ make
$ ./tictactoe
```
