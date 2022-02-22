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

static const std::string &construct1() {
  static bool constructed = false;
  static std::string s;
  if (!constructed) {
    constructed = true;
    s = "BOp + C 1 C 1 ";
    for (int i = 0; i < 20; i++) {
      s = "BOp + " + s + s;
    }
  }
  return s;
}

template <typename D>
static D::Expr &construct2() {
  static bool constructed = false;
  static typename D::Expr e;
  if (!constructed) {
    constructed = true;
    const auto &s = construct1();
    e = LoadFromString<D>(s);
  }
  return e;
}

template <typename D>
static void Test0_construct1(benchmark::State& state) {
  for (auto _ : state) {
    const auto &e = construct1();
  }
}

template <typename D>
static void Test0_construct2(benchmark::State& state) {
  for (auto _ : state) {
    const auto &e = construct2<D>();
  }
}

template <typename D>
static void Test5_LoadFromString(benchmark::State& state) {
  std::string s = "BOp + C 1 C 1 ";
  for (int i = 0; i < 20; i++) {
    s = "BOp + " + s + s;
  }
  for (auto _ : state) {
    typename D::Expr e = LoadFromString<D>(s);
  }
}

template <typename D>
static void Test5_ComputeBOp(benchmark::State& state) {
  std::string s = "BOp + C 1 C 1 ";
  for (int i = 0; i < 20; i++) {
    s = "BOp + " + s + s;
  }
  typename D::Expr e = LoadFromString<D>(s);
  auto &ee = e;
  for (auto _ : state) D::Compute(ee);
}

template <typename D>
static void Test5a_ComputeBOp(benchmark::State& state) {
  auto &ee = construct2<D>();
  for (auto _ : state) D::Compute(ee);
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

// BENCHMARK(Test0_construct1<AtExpressionsDriver>);
// BENCHMARK(Test0_construct1<KyExpressionsDriver>);
// BENCHMARK(Test0_construct1<KyExpressionsDriverDD>);

// BENCHMARK(Test0_construct2<AtExpressionsDriver>);
// BENCHMARK(Test0_construct2<KyExpressionsDriver>);
// BENCHMARK(Test0_construct2<KyExpressionsDriverDD>);

// BENCHMARK(Test4_1<AtExpressionsDriver>);
// BENCHMARK(Test4_1<KyExpressionsDriver>);
// BENCHMARK(Test4_1<KyExpressionsDriverDD>);

// BENCHMARK(Test4_2<AtExpressionsDriver>);
// BENCHMARK(Test4_2<KyExpressionsDriver>);
// BENCHMARK(Test4_2<KyExpressionsDriverDD>);

BENCHMARK(Test5_LoadFromString<AtExpressionsDriver>);
BENCHMARK(Test5_LoadFromString<KyExpressionsDriver>);
BENCHMARK(Test5_LoadFromString<KyExpressionsDriverDD>);

BENCHMARK(Test5_ComputeBOp<AtExpressionsDriver>);
BENCHMARK(Test5_ComputeBOp<KyExpressionsDriver>);
BENCHMARK(Test5_ComputeBOp<KyExpressionsDriverDD>);

BENCHMARK(Test5a_ComputeBOp<AtExpressionsDriver>);
BENCHMARK(Test5a_ComputeBOp<KyExpressionsDriver>);
BENCHMARK(Test5a_ComputeBOp<KyExpressionsDriverDD>);

BENCHMARK(Test5_ComputeFx<AtExpressionsDriver>);
BENCHMARK(Test5_ComputeFx<KyExpressionsDriver>);
BENCHMARK(Test5_ComputeFx<KyExpressionsDriverDD>);

BENCHMARK(Test5_AsString<AtExpressionsDriver>);
BENCHMARK(Test5_AsString<KyExpressionsDriver>);
BENCHMARK(Test5_AsString<KyExpressionsDriverDD>);

BENCHMARK_MAIN();