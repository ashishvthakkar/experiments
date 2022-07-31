#ifndef EXPERIMENTS_RC_CSPSCQ_H
#define EXPERIMENTS_RC_CSPSCQ_H

#include <glog/logging.h>

#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "spsc_q.h"

namespace code_experiments {

// TODO(ashish): Confirm size is within machine work ensuring atomic read/write
// or protect with std::atomic<Pair>
struct CPair {
  int id;
  int val;
};

inline std::string CPairAsString(const CPair &pair) {
  return std::to_string(pair.id) + ": " + std::to_string(pair.val);
}

template <typename T>
class ConsolidatingSpscQ {
public:
  // Note: Available size is 1 less than size specified in constructor.
  explicit ConsolidatingSpscQ(int size)
      : buffer_(size),
        head_(0),
        tail_(0),
        erase_q_(size) {
    CHECK(size >= 2) << "Q size too small";
  }

  QErrorCode Enqueue(T pair) {
    auto potential_match = id_to_index_.find(pair.id);
    if (potential_match != id_to_index_.end()) {
      // NOTE: As this is a single producer version, the below does not conflict
      // with a different head write from a non-cached scenario.
      buffer_[potential_match->second] = pair;
      // If read occurs between finding the match and writing the value, add
      // value to the queue as if this is a non-cached scenario. A rare race
      // condition exists where the reader gets two notifications with the
      // refereshed value - while sub-optimal that has been considered
      // acceptable.
      // TODO(ashish): Confirm whether the below comparison works as expected.
      auto erase_id = 0;
      while (erase_q_.Dequeue(erase_id) == kOk) {
        id_to_index_.erase(erase_id);
      }
      if (id_to_index_.find(pair.id) == potential_match) {
        return kOk;
      }
    }
    auto next_head = head_ + 1;
    if (next_head >= buffer_.size()) next_head = 0;
    if (next_head == tail_) return kFull;
    buffer_[head_] = pair;
    id_to_index_[pair.id] = head_;
    head_ = next_head;
    return kOk;
  }

  // NOTE: In this sample, a copy is performed. It would be a simple extension
  // to do a move instead.
  QErrorCode Dequeue(T &return_value) {
    if (tail_ == head_) return kEmpty;
    // Queue an erase. Not erasing directly to prevent invalidation of any
    // iterators in use in the producer.
    erase_q_.Enqueue(buffer_[tail_].id);
    // NOTE: Above statement is expected to be executed before the below.
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
  std::unordered_map<int, int> id_to_index_;
  SpscQ<int> erase_q_;
};

template class ConsolidatingSpscQ<CPair>;

}  // namespace code_experiments

#endif