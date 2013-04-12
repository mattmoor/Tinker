
#include "individual.h"

// Test the individual implementation with integers
template <>
Individual<uint32>* Individual<uint32>::Breed(const Individual<uint32>& mate) const {
  return new Individual<uint32>(info_ + mate.info_);
}

template <>
int Individual<uint32>::Combat(const Individual<uint32>& adversary) const {
  return info_ - adversary.info_;
}

#ifndef NO_MAIN
#include <iostream>

int main() {
  Individual<uint32> one(1);
  Individual<uint32> two(2);

  Individual<uint32>* three = one.Breed(two);

  std::cout << "Three = " << three->info() << std::endl;

  delete three;
  return 0;
}

#endif  // NO_MAIN
