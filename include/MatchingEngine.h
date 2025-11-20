#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include "Order.h"
#include "OrderBook.h"
#include "Trade.h"
#include <vector>
#include <optional>

class MatchingEngine {
public:
    MatchingEngine(OrderBook& orderBook);

    // Process an incoming order and return executed trades
    std::vector<Trade> processOrder(Order order);

    // Get the last executed trade
    std::optional<Trade> getLastTrade() const;

private:
    OrderBook& orderBook_;
    std::optional<Trade> lastTrade_;
    mutable std::mutex mutex_;

    // Check if a buy order can match with best ask
    bool canMatchBuy(const Order& buyOrder, const Order& askOrder) const;

    // Check if a sell order can match with best bid
    bool canMatchSell(const Order& sellOrder, const Order& bidOrder) const;
};

#endif // MATCHINGENGINE_H
