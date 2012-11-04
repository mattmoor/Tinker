#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>

#include "board.h"

class Player {
 public:
  // The public interface for players to implement

  // Move returns whether to pass without moving, otherwise it initializes
  // the out-parmeters "row" and "col" to valid moves on the given game board
  virtual bool Move(const Othello::Board& b, uint32* row, uint32* col) = 0;

  // name returns an identifier to glorify or shame the creator of this AI
  virtual const std::string& name() const = 0;

  // Allow players an interface for taunting other players.
  virtual const std::string taunt(const Player* player) const {
    return "";
  }


  // Some ID assignment logic, with checks to make sure
  // you fucking cheaters play honestly.  YOU MUST INVOKE
  // THIS CTOR TO AVOID ASSERTIONS
  Player() : m_id(0) {
  }

  void AssignID(uint32 id) {
    assertm(this->m_id == 0, 
	    "You forgot to invoke the base ctor, or you are cheating");
    this->m_id = id;
  }

  uint32 id() const {
    return m_id;
  }

 private:
  uint32 m_id;
};

#endif  // _PLAYER_H_
