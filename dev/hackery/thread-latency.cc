#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>

const int EOF_MARKER = -1;
const int NUM_ITEMS = 1000000;

struct PipelineStage {
  std::mutex mtx;
  std::condition_variable cv;
  std::optional<int> data;
  int n_ops = 0;
};

void threadFunction(int id, PipelineStage* inStage, PipelineStage* outStage) {
  while (true) {
    std::unique_lock<std::mutex> lk(inStage->mtx);
    inStage->cv.wait(lk, [&]{ return inStage->data.has_value(); });

    int value = inStage->data.value();
    inStage->data.reset();
    lk.unlock();
    inStage->cv.notify_one();

    ++inStage->n_ops;

    if (value == EOF_MARKER) {
      break;
    }

    if (outStage) {
      std::unique_lock<std::mutex> outLk(outStage->mtx);
      outStage->data = value + 1;
      outLk.unlock();
      outStage->cv.notify_one();
    }
  }

  if (outStage) {
    std::unique_lock<std::mutex> outLk(outStage->mtx);
    outStage->data = EOF_MARKER;
    outLk.unlock();
    outStage->cv.notify_one();
  }
}

void measureLatency(int numThreads) {
  std::vector<PipelineStage> stages(numThreads);
  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back(threadFunction, i, &stages[i], i < numThreads - 1 ? &stages[i + 1] : nullptr);
  }

  auto startTime = std::chrono::high_resolution_clock::now();

  {
    std::unique_lock<std::mutex> lk(stages[0].mtx);
    stages[0].data = 0;
  }
  stages[0].cv.notify_one();

  for (int i = 1; i < NUM_ITEMS / numThreads; ++i) {
    std::unique_lock<std::mutex> lk(stages[0].mtx);
    stages[0].cv.wait(lk, [&]{ return !stages[0].data.has_value(); });
    stages[0].data = i;
    lk.unlock();
    stages[0].cv.notify_one();
  }

  {
    std::unique_lock<std::mutex> lk(stages[0].mtx);
    stages[0].cv.wait(lk, [&]{ return !stages[0].data.has_value(); });
    stages[0].data = EOF_MARKER;
    lk.unlock();
    stages[0].cv.notify_one();
  }

  for (auto& t : threads) {
    t.join();
  }

  int n_ops = 0;
  for (auto &s : stages) n_ops += s.n_ops;

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::nano> diff = endTime - startTime;
  std::cout << "Total time for " << numThreads
            << " threads: " << diff.count() / 1000000 << "ms"
            << "; ns/op = " << diff.count() / n_ops << std::endl;
}

int main() {
  for (int n = 2; n <= 100000; n = n * 1.05 + 1) {
    measureLatency(n);
  }

  return 0;
}
