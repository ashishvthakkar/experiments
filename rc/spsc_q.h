#ifndef EXPERIMENTS_RC_SPSCQ_H
#define EXPERIMENTS_RC_SPSCQ_H

#include <glog/logging.h>

#include <atomic>
#include <vector>

namespace code_experiments {

// NOTE: This is a basic example of a potential single producer single consumer
// lockless queue in C++ for potential comparison with implementations in
// alternate languages. For production use, an implementation such as the one in
// Folly
// (https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h)
// may be better.
enum SpscQErrorCode { kOk = 0, kEmpty = -1, kFull = -2 };

template <typename T>
class SpscQ {
public:
  // Note: Available size is 1 less than size specified in constructor.
  explicit SpscQ(int size) : buffer_(size), head_(0), tail_(0) {
    CHECK(size >= 2) << "SpscQ size too small";
  }

  SpscQErrorCode Enqueue(T value) {
    auto next_head = head_ + 1;
    if (next_head >= buffer_.size()) next_head = 0;
    if (next_head == tail_) return kFull;
    buffer_[head_] = value;
    head_ = next_head;
    return kOk;
  }

  // NOTE: In this sample, a copy is performed. It would be a simple extension
  // to do a move instead.
  SpscQErrorCode Dequeue(T &return_value) {
    if (tail_ == head_) return kEmpty;
    return_value = buffer_[tail_];
    auto next_tail = tail_ + 1;
    if (next_tail >= buffer_.size()) next_tail = 0;
    tail_ = next_tail;
    return kOk;
  }

private:
  std::vector<T> buffer_;
  std::atomic<int> head_;
  std::atomic<int> tail_;
};

template class SpscQ<double>;

}  // namespace code_experiments

#endif