#ifndef _VALUE_BOARD_H_
#define _VALUE_BOARD_H_

#include "board.h"
#include "player.h"

#include <stdlib.h>
#include <sstream>

namespace mattmoor {

  namespace {
    // TODO(mattmoor): train this with machine learning.
    uint32 g_values[8][8] = {
      {50,  5, 15, 15, 15, 15, 5,  50},
      {5,   1,  5,  1,  1,  5, 1,   5},
      {15,  1, 10,  8,  8, 10, 5,  15},
      {15,  1,  8,  5,  5,  8, 1,  15},
      {15,  1,  8,  5,  5,  8, 1,  15},
      {15,  5, 10,  8,  8, 10, 5,  15},
      {5,   1,  5,  1,  1,  5, 1,   5},
      {50,  5, 15, 15, 15, 15, 5,  50},
    };
  }  // anonymous namespace

  class ValuePlayer : public Player {
  public:
    ValuePlayer(const std::string& name) : Player(), m_name(name) {
      if (char* values = getenv("MM_BOARD")) {
	ParseBoard(values);
      } else {
	memcpy(values_, g_values, sizeof(g_values));
      }
    }

    ValuePlayer(const std::string& name, const char* values)
      : Player(), m_name(name) {
      ParseBoard(values);
    }

    ValuePlayer(const std::string& name,
		const uint32 (&values)[8][8])
      : Player(), m_name(name) {
      memcpy(values_, values, sizeof(values));
    }

    virtual ~ValuePlayer() {
      // call the base dtor
      Player::~Player();

/*       std::cout << "SUMMARY[" << name() << "]: (" */
/* 		<< wins() << "," */
/* 		<< losses() << "," */
/* 		<< ties() << ") " */
/* 		<< ((wins() > losses()) ? "+" : "") */
/* 		<< (wins() - losses()) */
/* 		<< std::endl; */
    }

    virtual bool Move(const Othello::Board& b, uint32* row, uint32* col) {
      const BitBoard& valid = b.GetValidMoves();
      std::pair<uint32, uint32> best_move;
      uint32 best_value = 0;

      for (auto pos : valid.SetBits()) {
	auto value = Value(b, pos);
	if (value >= best_value) {
	  best_move = pos;
	  best_value = value;
	}
      }

      *row = best_move.first;
      *col = best_move.second;

      return false;
    }

    virtual const std::string& name() const {
      return this->m_name;
    }

    const uint32 (&values() const)[8][8] {
      return values_;
    }

  protected:
    virtual uint32 Value(const Othello::Board& b,
			 const std::pair<uint32, uint32>& pos) = 0;

  private:
    std::string m_name;
    uint32 values_[8][8];

    void ParseBoard(const char* env_board) {
      std::string str_brd(env_board);
      std::istringstream iss(str_brd);

      uint32 index = 0;
      while(iss) {
	std::string substring;
	iss >> substring;
	if (substring.empty()) {
	  continue;
	}
	values_[0][index] = atoi(substring.c_str());
	++index;
      }
      assertm(index == 64, index);
    }
  };
}  // namespace mattmoor

#endif  // _VALUE_BOARD_H_
