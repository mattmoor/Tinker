#include "tournament.h"

// Define g_manager
TournamentManager TournamentManager::g_manager;

int main() {

  TournamentManager::g_manager.RunTournament();

  return 0;
}
