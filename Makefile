all: tictactoe

tictactoe: source/TicTacToe.cpp include/min-max/MinMax.h
	c++ -o $@ -std=c++17 -Iinclude source/TicTacToe.cpp

clean:
	rm -f tictactoe
