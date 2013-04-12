#include "mattmoor/value-board.h"
#include "tournament.h"

#include <vector>
#include <unordered_map>


#define ALPHA_BETA_PRUNING
#define CACHING

// We put players in an anonymous namespace to avoid cross-player
// tampering
namespace OTHELLO_GOD {

  const uint32 GOAL = 8;

  typedef std::pair<uint32, uint32> BoardValue;
  typedef std::pair<uint64, uint64> BoardState;
#define BLACK first
#define WHITE second
#define ROW first
#define COLUMN second

  struct BoardStateHash {
    size_t operator()(const BoardState& key) const {
      uint64 black = key.BLACK;
      uint64 white = key.WHITE;
      
      white = (white << 32) | (white >> 32);
      return (black ^ white);
    }
  };

#if defined(CACHING)  
  typedef std::unordered_map<BoardState, BoardValue, BoardStateHash> Cache;
#else
  typedef int Cache;
#endif  // defined(CACHING)


#if defined(ALPHA_BETA_PRUNING)
#define ALPHA std::make_pair(0, 0xFFFFFFFF)
#define BETA  std::make_pair(0xFFFFFFFF, 0)

  typedef BoardValue AlphaBeta;

  bool operator<=(const AlphaBeta& lhs, const AlphaBeta& rhs) {
    int64 ldiff = ((int64)lhs.BLACK) - ((int64)lhs.WHITE);
    int64 rdiff = ((int64)rhs.BLACK) - ((int64)rhs.WHITE);

    return ldiff <= rdiff;
  }
#else
// When this isn't enabled, use another type to ensure there aren't
// incompatibilities
#define ALPHA 0
#define BETA  0

  typedef int AlphaBeta;
#endif  // defined(ALPHA_BETA_PRUNING)

  class DepthValuePlayer : public mattmoor::ValuePlayer {
   public:
#define STD_INIT() \
    goal_(GOAL), caches_(64+GOAL), played_(70)

    DepthValuePlayer() : mattmoor::ValuePlayer("depth value player"),
			 STD_INIT() {
    }

    DepthValuePlayer(const char* brd)
      : mattmoor::ValuePlayer("depth value player", brd),
	STD_INIT() {
    }

    DepthValuePlayer(const uint32 (&brd)[8][8])
      : mattmoor::ValuePlayer("depth value player", brd),
	STD_INIT() {
    }

   protected:
    // When we reach the goal depth, we return this as the value of
    // the board we reached.
    uint32 Value(const BitBoard& b) {
      uint32 value = 0;
      for (auto pos : b.SetBits()) {
	value += values()[pos.ROW][pos.COLUMN];
      }
      return value;
    }

