#ifndef THREAD_SAFE_FIFO_H
#define THREAD_SAFE_FIFO_H

#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadSafeFIFO {
private:
    std::queue<std::vector<char>> queue;
    std::mutex mutex;
    std::condition_variable cond;
    const size_t max_size;

public:
    ThreadSafeFIFO(size_t size);
    void push(const std::vector<char>& item);
    std::vector<char> pop();
};

#endif //THREAD_SAFE_FIFO_H

