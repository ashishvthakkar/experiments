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

static int64_t Product(const Combination &combination) {
  int64_t product = 1;
  for (const auto &[base, power] : combination) {
    for (auto i = 0; i < power; i++) {
      product *= base;
    }
  }
  return product;
}

void CombinationFinder::ComputeForSum(int target_sum) {
  CHECK(target_sum >= 1) << "Only positive sums allowed.";
  std::set<Combination> a = {{{1, 1}}, {{2, 1}}};
  std::set<Combination> b = {{{}}};
  std::set<Combination> *current = &a;
  std::set<Combination> *next = &b;
  // Note: Below collases 1^n to 1^1
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
}

void CombinationFinder::EvaluateCombination(const Combination &combination) {
  int64_t product = Product(combination);
  PrintCombination(combination, product);
  if (product > product_) {
    product_ = product;
    combination_ = combination;
  }
}

void CombinationFinder::PrintKyCombination() {
  for (const auto value : ky_combination_) {
    std::cout << value << " ";
  }
  std::cout << std::endl;
}

void CombinationFinder::KyComputeForSum(int target_sum) {
  // LOG(INFO) << "Ky: Identifying combinations for sum " << target_sum;
  for (auto count = 1; count <= target_sum; count++) {
    auto product = KyComputeForSum(count, target_sum);
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

int64_t CombinationFinder::KyComputeForSum(int count, int target_sum) {
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
    int64_t product = KyComputeForSum(count - 1, target_sum - num) * num;
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

int64_t CombinationFinderShort::ForSum(int sum) {
  int64_t max_product = 1;
  for (auto s = 2; s <= sum; s++) {
    Combination previous_best = combinations_[0];
    int index = 0;
    for (int i = 0; i < combinations_.size(); i++) {
      auto &combination = combinations_[i];
      auto smallest = combination.begin()->first;
      ReplaceFactor(combination, smallest, smallest + 1);
      auto product = Product(combination);
      if ((product > max_product) ||
          (product == max_product &&
           combination.begin()->first < combinations_[index].begin()->first))
      {
        max_product = product;
        index = i;
      }
    }
    combinations_[0] = combinations_[index];
    combinations_.resize(1);
    if (combinations_[0].size() == 1) {
      UpdateFactor(previous_best, 1, 1);
      combinations_.push_back(previous_best);
    }
  }
  PrintCombination(combinations_[0], Product(combinations_[0]));
  return max_product;
}

int64_t CombinationFinderShort::ForSum(int sum, Combination &combination) {
  int64_t product = ForSum(sum);
  LOG_ASSERT(!combinations_.empty());
  combination = combinations_[0];
  return product;
}

int64_t FastFinder::ForSum(int sum) {
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

int main() {
  LOG(INFO) << "Compute max products for numbers that add to a given sum.";
  const auto sum_start = 100;
  const auto sum_end = 119;  // NOTE: There is overflow for int64_t after 119

  namespace chrono = std::chrono;
  LOG(INFO) << "Trying direct/full exhaustive";
  auto start = chrono::high_resolution_clock::now();
  code_experiments::CombinationFinder finder;
  // finder.ComputeForSum(sum_end);
  auto end = chrono::high_resolution_clock::now();
  // LOG(INFO) << "Computing max product for " << sum_end << " took "
  //           << chrono::duration_cast<chrono::seconds>(end - start).count()
  //           << " seconds.";

  LOG(INFO) << "Trying Ky exhaustive";
  for (auto sum = sum_start; sum <= sum_end; sum++) {
    start = chrono::high_resolution_clock::now();
    code_experiments::CombinationFinder ky_finder;
    ky_finder.KyComputeForSum(sum);
    end = chrono::high_resolution_clock::now();
    LOG(INFO) << "Computing ky max product for " << sum << " took "
              << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << " nanoseconds, giving time / n^3 = "
              << ((end - start).count() / (sum * sum * sum));
  }

  LOG(INFO) << "Trying specialized combination finder";
  for (auto sum = sum_start; sum <= sum_end; sum++) {
    start = chrono::high_resolution_clock::now();
    code_experiments::CombinationFinderShort specialized;
    specialized.ForSum(sum);
    end = chrono::high_resolution_clock::now();
    LOG(INFO) << "Computing specialized max product for " << sum << " took "
              << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << " nanoseconds, giving time / n^3 = "
              << ((end - start).count() / (sum * sum * sum));
  }

  LOG(INFO) << "Trying fast finder";
  for (auto sum = sum_start; sum <= sum_end; sum++) {
    start = chrono::high_resolution_clock::now();
    code_experiments::FastFinder ff;
    ff.ForSum(sum);
    end = chrono::high_resolution_clock::now();
    LOG(INFO) << "Computing fast max product for " << sum << " took "
              << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
              << " nanoseconds, giving time / n^3 = "
              << ((end - start).count() / (sum * sum * sum));
  }

  return 0;
}
