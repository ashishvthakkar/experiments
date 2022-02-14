#include <benchmark/benchmark.h>

#include "expressions.h"
#include <iostream>

template <typename D>
static void Test4_1(benchmark::State& state) {
  typename D::Expr e = LoadFromString<D>("BOp - BOp * BOp - C 4 C 7 BOp - C 4 C 7 BOp * BOp - C 4 C 7 BOp - C 4 C 7 ");
  for (auto _ : state)
    D::Compute(e);
}

template <typename D>
static void Test4_2(benchmark::State& state) {
  typename D::Expr e = LoadFromString<D>("BOp - BOp * BOp - C 4 C 7 BOp - C 4 C 7 BOp * BOp - C 4 C 7 BOp - C 4 C 7 ");
  volatile int len;
  for (auto _ : state) {
    std::string result;
    result = D::AsString(e);
    len = result.size();
  }
}

BENCHMARK(Test4_1<AtExpressionsDriver>);
BENCHMARK(Test4_1<KyExpressionsDriver>);
BENCHMARK(Test4_1<KyExpressionsDriverDD>);

BENCHMARK(Test4_2<AtExpressionsDriver>);
BENCHMARK(Test4_2<KyExpressionsDriver>);
BENCHMARK(Test4_2<KyExpressionsDriverDD>);

BENCHMARK_MAIN();