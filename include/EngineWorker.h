#ifndef ENGINEWORKER_H
#define ENGINEWORKER_H

#include "ThreadSafeQueue.h"
#include "Order.h"
#include "MatchingEngine.h"
#include <atomic>

class EngineWorker {
public:
    EngineWorker(ThreadSafeQueue<Order>& queue, MatchingEngine& engine);

    // Run the engine worker thread
    void run();

    // Stop the worker
    void stop();

private:
    ThreadSafeQueue<Order>& queue_;
    MatchingEngine& engine_;
    std::atomic<bool> running_;
};

#endif // ENGINEWORKER_H
