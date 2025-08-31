#include "thread_safe_fifo.h"

ThreadSafeFIFO::ThreadSafeFIFO(size_t size) : max_size(size) {}

void ThreadSafeFIFO::push(const std::vector<char>& item) {
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this] { return queue.size() < max_size; });
    queue.push(item);
    lock.unlock();
    cond.notify_one();
}

std::vector<char> ThreadSafeFIFO::pop() {
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this] { return !queue.empty(); });
    std::vector<char> item = std::move(queue.front());
    queue.pop();
    lock.unlock();
    cond.notify_one();
    return item;
}

