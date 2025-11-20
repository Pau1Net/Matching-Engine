#include "MatchingEngine.h"
#include <algorithm>

MatchingEngine::MatchingEngine(OrderBook& orderBook)
    : orderBook_(orderBook) {}

bool MatchingEngine::canMatchBuy(const Order& buyOrder, const Order& askOrder) const {
    return buyOrder.getPrice() >= askOrder.getPrice();
}

bool MatchingEngine::canMatchSell(const Order& sellOrder, const Order& bidOrder) const {
    return sellOrder.getPrice() <= bidOrder.getPrice();
}

std::vector<Trade> MatchingEngine::processOrder(Order order) {
    std::vector<Trade> trades;
    uint64_t remainingQuantity = order.getQuantity();

    if (order.getSide() == OrderSide::Buy) {
        // Match against asks (sells)
        while (remainingQuantity > 0) {
            auto bestAsk = orderBook_.getBestAsk();
            if (!bestAsk.has_value() || !canMatchBuy(order, bestAsk.value())) {
                break;
            }

            uint64_t matchQuantity = std::min(remainingQuantity, bestAsk->getQuantity());
            double tradePrice = bestAsk->getPrice(); // Passive order price

            Trade trade(order.getId(), bestAsk->getId(), tradePrice, matchQuantity);
            trades.push_back(trade);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                lastTrade_ = trade;
            }

            orderBook_.removeAskQuantity(matchQuantity);
            remainingQuantity -= matchQuantity;
        }
    } else {
        // Match against bids (buys)
        while (remainingQuantity > 0) {
            auto bestBid = orderBook_.getBestBid();
            if (!bestBid.has_value() || !canMatchSell(order, bestBid.value())) {
                break;
            }

            uint64_t matchQuantity = std::min(remainingQuantity, bestBid->getQuantity());
            double tradePrice = bestBid->getPrice(); // Passive order price

            Trade trade(bestBid->getId(), order.getId(), tradePrice, matchQuantity);
            trades.push_back(trade);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                lastTrade_ = trade;
            }

            orderBook_.removeBidQuantity(matchQuantity);
            remainingQuantity -= matchQuantity;
        }
    }

    // If there's remaining quantity, add to order book
    if (remainingQuantity > 0) {
        Order remainingOrder = order;
        remainingOrder.setQuantity(remainingQuantity);
        orderBook_.addOrder(remainingOrder);
    }

    return trades;
}

std::optional<Trade> MatchingEngine::getLastTrade() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastTrade_;
}
