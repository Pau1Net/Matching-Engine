#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <map>
#include <queue>
#include <vector>
#include <mutex>

struct PriceLevel {
    double price;
    uint64_t totalQuantity;
    std::queue<Order> orders;

    PriceLevel(double p = 0.0) : price(p), totalQuantity(0) {}
};

class OrderBook {
public:
    OrderBook() = default;

    // Add an order to the book
    void addOrder(const Order& order);

    // Get best bid (highest buy price)
    std::optional<Order> getBestBid();

    // Get best ask (lowest sell price)
    std::optional<Order> getBestAsk();

    // Remove quantity from best bid
    void removeBidQuantity(uint64_t quantity);

    // Remove quantity from best ask
    void removeAskQuantity(uint64_t quantity);

    // Get top N bid levels for display
    std::vector<PriceLevel> getTopBids(size_t n) const;

    // Get top N ask levels for display
    std::vector<PriceLevel> getTopAsks(size_t n) const;

    // Thread-safe access
    std::mutex& getMutex() { return mutex_; }

private:
    // Bids: higher price is better (descending order)
    std::map<double, PriceLevel, std::greater<double>> bids_;

    // Asks: lower price is better (ascending order)
    std::map<double, PriceLevel, std::less<double>> asks_;

    mutable std::mutex mutex_;
};

#endif // ORDERBOOK_H
