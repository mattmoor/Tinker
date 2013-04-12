#include "mattmoor/value-board.h"
#include "tournament.h"

// We put players in an anonymous namespace to avoid cross-player
// tampering
namespace OTHELLO_GOD {

  class BoardValuePlayer : public mattmoor::ValuePlayer {
   public:
    BoardValuePlayer() : mattmoor::ValuePlayer("board value player") {
    }

    BoardValuePlayer(const char* brd)
      : mattmoor::ValuePlayer("board value player", brd) {
    }

    BoardValuePlayer(const uint32 (&brd)[8][8])
      : mattmoor::ValuePlayer("board value player", brd) {
    }

   protected:
    uint32 Value(const BitBoard& b) {
      uint32 value = 0;
      for (auto pos : b.SetBits()) {
	value += values()[pos.first][pos.second];
      }
      return value;
    }

    virtual uint32 Value(const Othello::Board& b,
			 const std::pair<uint32, uint32>& pos) {
      Othello::Board my_board(b);
      auto my_color = my_board.GetTurn();

      my_board.Move(pos.first, pos.second);

      return Value(my_board.GetColor(my_color));
    }
  };

  // Players must register themselves for the tournament, otherwise they aren't
  // discoverable due to the anonymous namespace
#ifndef OTHELLO_GOD
  REGISTER(BoardValuePlayer);
#endif  // OTHELLO_GOD

}  // namespace
