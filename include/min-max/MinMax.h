#pragma once

#include <cassert>

#include <optional>
#include <vector>

namespace min_max {

using Player = char;

static constexpr Player Empty = 0;
static constexpr Player You = 1;
static constexpr Player Them = 2;

template < typename Move, typename Score, typename Derived >
class State {
public:
  using MoveT = Move;
  using ScoreT = Score;

public:
  explicit State(Player turn) : turn_(turn) {}

  State(const State&) = default;

  State(State&&) = default;

  State& operator=(const State&) = default;

  State& operator=(State&&) = default;

  virtual ~State() = default;

  /* Return true if the game ended, someone won. */
  virtual bool is_terminal() const = 0;

  /**
   * Return your score.
   * Return `std::numeric_limits<Score>::max()` if you win
   * Return `std::numeric_limits<Score>::min()` if you lost.
   */
  virtual Score score() const = 0;

  /* Return all possible valid moves for the current turn. */
  virtual std::vector< std::pair< Move, Derived > > moves() const = 0;

  /* Return who's turn it is. */
  Player turn() const { return turn_; }

  Player next_turn() const { return turn_ == You ? Them : You; }

private:
  Player turn_;
};

template < typename State >
class MinMax {
public:
  using MoveT = typename State::MoveT;
  using ScoreT = typename State::ScoreT;

  struct Solution {
    std::optional< MoveT > move;
    ScoreT score;
  };

  /* Compute the best move and score for the given depth using min-max. */
  static Solution min_max(State state, unsigned depth) {
    bool is_you = state.turn() == You;
    return min_max(std::move(state),
                   depth,
                   /* alpha */ std::numeric_limits< ScoreT >::min(),
                   /* beta */ std::numeric_limits< ScoreT >::max(),
                   is_you);
  }

  static Solution min_max(
      State state, unsigned depth, ScoreT alpha, ScoreT beta, bool is_you) {
    if (depth == 0 || state.is_terminal()) {
      return {std::nullopt, state.score()};
    } else if (is_you) {
      Solution solution = {std::nullopt, std::numeric_limits< ScoreT >::min()};
      for (auto&& [move, new_state] : state.moves()) {
        auto new_solution = min_max(std::move(new_state),
                                    depth - 1,
                                    alpha,
                                    beta,
                                    /* is_you */ false);
        if (new_solution.score > solution.score) {
          solution.move = move;
          solution.score = new_solution.score;
        }
        if (solution.score > alpha) {
          alpha = solution.score;
        }
        if (alpha >= beta) {
          return solution;
        }
      }
      return solution;
    } else {
      Solution solution = {std::nullopt, std::numeric_limits< ScoreT >::max()};
      for (auto&& [move, new_state] : state.moves()) {
        auto new_solution = min_max(std::move(new_state),
                                    depth - 1,
                                    alpha,
                                    beta,
                                    /* is_you */ true);
        if (new_solution.score < solution.score) {
          solution.move = move;
          solution.score = new_solution.score;
        }
        if (solution.score < beta) {
          beta = solution.score;
        }
        if (beta <= alpha) {
          return solution;
        }
      }
      return solution;
    }
  }
};

} // namespace min_max
