#include <glog/logging.h>

#include <chrono>
#include <future>
#include <thread>

void RunWithMonitor() {
  const auto num_tasks = 5;
  std::atomic<int> tasks_completed = 0;
  std::vector<std::future<void>> futures;
  futures.reserve(num_tasks);
  for (auto i = 0; i < num_tasks; i++) {
    futures.push_back(
        std::async(std::launch::async, [num_tasks, &tasks_completed, i]() {
          const auto processing_time = i + 1;
          std::this_thread::sleep_for(std::chrono::seconds(processing_time));
          tasks_completed++;
        }));
  }
  while (tasks_completed < num_tasks) {
    LOG(INFO) << tasks_completed << " / " << num_tasks << " complete";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  LOG(INFO) << "All tasks complete.";
}

int main() {
  LOG(INFO) << "Simple progress monitor.";
  RunWithMonitor();
  return 0;
}