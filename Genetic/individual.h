#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include <vector>

typedef unsigned int uint32;

template<class T>
class Individual {
 public:
  Individual(const T& t) : info_(t), fitness_(0), id_(++g_id) {}

  uint32 Fitness() const {
    return fitness_;
  }

  void AddFitness(uint32 pts) {
    fitness_ += pts;
  }

  void ResetFitness() {
    fitness_ = 0;
  }

  uint32 id() const {
    return id_;
  }

  T& info() {
    return info_;
  }

  const T& info() const {
    return info_;
  }

  int Combat(const Individual<T>& adversary) const;

  std::vector<Individual<T>*> Breed(const Individual<T>& mate) const;

  // A set of comparison functions.
  static bool Equals(const Individual<T>* lhs,
		     const Individual<T>* rhs) {
    return Compare(lhs, rhs) == 0;
  }

  static bool CompareLessThan(const Individual<T>* lhs,
			      const Individual<T>* rhs) {
    return Compare(lhs, rhs) < 0;
  }

  static bool CompareGreaterThan(const Individual<T>* lhs,
				 const Individual<T>* rhs) {
    return Compare(lhs, rhs) > 0;
  }

  static int32 Compare(const Individual<T>* lhs,
		       const Individual<T>* rhs) {
    return memcmp(&lhs->info_, &rhs->info_, sizeof(T));
  }

 private:
  uint32 id_;
  T info_;
  uint32 fitness_;

  // global ID allocator
  static uint32 g_id;
};

template <class T>
uint32 Individual<T>::g_id = 0;

#endif  // INDIVIDUAL_H_
