#include "OrderBook.h"
#include <algorithm>

void OrderBook::addOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (order.getSide() == OrderSide::Buy) {
        auto& level = bids_[order.getPrice()];
        level.price = order.getPrice();
        level.orders.push(order);
        level.totalQuantity += order.getQuantity();
    } else {
        auto& level = asks_[order.getPrice()];
        level.price = order.getPrice();
        level.orders.push(order);
        level.totalQuantity += order.getQuantity();
    }
}

std::optional<Order> OrderBook::getBestBid() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (bids_.empty()) {
        return std::nullopt;
    }

    auto& level = bids_.begin()->second;
    if (level.orders.empty()) {
        return std::nullopt;
    }

    return level.orders.front();
}

std::optional<Order> OrderBook::getBestAsk() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (asks_.empty()) {
        return std::nullopt;
    }

    auto& level = asks_.begin()->second;
    if (level.orders.empty()) {
        return std::nullopt;
    }

    return level.orders.front();
}

void OrderBook::removeBidQuantity(uint64_t quantity) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (bids_.empty()) return;

    auto it = bids_.begin();
    auto& level = it->second;

    if (level.orders.empty()) return;

    auto& order = level.orders.front();
    if (order.getQuantity() <= quantity) {
        level.totalQuantity -= order.getQuantity();
        level.orders.pop();
        if (level.orders.empty()) {
            bids_.erase(it);
        }
    } else {
        Order updatedOrder = order;
        updatedOrder.setQuantity(order.getQuantity() - quantity);
        level.orders.pop();
        level.orders.push(updatedOrder);
        level.totalQuantity -= quantity;
    }
}

void OrderBook::removeAskQuantity(uint64_t quantity) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (asks_.empty()) return;

    auto it = asks_.begin();
    auto& level = it->second;

    if (level.orders.empty()) return;

    auto& order = level.orders.front();
    if (order.getQuantity() <= quantity) {
        level.totalQuantity -= order.getQuantity();
        level.orders.pop();
        if (level.orders.empty()) {
            asks_.erase(it);
        }
    } else {
        Order updatedOrder = order;
        updatedOrder.setQuantity(order.getQuantity() - quantity);
        level.orders.pop();
        level.orders.push(updatedOrder);
        level.totalQuantity -= quantity;
    }
}

std::vector<PriceLevel> OrderBook::getTopBids(size_t n) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<PriceLevel> result;
    result.reserve(n);

    size_t count = 0;
    for (const auto& [price, level] : bids_) {
        if (count >= n) break;
        if (!level.orders.empty()) {
            result.push_back(level);
            count++;
        }
    }

    return result;
}

std::vector<PriceLevel> OrderBook::getTopAsks(size_t n) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<PriceLevel> result;
    result.reserve(n);

    size_t count = 0;
    for (const auto& [price, level] : asks_) {
        if (count >= n) break;
        if (!level.orders.empty()) {
            result.push_back(level);
            count++;
        }
    }

    return result;
}
