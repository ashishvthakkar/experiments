#include <glog/logging.h>
#include <gtest/gtest.h>

#include "spsc_q.h"

namespace code_experiments {

class SpscTests : public ::testing::Test {};

TEST_F(SpscTests, EnqDeqCheck) {  // NOLINT
  const int size = 3;
  code_experiments::SpscQ<double> q(size);
  using code_experiments::SpscQErrorCode;
  CHECK((q.Enqueue(10) == SpscQErrorCode::kOk));
  CHECK((q.Enqueue(12) == SpscQErrorCode::kOk));
  CHECK((q.Enqueue(15) == SpscQErrorCode::kFull));
  double ret_val = 0;
  CHECK(q.Dequeue(ret_val) == SpscQErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQErrorCode::kEmpty);
  CHECK((q.Enqueue(15) == SpscQErrorCode::kOk));
  CHECK(q.Dequeue(ret_val) == SpscQErrorCode::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQErrorCode::kEmpty);
}

}  // namespace code_experiments