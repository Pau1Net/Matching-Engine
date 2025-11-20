#ifndef TRADE_H
#define TRADE_H

#include <cstdint>
#include <chrono>

struct Trade {
    uint64_t buyOrderId;
    uint64_t sellOrderId;
    double price;
    uint64_t quantity;
    std::chrono::steady_clock::time_point timestamp;

    Trade(uint64_t buyId, uint64_t sellId, double p, uint64_t qty)
        : buyOrderId(buyId), sellOrderId(sellId), price(p), quantity(qty),
          timestamp(std::chrono::steady_clock::now()) {}
};

#endif // TRADE_H
