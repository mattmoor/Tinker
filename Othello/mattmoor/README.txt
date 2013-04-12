This uses the Genetic algorithm package to optimize the board values used by the {square,board}-value-player's.

The player it optimizes is determined by the PLAYER #define in othello-god.cc

It can be built via:

  clang++ -O3 -std=c++11 -stdlib=libc++ -I. -I../Genetic/ mattmoor/othello-god.cc

