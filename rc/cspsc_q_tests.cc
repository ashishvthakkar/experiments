#include <glog/logging.h>
#include <gtest/gtest.h>

#include "cspsc_q.h"

namespace code_experiments {

class CSpscTests : public ::testing::Test {};

TEST_F(CSpscTests, EnqDeqSizeCheck) {  // NOLINT
  const int size = 3;
  const std::array<CPair, 3> pairs{{{0, 323}, {1, 100}, {2, 1000}}};
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  CHECK((q.Enqueue(pairs[0]) == QErrorCode::kOk));
  CHECK((q.Enqueue(pairs[1]) == QErrorCode::kOk));
  CHECK((q.Enqueue(pairs[2]) == QErrorCode::kFull));
  CPair pair{0, 0};
  CHECK(q.Dequeue(pair) == QErrorCode::kOk);
  LOG(INFO) << CPairAsString(pair);
  CHECK(q.Dequeue(pair) == QErrorCode::kOk);
  LOG(INFO) << CPairAsString(pair);
  CHECK(q.Dequeue(pair) == QErrorCode::kEmpty);
  CHECK((q.Enqueue(pairs[0]) == QErrorCode::kOk));
  CHECK(q.Dequeue(pair) == QErrorCode::kOk);
  LOG(INFO) << CPairAsString(pair);
  CHECK(q.Dequeue(pair) == QErrorCode::kEmpty);
}

TEST_F(CSpscTests, EnqDeqCheck) {  // NOLINT
  const int size = 5;
  const std::array<CPair, 4> pairs{{{0, 323}, {1, 100}, {2, 1000}, {3, 324}}};
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  for (const auto &pair : pairs) {
    CHECK((q.Enqueue(pair) == QErrorCode::kOk));
  }
  for (int i = 0; i < pairs.size(); i++) {  // NOLINT(modernize-loop-convert)
    CPair pair{0, 0};
    CHECK(q.Dequeue(pair) == QErrorCode::kOk);
    ASSERT_EQ(pair.id, pairs.at(i).id);
    ASSERT_EQ(pair.val, pairs.at(i).val);
  }
}

TEST_F(CSpscTests, FastReaderCheck) {  // NOLINT
  const int size = 5;
  const std::array<CPair, 4> pairs{{{0, 323}, {1, 100}, {2, 1000}, {0, 324}}};
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  for (const auto &pair : pairs) {
    CHECK((q.Enqueue(pair) == QErrorCode::kOk));
    CPair return_pair{0, 0};
    CHECK(q.Dequeue(return_pair) == QErrorCode::kOk);
    ASSERT_EQ(return_pair.id, pair.id);
    ASSERT_EQ(return_pair.val, pair.val);
  }
}

TEST_F(CSpscTests, SlowReaderCheck) {  // NOLINT
  const int size = 5;
  const std::array<CPair, 4> pairs{{{0, 323}, {1, 100}, {2, 1000}, {0, 324}}};
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  for (const auto &pair : pairs) {
    CHECK((q.Enqueue(pair) == QErrorCode::kOk));
  }
  CPair pair{0, 0};
  CHECK(q.Dequeue(pair) == QErrorCode::kOk);
  ASSERT_EQ(pair.id, pairs.at(3).id);
  ASSERT_EQ(pair.val, pairs.at(3).val);
  for (int i = 1; i <= 2; i++) {  // NOLINT(modernize-loop-convert)
    CHECK(q.Dequeue(pair) == QErrorCode::kOk);
    ASSERT_EQ(pair.id, pairs.at(i).id);
    ASSERT_EQ(pair.val, pairs.at(i).val);
  }
}

}  // namespace code_experiments