#include "tournament.h"

// Define g_manager
TournamentManager TournamentManager::g_manager;

// Don't allow a real tournament to be reset
void TournamentManager::Reset() {
  assertm(in_shutdown_, "Real tournaments cannot be reset, stop cheating." << in_shutdown_);
}


int main() {

  TournamentManager::g_manager.RunTournament();

  return 0;
}
