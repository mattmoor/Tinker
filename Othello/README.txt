This code implements the basic framework for a simplistic Othello AI tournament.  To prove the concept, it implements two players: Random and User, which will allow entrants to play against their AI or pit it against another automated entrant.

Some TODOs are left through the code, but it is functionally complete.


It can be build with Clang via:
   clang++ -O2 -std=c++11 tournament.cc player1.cc player2.cc ... playerN.cc


Consult "random.cc" and "user-player.cc" for how to properly set up a player.
