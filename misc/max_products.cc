#include "max_products.h"

#include <chrono>
#include <cmath>
#include <iostream>

namespace code_experiments {

static void PrintCombination(const Combination &combination, int64_t product) {
  bool is_first = true;
  for (const auto &element : combination) {
    if (!is_first) std::cout << " * ";
    std::cout << element.first << "^" << element.second;
    is_first = false;
  }
  std::cout << " = " << product;
  std::cout << std::endl;
}

static void UpdateFactor(Combination &combination, int factor, int by) {
  auto match = combination.find(factor);
  if (match != combination.end()) {
    match->second += by;
    if (match->second == 0) {
      combination.erase(match);
    }
  } else {
    combination.insert(std::pair<int, int>(factor, by));
  }
}

static void ReplaceFactor(Combination &combination, int remove, int add) {
  UpdateFactor(combination, add, 1);
  UpdateFactor(combination, remove, -1);
}

void CombinationFinder::ComputeForSum(int target_sum) {
  CHECK(target_sum >= 1) << "Only positive sums allowed.";
  std::set<Combination> a = {{{1, 1}}};
  std::set<Combination> b = {{{}}};
  std::set<Combination> *current = &a;
  std::set<Combination> *next = &b;
  for (auto sum = 2; sum <= target_sum; sum++) {
    next->clear();
    for (const auto &combination : *current) {
      auto candidate = combination;
      candidate = combination;
      for (const auto &[base, power] : combination) {
        ReplaceFactor(candidate, base, base + 1);
        next->insert(candidate);
        ReplaceFactor(candidate, base + 1, base);
      }
      // Add combination for trailing 1
      UpdateFactor(candidate, 1, 1);
      next->insert(candidate);
    }
    LOG(INFO) << "Evaluated " << next->size() << " combinations for sum "
              << sum;
    std::set<Combination> *tmp = current;
    current = next;
    next = tmp;
  }
  for (const auto &combination : *current) {
    EvaluateCombination(combination);
  }
  LOG(INFO) << "Combination with max product: ";
  PrintCombination(combination_, product_);
}

void CombinationFinder::EvaluateCombination(const Combination &combination) {
  int64_t product = 1;
  for (const auto &[base, power] : combination) {
    product *= std::pow(base, power);
  }
  if (product > product_) {
    product_ = product;
    combination_ = combination;
  }
}

}  // namespace code_experiments

int main() {
  LOG(INFO) << "Compute max products for numbers that add to a given sum.";
  const auto sum = 20;

  namespace chrono = std::chrono;
  auto start = chrono::high_resolution_clock::now();
  code_experiments::CombinationFinder finder;
  finder.ComputeForSum(sum);
  auto end = chrono::high_resolution_clock::now();
  LOG(INFO) << "Computing max product for " << sum << " took "
            << chrono::duration_cast<chrono::seconds>(end - start).count()
            << " seconds.";

  return 0;
}
