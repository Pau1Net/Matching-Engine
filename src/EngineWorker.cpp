#include "EngineWorker.h"
#include "Trade.h"
#include <iostream>
#include <iomanip>

EngineWorker::EngineWorker(ThreadSafeQueue<Order>& queue, MatchingEngine& engine)
    : queue_(queue), engine_(engine), running_(true) {}

void EngineWorker::run() {
    while (running_) {
        // Get order from queue (blocking)
        Order order = queue_.pop();

        // Process the order through matching engine
        auto trades = engine_.processOrder(order);

        // Log executed trades
        for (const auto& trade : trades) {
            std::cout << "[TRADE] "
                      << "BuyOrderID: " << trade.buyOrderId << " "
                      << "SellOrderID: " << trade.sellOrderId << " "
                      << "Price: " << std::fixed << std::setprecision(2) << trade.price << " "
                      << "Quantity: " << trade.quantity
                      << std::endl;
        }
    }
}

void EngineWorker::stop() {
    running_ = false;
}
