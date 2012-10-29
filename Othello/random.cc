#include "board.h"
#include "player.h"
#include "tournament.h"

using std::string;

// We put players in an anonymous namespace to avoid cross-player
// tampering
namespace {

  class RandomPlayer : public Player {
  public:
    RandomPlayer() : Player(), m_name("monkey with keyboard") {
    }

    virtual bool Move(const Othello::Board& b, uint32* row, uint32* col) {
      const BitBoard& valid = b.GetValidMoves();
      std::vector<std::pair<uint32, uint32>> moves;

      for (uint32 row = 0; row < 8; ++row) {
	for (uint32 col = 0; col < 8; ++col) {
	  if (valid[row][col]) {
	    moves.push_back(std::make_pair(row, col));
	  }
	}
      }
      assert(moves.size() > 0);
      uint32 move = 0xdeadbeef % moves.size();
      assert(move < moves.size());

      const std::pair<uint32, uint32> pair = moves[move];

      assert(pair.first < 8);
      assert(pair.second < 8);

      *row = pair.first;
      *col = pair.second;

      return false;
    }

    virtual const string& name() const {
      return this->m_name;
    }
  private:
    string m_name;
  };

  // Players must register themselves for the tournament, otherwise they aren't
  // discoverable due to the anonymous namespace
  REGISTER(RandomPlayer);

}  // namespace
