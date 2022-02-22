#include <benchmark/benchmark.h>

#include <iostream>

#include "expressions.h"

template <typename D>
static void Test4_1(benchmark::State& state) {
  typename D::Expr e = LoadFromString<D>(
      "BOp - BOp * BOp - C 4 C 7 BOp - C 4 C 7 BOp * BOp - C 4 C 7 BOp - C 4 C "
      "7 ");
  for (auto _ : state) D::Compute(e);
}

template <typename D>
static void Test4_2(benchmark::State& state) {
  typename D::Expr e = LoadFromString<D>(
      "BOp - BOp * BOp - C 4 C 7 BOp - C 4 C 7 BOp * BOp - C 4 C 7 BOp - C 4 C "
      "7 ");
  volatile int len;
  for (auto _ : state) {
    std::string result;
    result = D::AsString(e);
    len = result.size();
  }
}

template <typename D>
static void Test5_ComputeBOp(benchmark::State& state) {
  std::string s = "BOp + C 1 C 1 ";
  for (int i = 0; i < 20; i++) {
    s = "BOp + " + s + s;
  }
  typename D::Expr e = LoadFromString<D>(s);
  for (auto _ : state) D::Compute(e);
}

template <typename D>
static void Test5_ComputeFx(benchmark::State& state) {
  std::string s = "Fx min 2 C 1 C 1 ";
  for (int i = 0; i < 10; i++) {
    s = "Fx min 2 " + s + s;
  }
  typename D::Expr e = LoadFromString<D>(s);
  for (auto _ : state) D::Compute(e);
}

template <typename D>
static void Test5_AsString(benchmark::State& state) {
  std::string s = "BOp + C 1 C 1 ";
  for (int i = 0; i < 20; i++) {
    s = "BOp + " + s + s;
  }
  typename D::Expr e = LoadFromString<D>(s);
  for (auto _ : state) D::AsString(e);
}

BENCHMARK(Test4_1<AtExpressionsDriver>);
BENCHMARK(Test4_1<KyExpressionsDriver>);
BENCHMARK(Test4_1<KyExpressionsDriverDD>);

BENCHMARK(Test4_2<AtExpressionsDriver>);
BENCHMARK(Test4_2<KyExpressionsDriver>);
BENCHMARK(Test4_2<KyExpressionsDriverDD>);

BENCHMARK(Test5_ComputeBOp<AtExpressionsDriver>);
BENCHMARK(Test5_ComputeBOp<KyExpressionsDriver>);
BENCHMARK(Test5_ComputeBOp<KyExpressionsDriverDD>);

BENCHMARK(Test5_ComputeFx<AtExpressionsDriver>);
BENCHMARK(Test5_ComputeFx<KyExpressionsDriver>);
BENCHMARK(Test5_ComputeFx<KyExpressionsDriverDD>);

BENCHMARK(Test5_AsString<AtExpressionsDriver>);
BENCHMARK(Test5_AsString<KyExpressionsDriver>);
BENCHMARK(Test5_AsString<KyExpressionsDriverDD>);

BENCHMARK_MAIN();