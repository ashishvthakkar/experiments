#include <glog/logging.h>
#include <gtest/gtest.h>

#include "spsc_q.h"

namespace code_experiments {

class SpscTests : public ::testing::Test {};

TEST_F(SpscTests, EnqDeqCheck) {  // NOLINT
  const int size = 3;
  code_experiments::SpscQ<double> q(size);
  using code_experiments::QErrorCode;
  CHECK((q.Enqueue(10) == QErrorCode::kOk));
  CHECK((q.Enqueue(12) == QErrorCode::kOk));
  CHECK((q.Enqueue(15) == QErrorCode::kFull));
  double ret_val = 0;
  CHECK(q.Dequeue(ret_val) == QErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == QErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == QErrorCode::kEmpty);
  CHECK((q.Enqueue(15) == QErrorCode::kOk));
  CHECK(q.Dequeue(ret_val) == QErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == QErrorCode::kEmpty);
}

}  // namespace code_experiments