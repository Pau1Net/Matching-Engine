#include "Order.h"

Order::Order(uint64_t id, OrderSide side, double price, uint64_t quantity)
    : id_(id), side_(side), price_(price), quantity_(quantity),
      timestamp_(std::chrono::steady_clock::now()) {}

bool Order::operator<(const Order& other) const {
    // For time priority: earlier timestamp is better
    return timestamp_ < other.timestamp_;
}
