#include "max_products.h"

#include <chrono>
// #include <cmath> // NOTE: std::pow doesn't always seem consistent with direct
// multiplication
#include <iostream>
#include <set>

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

static int64_t Product(const Combination &combination) {
  int64_t product = 1;
  for (const auto &[base, power] : combination) {
    for (auto i = 0; i < power; i++) {
      product *= base;
    }
  }
  return product;
}

int64_t BasicFinder::ComputeForSum(int target_sum) {
  CHECK(target_sum >= 2) << "Only sums > 1 allowed.";
  std::set<Combination> a = {{{1, 1}}, {{2, 1}}};
  std::set<Combination> b = {{{}}};
  std::set<Combination> *current = &a;
  std::set<Combination> *next = &b;
  // Note: Below collates 1^n to 1^1
  for (auto sum = 3; sum <= target_sum; sum++) {
    LOG(INFO) << "Identifying combinations for sum " << sum;
    next->clear();
    for (const auto &combination : *current) {
      auto candidate = combination;
      candidate = combination;
      for (const auto &[base, power] : combination) {
        ReplaceFactor(candidate, base, base + 1);
        next->insert(candidate);
        ReplaceFactor(candidate, base + 1, base);
      }
      // Add combination for trailing 1 unless it only increases the power
      // for 1. If the combination already contains 1 as the base, adding to the
      // power of 1 will not increase the product, and any relevant combinations
      // for next sum will be identified with the existing base 1 and power 1.
      if (candidate.find(1) == candidate.end()) {
        UpdateFactor(candidate, 1, 1);
        next->insert(candidate);
      }
    }
    for (const auto &combination : *next) {
      EvaluateCombination(combination);
    }
    LOG(INFO) << "Evaluated " << next->size() << " combinations for sum "
              << sum;
    std::set<Combination> *tmp = current;
    current = next;
    next = tmp;
  }
  LOG(INFO) << "Combination with max product: ";
  PrintCombination(combination_, product_);
  return product_;
}

void BasicFinder::EvaluateCombination(const Combination &combination) {
  int64_t product = Product(combination);
  PrintCombination(combination, product);
  if (product > product_) {
    product_ = product;
    combination_ = combination;
  }
}

void KyFinder::PrintCombination() {
  for (const auto value : ky_combination_) {
    std::cout << value << " ";
  }
  std::cout << std::endl;
}

void KyFinder::ComputeForSum(int target_sum) {
  // LOG(INFO) << "Ky: Identifying combinations for sum " << target_sum;
  for (auto count = 1; count <= target_sum; count++) {
    auto product = ComputeForSum(count, target_sum);
    if (!ky_combination_.empty()) {
      // PrintKyCombination();
      // LOG(INFO) << "Debug: Extra print for " << count;
    }
    ky_combination_.clear();
    // LOG(INFO) << product;
    if (product > product_) {
      product_ = product;
    }
  }
  LOG(INFO) << "KyMax = " << product_;
}

int64_t KyFinder::ComputeForSum(int count, int target_sum) {
  if (count == 1) {
    ky_combination_.push_back(target_sum);
    added_tail_ = true;
    return target_sum;
  }
  if (cache_.find(count) != cache_.end() &&
      cache_[count].find(target_sum) != cache_[count].end())
  {
    return cache_[count][target_sum];
  }
  int64_t max = 0;
  for (auto num = 1; num < target_sum - count + 1; num++) {
    // std::cout << num << " ";
    ky_combination_.push_back(num);
    int64_t product = ComputeForSum(count - 1, target_sum - num) * num;
    // LOG(INFO) << product;
    // PrintKyCombination();
    if (product > max) {
      max = product;
    }
    if (added_tail_) {
      ky_combination_.pop_back();
      added_tail_ = false;
    }
    ky_combination_.pop_back();
  }
  if (cache_.find(count) == cache_.end()) {
    cache_[count] = std::map<int, int64_t>();
  }
  cache_[count][target_sum] = max;
  return max;
}

int64_t FastFinder::ComputeForSum(int sum) {
  for (auto s = 2; s <= sum; s++) {
    auto smallest = combination_.begin()->first;
    auto replacement = smallest + 1;
    ReplaceFactor(combination_, smallest, replacement);
    for (auto factor = 2; factor <= replacement / 2; factor++) {
      if (replacement % factor == 0) {
        ReplaceFactor(combination_, replacement, factor);
        UpdateFactor(combination_, replacement / factor, 1);
        break;
      }
    }
  }
  auto product = Product(combination_);
  PrintCombination(combination_, product);
  return product;
}

}  // namespace code_experiments
