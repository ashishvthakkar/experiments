#include <chrono>

#include "max_products.h"

template <typename T>
void Evaluate(T &t, int sum_start, int sum_end) {
  namespace chrono = std::chrono;
  LOG(INFO) << "Evaluating " << typeid(t).name();
  for (auto sum = sum_start; sum <= sum_end; sum++) {
    auto start = chrono::high_resolution_clock::now();
    T copy = t;
    copy.ComputeForSum(sum);
    auto end = chrono::high_resolution_clock::now();
    auto diff = (end - start).count();
    LOG(INFO)
        << "Computing max product for " << sum << " took "
        << chrono::duration_cast<chrono::microseconds>(end - start).count()
        << " microseconds. (time / n^3, time / n^2, time / n) = ("
        << (diff / (sum * sum * sum)) << ", " << (diff / (sum * sum)) << ", "
        << (diff / sum) << ")";
  }
}

int main() {
  LOG(INFO) << "Compute max products for numbers that add to a given sum.";
  const auto sum_start = 100;
  const auto sum_end = 119;  // NOTE: There is overflow for int64_t after 119

  // const auto sum_start = 1;
  // const auto sum_end = 5;  // NOTE: There is overflow for int64_t after 119

  // code_experiments::BasicFinder finder;
  // Evaluate(finder, sum_start, sum_end);

  code_experiments::KyFinder ky_finder;
  Evaluate(ky_finder, sum_start, sum_end);

  code_experiments::FastFinder ff;
  Evaluate(ff, sum_start, sum_end);

  return 0;
}
