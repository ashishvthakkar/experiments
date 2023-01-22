#include <gtest/gtest.h>
#include <glog/logging.h>

#include "primes.h"

namespace code_experiments {

class PrimesTests : public ::testing::Test {
public:
  const std::vector<int> expected_primes{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
};

TEST_F(PrimesTests, DerefCheck) {  // NOLINT
  PrimeNumbers primes;
  EXPECT_EQ(*primes.begin(), 2);
}

void ValidateSequence(
    PrimeNumbers &primes,
    const std::vector<int> &expected) {  // NOLINT
  int index_expected = 0;
  for (auto x : primes) {
    LOG(INFO) << "Iterator value: " << x;
    EXPECT_EQ(x, expected[index_expected]);
    index_expected++;
    if (index_expected >= expected.size()) {
      break;
    }
  }
}

TEST_F(PrimesTests, SimpleSequenceCheck) {  // NOLINT
  PrimeNumbers primes;
  ValidateSequence(primes, expected_primes);
}

TEST_F(PrimesTests, TwoSequencesCheck) {  // NOLINT
  std::vector<int> small_seq{
      2,
      3,
      5};  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  PrimeNumbers primes;
  ValidateSequence(primes, small_seq);
  ValidateSequence(primes, expected_primes);
}

TEST_F(PrimesTests, RegularUpdateCheck) {  // NOLINT
  PrimeNumbers primes;
  auto it = primes.begin();
  for (auto expected : expected_primes) {
    EXPECT_EQ(*it, expected);
    ++it;
  }
}

}  // namespace code_experiments