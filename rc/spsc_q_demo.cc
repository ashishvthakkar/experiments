#include "spsc_q.h"

#include <glog/logging.h>

int main() {
  LOG(INFO) << "Simple single producer single consumer demo.";
  const int size = 3;
  code_experiments::SpscQ<double> queue(size);
  using code_experiments::QErrorCode;
  CHECK((queue.Enqueue(10) == QErrorCode::kOk));
  CHECK((queue.Enqueue(12) == QErrorCode::kOk));
  double ret_val = 0;
  CHECK(queue.Dequeue(ret_val) == QErrorCode::kOk);
  LOG(INFO) << ret_val;
  return 0;
}
