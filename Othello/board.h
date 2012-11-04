#ifndef _BOARD_H_
#define _BOARD_H_

#include "bit-board.h"

namespace Othello {

// TODO(mattmoor): iterator over next moves

typedef bool Color;
const Color Black = true;
const Color White = false;

class Board {
 public:
  Board() {
    // Explicitly clear the contents of these bit-boards
    m_color[White] = 0;
    m_color[Black] = 0;
    m_next_moves = 0;

    // Set the initial black squares
    m_color[Black][3][3] = true;
    m_color[Black][4][4] = true;

    // Set the initial White squares
    m_color[White][3][4] = true;
    m_color[White][4][3] = true;

    // Set the initial turn to Black
    m_turn = Black;

    this->BuildNextMoves();
    this->Check();
  }

  void Move(uint32 row, uint32 col) {
    this->CheckMove(row, col);

    // Set up pointers to the right colors for
    // the sense of whose turn it is.
    BitBoard& current_turn = m_color[this->m_turn];
    BitBoard& other_turn = m_color[!this->m_turn];

    // Visit the empty square tracking the set of bits that will
    // flipped with a piece there this turn
    BitBoard toggles; toggles = 0;
    bool isvalid = this->VisitEmptySquare(row, col, &toggles);
    assert(toggles != 0);
    assert(isvalid);

    // Apply the toggles
    current_turn |= toggles;
    other_turn -= toggles;

    // Switch turns
    this->m_turn = !this->m_turn;

    // Build up the next moves
    this->BuildNextMoves();

    // Deal with a user not having moves left, but the other
    // having moves left.
    if (this->m_next_moves == 0) {
      // Switch back to the user who just moved
      this->m_turn = !this->m_turn;

      // If this guy doesn't have moves, the game really is
      // over...
      this->BuildNextMoves();
    }

    // Make sure we did it all correctly
    this->Check();
  }

  void GetFilled(BitBoard* filled_) const {
    BitBoard& filled = *filled_; filled = 0;

    filled |= this->m_color[White];
    filled |= this->m_color[Black];
  }

  const BitBoard& GetValidMoves() const {
    return this->m_next_moves;
  }

  const BitBoard& GetColor(Color c) const {
    assert((c == Black) || (c == White));
    return this->m_color[c];
  }

  Color GetTurn() const {
    return this->m_turn;
  }

  // Return whether the game is over and the 
  // white and black scores
  bool Score(uint32* black_, uint32* white_) const {
    // Avoid confusion about operator precedence
    uint32& black = *black_ = 0;
    uint32& white = *white_ = 0;

    for (auto pos : this->m_color[Black].SetBits()) {
      black++;
    }

    for (auto pos : this->m_color[White].SetBits()) {
      white++;
    }
    // If there are no more moves, then the game is over.
    return (this->m_next_moves == 0);
  }

 protected:
  void BuildNextMoves() {
    // Establish the set of currently occupied squares
    BitBoard filled; this->GetFilled(&filled);

    // Clear out the current set of next_moves
    this->m_next_moves = 0;

    // foreach empty square
    //  - if there is an opposite color adjacent to it and if following that
    //   path leads to a square of our color, then set this square in 
    //   this->m_next_moves
    for (std::pair<uint32, uint32> pos : filled.ClearBits()) {
      if (this->VisitEmptySquare(pos.first, pos.second, NULL)) {
	this->m_next_moves[pos.first][pos.second] = true;
      }
    }

    this->Check();
  }

  bool VisitEmptySquare(uint32 row, uint32 col, BitBoard* toggles) const {
    const BitBoard& other_color = this->m_color[!this->m_turn];
    bool found_flips = false;
    // Walk the neighbors of this square
    for (int32 nrow_ = row-1; nrow_ <= static_cast<int32>(row+1); ++nrow_) {
      for (int32 ncol_ = col-1; ncol_ <= static_cast<int32>(col+1); ++ncol_) {
	uint32 nrow = static_cast<uint32>(nrow_);
	uint32 ncol = static_cast<uint32>(ncol_);
	if ((nrow < 8) && (ncol < 8) && ((nrow != row) || (ncol != col))) {
	  // Check the neighbor for a square of the opposite color
	  if (other_color[nrow][ncol]) {
	    // Track the set of squares flipped if we find an endpoint
	    BitBoard flips;
	    flips = 0;

	    // If so, walk in that direction following that color until we
	    // find an empty square, the edge of the board, or a square
	    // that is filled with our color (success).
	    if (this->VisitDirection(row, col, nrow-row, ncol-col, &flips)) {
	      if (toggles == NULL) {
		return true;
	      }
	      *toggles |= flips;
	      found_flips = true;
	    }
	  }
	}
      }
    }
    return found_flips;
  }

  bool VisitDirection(uint32 row, uint32 col, int hstep, int vstep,
		      BitBoard* flips_) const {
    const BitBoard& other_color = this->m_color[!this->m_turn];
    BitBoard& flips = *flips_;
    uint32 irow = row+hstep;
    uint32 icol = col+vstep;

    // row/col is the first of the other color, so mark it for flipping
    flips[row][col] = true;

    // While we are within the bounds of the board
    for(; (irow < 8) && (icol < 8); irow += hstep, icol += vstep) {
      if (!other_color[irow][icol]) {
	// We found a square within the board that is not filled with
	// the other color along this direction.  This terminates our
	// path.  Success or failure is gated on whether we found OUR
	// color in that square.
	return this->m_color[this->m_turn][irow][icol];
      } else {
	flips[irow][icol] = true;
      }
    }
    return false;
  }

 private:
  BitBoard m_color[2];
  BitBoard m_next_moves;
  Color    m_turn;
  DISABLE_COPY(Board);

 private:
  // TODO(mattmoor): Implement a release mode with this disabled.
  void CheckMove(uint32 row, uint32 col) const {
    // Get the set of filled squares
    BitBoard filled_;
    this->GetFilled(&filled_);
    const BitBoard& filled = filled_; // make immutable

    assert(!filled[row][col]);
    assert(this->m_next_moves[row][col]);
  }

  void Check() const {
    // Get the set of filled squares
    BitBoard filled_;
    this->GetFilled(&filled_);
    const BitBoard& filled = filled_; // make immutable
    BitBoard b;

    // Initialize to next moves
    b = 0;
    b |= m_next_moves;

    // Check that none of the next moves is in a filled square
    b &= filled;
    assert(b == 0);

    // Initilize to Black
    b |= m_color[Black];

    // Check that none of the White squares sits on the same square
    b &= m_color[White];
    assert(b == 0);

    // TODO(mattmoor): Other stuff to check?
  }
};

inline std::ostream& operator<<(std::ostream& out, const Board& b) {
  out << ((b.GetTurn() == Black) ? "Black" : "White")
      << "'s turn:" << std::endl;
  for (uint32 row = 0; row < 8; ++row) {
    for (uint32 col = 0; col < 8; ++col) {
      if (b.GetColor(Black)[row][col]) {
	out << '1';
      } else if (b.GetColor(White)[row][col]) {
	out << '0';
      } else if (b.GetValidMoves()[row][col]) {
	out << '.';
      } else {
	out << '-';
      }
    }
    out << std::endl;
  }
  out << std::endl;
  return out;
}

} // namespace Othello

#endif  // _BOARD_H_