    // Use templates to achieve a function clone to make all predication on
    // color foldable.
    template<Othello::Color color>
    void DepthValue(const Othello::Board& b,
		    const std::pair<uint32, uint32>& pos,
		    uint32 depth, Cache& cache,
		    BoardValue* board_value,
		    const AlphaBeta& alpha_,
		    const AlphaBeta& beta_) {
      if (b.GetTurn() != color) {
	return OtherDepthValue<color>(b, pos, depth, cache, board_value, alpha_, beta_);
      }

      Othello::Board my_board(b);
      my_board.Move(pos.ROW, pos.COLUMN);

      const auto& black_pos = my_board.GetColor(Othello::Black);
      const auto& white_pos = my_board.GetColor(Othello::White);
      auto key = std::make_pair((uint64)black_pos, (uint64)white_pos);

#if defined(CACHING)
      // Check the cache for this depth for the key.
      if (cache.count(key) == 1) {
	*board_value = cache[key];
	return;
      }
#endif  // defined(CACHING)

      // Check for victory (no moves) before doing the depth-limited
      // value approximation, since it will undervalue the victory
      // condition.
      if (my_board.game_over()) {
	BoardValue final_score;
	my_board.Score(&final_score.BLACK, &final_score.WHITE);

	if (final_score.BLACK > final_score.WHITE) {
	  // Victory for black is +INFINITY for black and
	  // zero for white.
	  board_value->BLACK = 0xFFFFFFFF;
	  board_value->WHITE = 0;
	} else if (final_score.BLACK < final_score.WHITE) {
	  // vice versa for white victory.
	  board_value->BLACK = 0;
	  board_value->WHITE = 0xFFFFFFFF;
	} else {
	  // Tie is a worthless state.
	  board_value->BLACK = 0;
	  board_value->WHITE = 0;
	}
#if defined(CACHING)
	// Cache the victory condition.
	cache[key] = *board_value;
#endif  // defined(CACHING)
	return;
      }

      if (depth == 0) {
	// Our base case.  When we've hit the target depth of this
	// iteration, return the value of the board we've reached.
	board_value->BLACK = Value(black_pos);
	board_value->WHITE = Value(white_pos);
#if defined(CACHING)
	cache[key] = *board_value;
#endif  // defined(CACHING)
	return;
      }

      // TODO(mattmoor): We should have all of the data to perform a
      // negamax search instead of minimax with alpha-beta pruning.

      const BitBoard& moves = my_board.GetValidMoves();

      // TODO(mattmoor): Killer heuristic
      // NOTE: consider caching the prior "killer" move when iteratively
      // deepening
      //
      // From Wikipedia:
      //   The killer heuristic attempts to produce a cutoff by assuming that a
      //   move that produced a cutoff in another branch of the game tree at the
      //   same depth is likely to produce a cutoff in the present position,
      //   that is to say that a move that was a very good move from a different
      //   (but possibly similar) position might also be a good move in the
      //   present position. By trying the killer move before other moves, a
      //   game playing program can often produce an early cutoff, saving itself
      //   the effort of considering or even generating all legal moves from a
      //   position.

      // TODO(mattmoor): Clear the bit for the killer move, so the loop doesn't
      // visit it.

      // From Wikipedia:
      //   An improved sort at any depth will exponentially reduce the total
      //   number of positions searched, but sorting all positions at depths
      //   near the root node is relatively cheap as there are so few of them.
      //   In practice, the move ordering is often determined by the results of
      //   earlier, smaller searches, such as through iterative deepening.

      auto set_bits = moves.SetBits();

      std::vector<Position> vmoves;
      for (const Position& pos : set_bits) {
	vmoves.push_back(pos);
      }

      // TODO(mattmoor): Sort the moves here based on knowledge from previous
      // depths.
      // NOTE: Any move not in the prior level's cache was alpha-beta pruned.
      //
      // std::sort(vmoves.begin(), vmoves.end(), ???);

      // Create mutable copies of the values of alpha and beta for local
      // mutation.
      AlphaBeta alpha = alpha_;
      AlphaBeta beta = beta_;

      // Otherwise, continue diving until we have reached the target depth.
      for (const auto& next_pos : vmoves) {
	BoardValue next_value;
	OtherDepthValue<color>(my_board, next_pos, depth - 1, cache, &next_value, alpha, beta);

	// For the OTHER player's move, they would optimize THEIR color
	uint32 value = (color == Othello::Black) ?
	  next_value.WHITE : next_value.BLACK;
	uint32 best_value = (color == Othello::Black) ?
	  board_value->WHITE : board_value->BLACK;

	// Keep the maximum value piece
	if (value > best_value) {
	  value = best_value;
	  *board_value = next_value;
	}

#if defined(ALPHA_BETA_PRUNING)
	// Perform alpha-beta pruning
	if (color == Othello::Black) {
	  ABMax(&alpha, next_value);
	} else {
	  ABMin(&beta, next_value);
	}
	// Don't both pushing beta lower or alpha higher if beta is already less than alpha
	if (beta <= alpha) {
#if defined(CACHING)
	  // TODO(mattmoor): Cache the move that resulted in the prune in a way that we will
	  // recover it the next time we go deeper.
#endif  // defined(CACHING)
	  break;
	}
#endif  // defined(ALPHA_BETA_PRUNING)
      }

#if defined(CACHING)
      // Put the computed result in the cache
      cache[key] = *board_value;
#endif  // defined(CACHING)
    }

#if defined(ALPHA_BETA_PRUNING)    
    void ABMax(AlphaBeta* alpha, const AlphaBeta& value) {
      if ((*alpha) <= value) {
	// alpha keeps the max
	//std::cout << "New alpha (" << value.first << " , " << value.second << ")" << std::endl;
	*alpha = value;
      }
    }

