#include <glog/logging.h>
#include <gtest/gtest.h>

#include <thread>

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
  CHECK(q.Dequeue(pair) == QErrorCode::kEmpty);
}

TEST_F(CSpscTests, MultithreadedWriterReaderCheck) {  // NOLINT
  const int num_elements = 2;
  const std::array<CPair, num_elements> pairs{{{0, 323}, {1, 100}}};
  const int size = num_elements + 1;
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  std::thread writer([&pairs, &q]() {
    for (const auto &pair : pairs) {
      CHECK((q.Enqueue(pair) == QErrorCode::kOk));
    }
  });
  std::thread reader([&pairs, &q]() {
    CPair pair{0, 0};
    for (auto num_read = 0; num_read < num_elements; num_read++) {
      while (q.Dequeue(pair) == QErrorCode::kEmpty) {
        // No-op. Wait for producer if required.
      }
      ASSERT_TRUE(pair.id == pairs.at(num_read).id);
      ASSERT_TRUE(pair.val == pairs.at(num_read).val);
    }
    ASSERT_TRUE(q.Dequeue(pair) == QErrorCode::kEmpty);
  });
  writer.join();
  reader.join();
}

TEST_F(CSpscTests, MultithreadedWriterReaderInducedSleepCheck) {  // NOLINT
  // To test the edge case where a new value is read twice by reader because the
  // reader reads just after the writer checks the cache, uncomment the sleep
  // statements in the Enqueue and Dequeue, and remove the || old value in the
  // test condition below.
  // Note: This test is setup in this manner as we (obviously) do not want to
  // induce sleeps in real runs. This test is not fully commented out to ensure
  // it is updated with other potential api changes.
  // To summarize when checking the edge case:
  // 1. Uncomment the sleep in the writer.
  // 2. Uncomment the for loop in the reader test logic below.
  // 3. Use the stricter version of the read test where id==1 is checked below.
  const int num_elements = 2;
  const std::array<CPair, num_elements> pairs{{{0, 323}, {0, 324}}};
  const int size = num_elements + 1;
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> q(size);
  using code_experiments::QErrorCode;
  std::thread writer([&pairs, &q]() {
    for (const auto &pair : pairs) {
      CHECK((q.Enqueue(pair) == QErrorCode::kOk));
    }
  });
  std::thread reader([&pairs, &q]() {
    CPair pair{0, 0};
    // Note: Uncomment the for loop if checking edge case
    // for (auto num_read = 0; num_read < num_elements; num_read++) {
    while (q.Dequeue(pair) == QErrorCode::kEmpty) {
      // No-op. Wait for producer if required.
    }
    // Note: Use the following two checks if and only if sleep is induced in
    // the writer to test the edge case mentioned above.
    // ASSERT_TRUE(pair.id == pairs.at(1).id);
    // ASSERT_TRUE(pair.val == pairs.at(1).val);
    // Note: Use the following two checks otherwise.
    ASSERT_TRUE(pair.id == pairs.at(1).id || pair.id == pairs.at(0).id);
    ASSERT_TRUE(pair.val == pairs.at(1).val || pair.val == pairs.at(0).val);
    LOG(INFO) << CPairAsString(pair);
    // }
  });
  writer.join();
  reader.join();
}

}  // namespace code_experiments