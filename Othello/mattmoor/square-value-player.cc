#include "board.h"
#include "player.h"
#include "tournament.h"
#include "mattmoor/value-board.h"

// We put players in an anonymous namespace to avoid cross-player
// tampering
namespace OTHELLO_GOD {

  class SquareValuePlayer : public mattmoor::ValuePlayer {
  public:
    SquareValuePlayer() : mattmoor::ValuePlayer("square value player") {
    }

    SquareValuePlayer(const char* brd)
      : mattmoor::ValuePlayer("square value player", brd) {
    }

    SquareValuePlayer(const uint32 (&brd)[8][8])
      : mattmoor::ValuePlayer("square value player", brd) {
    }

    virtual uint32 Value(const Othello::Board& b,
			 const std::pair<uint32, uint32>& pos) {
      return values()[pos.first][pos.second];
    }
  };

  // Players must register themselves for the tournament, otherwise they aren't
  // discoverable due to the anonymous namespace
#ifndef OTHELLO_GOD
  REGISTER(SquareValuePlayer);
#endif  // OTHELLO_GOD

}  // namespace
