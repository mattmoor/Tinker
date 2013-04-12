
#include "god.h"

// Include the trivial integer implementation for testing our God, but not that main
#define NO_MAIN
#include "individual.cc"

// Implement the our seeding for the integer population
template <>
void God<uint32>::BuildAdamAndEve() {
  for(uint32 i=0; i < N_; ++i) {
    population_.push_back(new Individual<uint32>(i));
  }
}

#include <iostream>
int main() {
  God<uint32> god(10);

  god.BuildAdamAndEve();

  god.HandDarwinKeys();

  for (auto* ind : god.FinalPopulation()) {
    std::cout << ind->info() << std::endl;
  }

  return 0;
}
