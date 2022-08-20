#ifndef EXPERIMENTS_RC_CSPSCQ_H
#define EXPERIMENTS_RC_CSPSCQ_H

#include <glog/logging.h>

#include <atomic>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "spsc_q.h"

namespace code_experiments {

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
const int kMachineWordSize = 8;
#else
const int kMachineWordSize = 4;
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
const int kMachineWordSize = 8;
#else
const int kMachineWordSize = 4;
#endif
#endif

// Note: The beloe checks for min machine word size of 8, ensures alignment to
// 8-byte boundary, and expects that reads and write of the 8-byte value are
// done atomically. If required, these can be protected through std::atomic.
struct alignas(alignof(int64_t)) CPair {
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
    CHECK(size >= 2) << "Queue size too small";
    // Note: The below checks can likely be converted to be at compile time.
    CHECK(sizeof(void *) >= kMinMachineWordSize)
        << "Min machined word size of 64-bit (or larger) expected";
    CHECK(kMachineWordSize >= kMinMachineWordSize)
        << "Min machined word size of 64-bit (or larger) expected";
    CHECK(alignof(T) == kMinMachineWordSize) << "Unexpected alignment";
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
      // Note: Uncomment the below sleep statement to test the edge case where
      // the reader reads just after the writer checks the cache. Additional
      // comments in test "MultithreadedWriterReaderInducedSleepCheck".
      // std::this_thread::sleep_for(std::chrono::seconds(1));
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

  const int kMinMachineWordSize = 8;
};

template class ConsolidatingSpscQ<CPair>;

}  // namespace code_experiments

#endif