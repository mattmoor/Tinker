#ifndef GOD_H_
#define GOD_H_

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

#include "individual.h"

template<class T>
class God {
 public:
  God(uint32 N, uint32 G, uint32 R) : N_(N), G_(G), R_(R) {}

  virtual ~God() {
    // Armageddon!  Delete all individuals
    for (const auto* individual : population_) {
      delete individual;
    }
  }

  typedef std::vector<Individual<T>*> Population;

  // Seed the initial population
  void BuildAdamAndEve() {
    add_new(N_);
  }

  // Iteratively breed the population until we find a maxima
  void HandDarwinKeys()  {
    uint32 generation = 0;
    const uint32 generations = G_;

    while (generation < generations) {
      std::cout << "Generation " << generation << "..." << std::endl;

      // Cache the previous population
      std::set<uint32> ids;
      for (const auto* ind : population_) {
	ids.insert(ind->id());
      }

      // Iteratively:
      // 1) Thin the population to the top N
      Thin();

      // 2) Cross-breed the population to include new entrants
      Breed();

      // 3) Introduce nomads (new random population)
      Nomads();

      // Check for convergence
      bool mismatch = false;
      for (const auto* ind : population_) {
	if (ids.find(ind->id()) != ids.end()) {
	  mismatch = true;
	  break;
	}
      }
      if (!mismatch) {
	break;
      }

      generation++;
    }

    // Thin the final population
    Thin();
  }

  const Population& FinalPopulation() const {
    return population_;
  }

 private:
  static Individual<T>* New();

  void Combat() {
    // Clear everyone's fitness
    for (auto* xx : population_) {
      xx->ResetFitness();
    }

    // Allow all pairs to combat to see who the dominant elements are
    for (auto* xx : population_) {
      for (auto* xy : population_) {
	if (xx != xy) {
	  int score = xx->Combat(*xy);
	  if (score > 0) {
	    xx->AddFitness(score);
	  } else if (score < 0) {
	    xy->AddFitness(-score);
	  }
	}
      }
    }
  }

  void Thin() {
    // Have all pairs enter combat, to establish rankings
    Combat();

    // Order the population based on combat rankings
    std::sort(population_.begin(), population_.end(), God<T>::Compare);

    // Eliminate all except for the top N_
    for (uint32 index = population_.size()-1; index >= N_; --index) {
      delete population_.back();
      population_.pop_back();
    }
    assert(population_.size() == N_);

    Display();
  }

  void Nomads() {
    add_new(R_);
  }

  void Display() {
  }

  void add_new(uint32 n) {
    for(uint32 i=0; i < n; ++i) {
      population_.push_back(New());
    }
  }


  void Breed() {
    // Cross-breed the population to include a whole new set of hybrids
    // NOTE: don't use foreach syntax because it doesn't handle editing,
    // nor are triangular loops easy...
    assert(population_.size() == N_);
    for (uint32 i = 0; i < N_; ++i) {
      const auto* xx = population_[i];
      for (uint32 j = i + 1; j < N_; ++j) {
	const auto* xy = population_[j];
	auto children = xx->Breed(*xy);
	population_.insert(population_.end(), children.begin(), children.end());
      }
    }

    // Remove duplicates
    RemoveClones();
  }

  void RemoveClones() {
    bool new_blood = false;

    // Order the population so that equivalent elements will be adjacent
    std::sort(population_.begin(), population_.end(), Individual<T>::CompareLessThan);

    for (uint32 i = 0; i < N_;) {
      if (Individual<T>::Equals(population_[i], population_[i+1])) {
	delete (population_[i+1]);
	auto position = population_.begin()+(i+1);
	population_.erase(position, position); 
	population_.push_back(New());
	new_blood = true;
      } else {
	++i;
      }
    }

    if (new_blood) {
      // Make sure our random arrival isn't a genetic match
      RemoveClones();
    }
  }

  Population population_;
  const uint32 N_;
  const uint32 G_;
  const uint32 R_;

  static bool Compare(const Individual<T>* lhs,
		      const Individual<T>* rhs) {
    return lhs->Fitness() > rhs->Fitness();
  }
};

#endif  // GOD_H_
