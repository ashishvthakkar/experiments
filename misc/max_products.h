#ifndef EXPERIMENTS_MISC_MAX_PRODUCTS_H
#define EXPERIMENTS_MISC_MAX_PRODUCTS_H

#include <glog/logging.h>

#include <map>

namespace code_experiments {

using Combination = std::map<int, int>;

class BasicFinder {
public:
  explicit BasicFinder() : combination_{{1, 1}}, product_{1} {}
  int64_t ComputeForSum(int target_sum);

private:
  void EvaluateCombination(const Combination &combination);

  Combination combination_;
  int64_t product_;
  std::map<int, std::map<int, int64_t>> cache_;
};

class KyFinder {
public:
  explicit KyFinder() : added_tail_(false), product_{1} {}
  void ComputeForSum(int target_sum);

private:
  int64_t ComputeForSum(int count, int target_sum);
  void PrintCombination();

  std::map<int, std::map<int, int64_t>> cache_;
  std::vector<int> ky_combination_;
  int64_t product_;
  bool added_tail_;
};

class FastFinder {
public:
  explicit FastFinder() : combination_{{1, 1}} {}
  int64_t ComputeForSum(int sum);

private:
  Combination combination_;
};

}  // namespace code_experiments

#endif