    void ABMin(AlphaBeta* beta, const AlphaBeta& value) {
      if ((*beta) <= value) {
	// Beta keeps the min
      } else {
	//std::cout << "New beta (" << value.first << " , " << value.second << ")" << std::endl;
	*beta = value;
      }
    }
#endif  // defined(ALPHA_BETA_PRUNING)

    // We use template specialization of this function to avoid specializing
    // the above function just for the recursion.
    template <Othello::Color color>
    void OtherDepthValue(const Othello::Board& b,
			 const std::pair<uint32, uint32>& pos,
			 uint32 depth, Cache& cache,
			 BoardValue* board_value,
			 const AlphaBeta& alpha_,
			 const AlphaBeta& beta_);

    virtual uint32 Value(const Othello::Board& b,
			 const std::pair<uint32, uint32>& pos) {
      BoardValue board_value;
      const Othello::Color c = b.GetTurn();

      uint32 previous_size = 20;
      for (uint32 depth = 0; depth < goal_; ++depth) {
	// Ensure our cache is bigger than the one before it to avoid rehashing
	Cache& cache = caches_[played_ + depth + 1];
#if defined(CACHING)
	cache.reserve(2 * previous_size);
#endif  // defined(CACHING)

	// Seed the values for our alpha-beta pruning.
	AlphaBeta alpha = ALPHA;
	AlphaBeta beta = BETA;

	if (c == Othello::Black) {
	  DepthValue<Othello::Black>(b, pos, depth, cache, &board_value, alpha, beta);
	} else {
	  DepthValue<Othello::White>(b, pos, depth, cache, &board_value, alpha, beta);
	}

#if defined(CACHING)
	previous_size = cache.size();
#endif  // defined(CACHING)
      }
      return (c == Othello::Black) ? board_value.BLACK : board_value.WHITE;
    }

    virtual bool Move(const Othello::Board& b, uint32* row, uint32* col) {
      // Check how many moves have been made since our last move.
      uint32 moves = 0; b.Score(&moves, &moves);

#if defined(CACHING)
      if (played_ > moves) {
	// This is a new game, reset all of our caches.
	for (auto& cache : caches_) {
	  cache.clear();
	  cache.reserve(0);
	}
      } else {
	// At least our move must have happened
	assert(played_ != moves);
	// Clear the caches for piece counts we will never see again
	for (uint32 i = played_ + 1; i <= moves; ++i) {
	  caches_[i].clear();
	  caches_[i].reserve(0);
	}
      }
#endif  // defined(CACHING)
      played_ = moves;

      //std::cout << b;

      bool pass = ValuePlayer::Move(b, row, col);

//       for (uint32 i = played_; i <= (played_ + goal_ + 1); ++i) {
// 	Cache& cache = caches_[i];
// 	std::cout << "Cache: " << cache.bucket_count() << std::endl;
//       }
//       std::cout << std::endl;

      return pass;
    }

  private:
    uint32 goal_;
    uint32 played_;
    std::vector<Cache> caches_;
  };

  template<>
  void DepthValuePlayer::OtherDepthValue<Othello::Black>(const Othello::Board& b,
							 const std::pair<uint32, uint32>& pos,
							 uint32 depth, Cache& cache,
							 BoardValue* board_value,
							 const AlphaBeta& alpha_,
							 const AlphaBeta& beta_) {
    return DepthValue<Othello::White>(b, pos, depth, cache, board_value, alpha_, beta_);
  }

  template<>
  void DepthValuePlayer::OtherDepthValue<Othello::White>(const Othello::Board& b,
							 const std::pair<uint32, uint32>& pos,
							 uint32 depth, Cache& cache,
							 BoardValue* board_value,
							 const AlphaBeta& alpha_,
							 const AlphaBeta& beta_) {
    return DepthValue<Othello::Black>(b, pos, depth, cache, board_value, alpha_, beta_);
  }

  // Players must register themselves for the tournament, otherwise they aren't
  // discoverable due to the anonymous namespace
#ifndef OTHELLO_GOD
  REGISTER(DepthValuePlayer);
#endif  // OTHELLO_GOD

}  // namespace
