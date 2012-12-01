#ifndef _TOURNAMENT_H_
#define _TOURNAMENT_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "types.h"
#include "player.h"

using std::shared_ptr;

class TournamentManager {
 public:
  virtual ~TournamentManager() {
    in_shutdown_ = true;
    Reset();
  }

  void BeQuiet() {
    be_quiet_ = true;
  }

  void Reset();

  void RegisterPlayer(shared_ptr<Player>& player) {
    uint32 id = AssignID();
    m_players.push_back(player);
    m_ids.push_back(id);
    player->AssignID(id);

    m_scores[id] = 0;
  }

  void RunTournament() {
    // List entrants
    if (!be_quiet_) {
      std::cout << "Entrants: " << std::endl;
      for (const auto& player : m_players) {
	std::cout << player->name() << std::endl;
      }
      std::cout << std::endl;
    }

    if (m_players.size() < 2) {
      std::cerr << "Insufficient players to mount tournament" << std::endl;
      return;
    }

    // Check that nobody fooled with their IDs
    this->CheckIDs();

    // Have each entrant play each other, with color decided by a coin toss.
    for (uint32 i = 0; i < m_players.size(); ++i) {
      const auto& player1 = m_players[i];
      for (uint32 j = i + 1; j < m_players.size(); ++j) {
	const auto& player2 = m_players[j];
	PlayGame(player1.get(), player2.get());
      }
    }

    // Check IDs right before we score this bitch, to make sure that
    // no players fooled with IDs during gameplay.
    this->CheckIDs();

    // Sort using the score map
    // std::sort

    // Display the scores
    if (!be_quiet_) {
      std::cout << "Entrants: " << std::endl;
      for (const auto& player : m_players) {
	std::cout << player->name() << " : " << score(player.get()) << std::endl;
      }
      std::cout << std::endl;
    }
  }

  // TODO(mattmoor): Why doesn't std::map have a const overload of operator[]?
  uint32 score(const Player* player) {
    return m_scores[player->id()];
  }

 private:
  std::vector<std::shared_ptr<Player>> m_players;
  std::map<uint32, uint32> m_scores;

  void PlayGame(Player* p1_, Player* p2_) {
    bool coin_toss = CoinFlip();
    Player* players[2] = {
      (coin_toss ? p1_ : p2_),
      (coin_toss ? p2_ : p1_)
    };

    uint32 black, white;
    Othello::Board b;
    while (!b.Score(&black, &white)) {
      Player* turn = players[b.GetTurn()];
      uint32 row = 8, col = 8; // assign to invalid values to ensure move
      bool pass = turn->Move(b, &row, &col);

      // TODO(mattmoor): integrate taunting.
      // TODO(mattmoor): implement passing?  (make sure no pass-passing)
      if (pass) {
	assert(!"NYI");
      }

      assertm((row < 8) && (col < 8), 
	      turn->name() << " played off board: "
	      "(" << row << "," << col << ")");
      assertm(b.GetValidMoves()[row][col],
	      turn->name() << " made an invalid move: "
	      "(" << row << "," << col << ")");

      b.Move(row, col);
    }

    if (black == white) {
      // TODO(mattmoor): Decide whether this should count as a victory 
      // for both players or not.
      players[0]->NotifyTie();
      players[1]->NotifyTie();
      return;
    }

    Player* winner = (black < white) ? players[0] : players[1];
    Player* loser = (black > white) ? players[0] : players[1];
    if (!be_quiet_) {
      std::cout << winner->name() << " beat " << loser->name() << std::endl;
      std::cout << "Final Board: " << b;
    }
    m_scores[winner->id()]++;

    // Notify the winners, so at DTOR time, they know their record.
    winner->NotifyWinner();
    loser->NotifyLoser();
  }

  bool CoinFlip() {
    return (rand() & 1) != 0;
  }

  // For checking IDs
  std::vector<uint32> m_ids;
  void CheckIDs() const {
    assert(m_players.size() == m_ids.size());

    for (uint32 i = 0; i < m_ids.size(); ++i) {
      assertm(m_players[i]->id() == m_ids[i],
	      m_players[i]->name() << " is a fucking cheater");
    }
  }

  uint32 AssignID() {
    static uint32 g_ID = 0;
    return ++g_ID;
  }

  // Only allow setting up the one static manager
  DISABLE_COPY(TournamentManager);
 TournamentManager() : be_quiet_(false), in_shutdown_(false) {
    ;
  }

  bool be_quiet_;
  bool in_shutdown_;

 public:
  static TournamentManager g_manager;
};

// Use Dynamic Initializers to register each player at startup
#define REGISTER(P)							\
  namespace { /* Anonymize this to allow multiple Registrations */	\
    class P ## Registrar {						\
    public:								\
      P ## Registrar() {						\
	shared_ptr<Player> p(new P());					\
	TournamentManager::g_manager.RegisterPlayer(p);			\
      }									\
    } __ ## P ## Registrar;  /* Create an instance of the Registrar*/	\
  }  // namespace

#endif  // _TOURNAMENT_H_
