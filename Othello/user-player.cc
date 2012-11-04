#include "board.h"
#include "player.h"
#include "tournament.h"

using std::string;

// We put players in an anonymous namespace to avoid cross-player
// tampering
namespace {

  class RealPlayer : public Player {
  public:
    RealPlayer() : Player(), m_name("User") {
    }

    virtual bool Move(const Othello::Board& b, uint32* row_, uint32* col_) {
      while(true) {
	uint32 black, white;
	bool wtf = !b.Score(&black, &white);
	assert(wtf);

	// Display the score and board
	std::cout << "Black: " << black
		  << ", White: " << white 
		  << std::endl << b;

	uint32& row = *row_ = 0;
	uint32& col = *col_ = 0;
	std::cin >> row >> col;

	if ((row < 8) && (col < 8)) {
	  if (b.GetValidMoves()[row][col]) {
	    return false;
	  } else {
	    std::cout << "INVALID MOVE!" << std::endl;
	  }
	} else {
	  // Treat out of bounds as an exit condition...
	  exit(1);
	}
      }
    }

    virtual const string& name() const {
      return this->m_name;
    }
  private:
    string m_name;
  };

  // Players must register themselves for the tournament, otherwise they aren't
  // discoverable due to the anonymous namespace
  REGISTER(RealPlayer);

}  // namespace
