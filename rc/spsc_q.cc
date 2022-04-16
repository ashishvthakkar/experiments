#include <glog/logging.h>

#include <atomic>
#include <vector>

// NOTE: This is a basic example of a potential single producer single consumer
// lockless queue in C++ for potential comparison with implementations in
// alternate languages. For production use, an implementation such as the one in
// Folly
// (https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h)
// may be better.
class SpscQ {
public:
  enum ErrorCode { kOk = 0, kEmpty = -1, kFull = -2 };

  // Note: Available size is 1 less than size specified in constructor.
  explicit SpscQ(int size) : buffer_(size), head_(0), tail_(0) {
    CHECK(size >= 2) << "SpscQ size too small";
  }

  ErrorCode Enqueue(double value) {
    auto next_head = head_ + 1;
    if (next_head >= buffer_.size()) next_head = 0;
    if (next_head == tail_) return kFull;
    buffer_[head_] = value;
    head_ = next_head;
    return kOk;
  }

  ErrorCode Dequeue(double &return_value) {
    if (tail_ == head_) return kEmpty;
    return_value = buffer_[tail_];
    auto next_tail = tail_ + 1;
    if (next_tail >= buffer_.size()) next_tail = 0;
    tail_ = next_tail;
    return kOk;
  }

private:
  std::vector<double> buffer_;
  std::atomic<int> head_;
  std::atomic<int> tail_;
};

int main() {
  LOG(INFO) << "Simple single producer single consumer demo.";
  const int size = 3;
  SpscQ q(size);
  CHECK((q.Enqueue(10) == SpscQ::kOk));
  CHECK((q.Enqueue(12) == SpscQ::kOk));
  CHECK((q.Enqueue(15) == SpscQ::kFull));
  double ret_val = 0;
  CHECK(q.Dequeue(ret_val) == SpscQ::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQ::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQ::kEmpty);
  CHECK((q.Enqueue(15) == SpscQ::kOk));
  CHECK(q.Dequeue(ret_val) == SpscQ::kOk);
  LOG(INFO) << ret_val;
  CHECK(q.Dequeue(ret_val) == SpscQ::kEmpty);
  return 0;
}
