#include <array>
#include <iostream>

#include <min-max/MinMax.h>

using Player = min_max::Player; // char
using Move = std::pair< int, int >;
using Score = int;

class TicTacToeState final
    : public min_max::State< Move, Score, TicTacToeState > {
public:
  // Initial state.
  TicTacToeState() : State(min_max::You), winner_(min_max::Empty) {
    for (int x = 0; x <= 2; x++) {
      for (int y = 0; y <= 2; y++) {
        grid_[x][y] = min_max::Empty;
      }
    }
  }

  explicit TicTacToeState(Player turn,
                          std::array< std::array< Player, 3 >, 3 > grid)
      : State(turn), winner_(min_max::Empty), grid_(grid) {
    winner_ = compute_winner();
  }

private:
  Player compute_winner() const {
    for (int i = 0; i <= 2; i++) {
      // All rows
      if (grid_[i][0] == grid_[i][1] && grid_[i][1] == grid_[i][2] &&
          grid_[i][0] != min_max::Empty) {
        return grid_[i][0];
      }
      // All columns
      if (grid_[0][i] == grid_[1][i] && grid_[1][i] == grid_[2][i] &&
          grid_[0][i] != min_max::Empty) {
        return grid_[0][i];
      }
    }
    // Diagonal
    if (grid_[0][0] == grid_[1][1] && grid_[1][1] == grid_[2][2] &&
        grid_[1][1] != min_max::Empty) {
      return grid_[1][1];
    }
    if (grid_[2][0] == grid_[1][1] && grid_[1][1] == grid_[0][2] &&
        grid_[1][1] != min_max::Empty) {
      return grid_[1][1];
    }
    return min_max::Empty;
  }

public:
  bool is_terminal() const override {
    if (winner_ != min_max::Empty) {
      return true;
    }
    for (int x = 0; x <= 2; x++) {
      for (int y = 0; y <= 2; y++) {
        if (grid_[x][y] == min_max::Empty) {
          return false;
        }
      }
    }
    return true;
  }

  int score() const override {
    switch (winner_) {
      case min_max::You:
        return std::numeric_limits< Score >::max();
      case min_max::Them:
        return std::numeric_limits< Score >::min();
    }

    return 0; // TODO: better score function
  }

  std::vector< std::pair< Move, TicTacToeState > > moves() const override {
    std::vector< std::pair< Move, TicTacToeState > > result;
    for (int x = 0; x <= 2; x++) {
      for (int y = 0; y <= 2; y++) {
        if (grid_[x][y] == min_max::Empty) {
          auto grid = grid_;
          grid[x][y] = turn();
          result.push_back({Move{x, y}, TicTacToeState(next_turn(), grid)});
        }
      }
    }
    return result;
  }

  friend std::ostream& operator<<(std::ostream& out,
                                  const TicTacToeState& state) {
    out << "+---+\n";
    for (int x = 0; x <= 2; x++) {
      out << "|";
      for (int y = 0; y <= 2; y++) {
        switch (state.grid_[x][y]) {
          case min_max::Empty:
            out << " ";
            break;
          case min_max::You:
            out << "X";
            break;
          case min_max::Them:
            out << "O";
            break;
          default:
            assert(false && "unexpected");
        }
      }
      out << "|\n";
    }
    out << "+---+";
    return out;
  }

  std::optional< TicTacToeState > play(Move move) const {
    if (move.first < 0 || move.first > 2 || move.second < 0 ||
        move.second > 2) {
      // Invalid move
      return std::nullopt;
    }
    if (grid_[move.first][move.second] != min_max::Empty) {
      // Invalid move
      return std::nullopt;
    }

    auto grid = grid_;
    grid[move.first][move.second] = turn();
    return TicTacToeState(next_turn(), grid);
  }

  Player winner() const { return winner_; }

private:
  Player winner_;
  std::array< std::array< Player, 3 >, 3 > grid_;
};

using TicTacToeMinMax = min_max::MinMax< TicTacToeState >;

int main() {
  std::cout << "Welcome to the tic-tac-toe AI.\n";
  std::cout << "You are player X, the bot is O.\n";
  TicTacToeState state;
  while (!state.is_terminal()) {
    // Player turn.
    std::cout << state << "\n";
    std::cout << "Your move (row, column) : " << std::flush;
    int x = 0, y = 0;
    std::cin >> x >> y;
    std::cout << "You are playing (" << x << ", " << y << ")" << std::endl;
    auto new_state = state.play(Move{x, y});
    if (!new_state) {
      std::cout << "Invalid move." << std::endl;
      continue;
    }
    state = *new_state;

    if (state.is_terminal()) {
      break;
    }

    // AI turn.
    std::cout << "Computing AI move...\n";
    std::optional< Move > move;
    for (int depth = 1; depth <= 10; depth++) {
      auto solution = TicTacToeMinMax::min_max(state, depth);
      assert(solution.move);
      std::cout << "Depth " << depth << ": best move (" << solution.move->first
                << ", " << solution.move->second << ")\n";
      move = *solution.move;
    }
    assert(move);
    new_state = state.play(*move);
    if (!new_state) {
      std::cout << "AI move is invalid!" << std::endl;
      return 1;
    }
    state = *new_state;
  }
  if (state.winner() == min_max::You) {
    std::cout << "You won, gg!" << std::endl;
  } else if (state.winner() == min_max::Them) {
    std::cout << state << "\n";
    std::cout << "You lost, gg!" << std::endl;
  } else {
    std::cout << state << "\n";
    std::cout << "This is a draw :(" << std::endl;
  }
  return 0;
}
