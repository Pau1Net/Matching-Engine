#ifndef ORDERPRODUCER_H
#define ORDERPRODUCER_H

#include "Order.h"
#include "ThreadSafeQueue.h"
#include <atomic>
#include <memory>

enum class ProducerMode {
    Random,
    Stdin
};

class OrderProducer {
public:
    OrderProducer(ThreadSafeQueue<Order>& queue, ProducerMode mode);

    // Run the producer thread
    void run();

    // Stop the producer
    void stop();

private:
    ThreadSafeQueue<Order>& queue_;
    ProducerMode mode_;
    std::atomic<bool> running_;
    std::atomic<uint64_t> nextOrderId_;

    // Generate a random order
    Order generateRandomOrder();

    // Read an order from stdin
    bool readOrderFromStdin(Order& order);
};

#endif // ORDERPRODUCER_H
