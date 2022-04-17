#ifndef EXPERIMENTS_MISC_MAX_PRODUCTS_H
#define EXPERIMENTS_MISC_MAX_PRODUCTS_H

#include <glog/logging.h>

#include <map>
#include <set>

namespace code_experiments {

using Combination = std::map<int, int>;

class CombinationFinder {
public:
  explicit CombinationFinder() : combination_{{1, 1}}, product_{1} {}
  void ComputeForSum(int target_sum);

private:
  void EvaluateCombination(
      const Combination &combination,
      std::set<Combination> *next);

  std::set<Combination> cache_;
  Combination combination_;
  int64_t product_;
};

}  // namespace code_experiments

#endif