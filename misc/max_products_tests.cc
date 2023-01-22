#include <gtest/gtest.h>
#include <glog/logging.h>

#include "max_products.h"

namespace code_experiments {

class MaxProductsTests : public ::testing::Test {};

TEST_F(MaxProductsTests, BasicFinderCheck) {  // NOLINT
  BasicFinder f;
  EXPECT_EQ(f.ComputeForSum(5), 6);
}

}  // namespace code_experiments