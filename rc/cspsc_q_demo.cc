#include "cspsc_q.h"

#include <glog/logging.h>

#include <filesystem>

int main() {
  LOG(INFO) << "Simple consolidating single producer single consumer demo.";
  const int size = 10;
  using code_experiments::CPair;
  code_experiments::ConsolidatingSpscQ<CPair> queue(size);
  using code_experiments::QErrorCode;
  CHECK((queue.Enqueue(CPair{0, 323}) == QErrorCode::kOk));
  CHECK((queue.Enqueue(CPair{1, 100}) == QErrorCode::kOk));
  CHECK((queue.Enqueue(CPair{0, 324}) == QErrorCode::kOk));
  CPair ret_val{0, 0};
  CHECK(queue.Dequeue(ret_val) == QErrorCode::kOk);
  LOG(INFO) << code_experiments::CPairAsString(ret_val);
  return 0;
}
