#ifndef EXPERIMENTS_MISC_MAX_PRODUCTS_H
#define EXPERIMENTS_MISC_MAX_PRODUCTS_H

#include <glog/logging.h>

#include <map>
#include <set>

namespace code_experiments {

using Combination = std::map<int, int>;

class CombinationFinder {
public:
  explicit CombinationFinder()
      : combination_{{1, 1}},
        product_{1},
        added_tail_(false) {}
  void ComputeForSum(int target_sum);
  void KyComputeForSum(int target_sum);

private:
  void EvaluateCombination(const Combination &combination);
  int64_t KyComputeForSum(int count, int target_sum);
  void PrintKyCombination();

  Combination combination_;
  int64_t product_;
  std::map<int, std::map<int, int64_t>> cache_;
  std::vector<int> ky_combination_;
  bool added_tail_;
};

class CombinationFinderShort {
public:
  CombinationFinderShort() : combinations_{{{1, 1}}} {}
  int64_t ForSum(int sum);
  int64_t ForSum(int sum, Combination &combination);

private:
  std::vector<Combination> combinations_;
};

class FastFinder {
public:
  explicit FastFinder() : combination_{{1, 1}} {}

  int64_t ForSum(int sum);

private:
  Combination combination_;
};

}  // namespace code_experiments

#endif