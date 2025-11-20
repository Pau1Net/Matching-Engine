#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <chrono>

enum class OrderSide {
    Buy,
    Sell
};

class Order {
public:
    Order(uint64_t id, OrderSide side, double price, uint64_t quantity);

    uint64_t getId() const { return id_; }
    OrderSide getSide() const { return side_; }
    double getPrice() const { return price_; }
    uint64_t getQuantity() const { return quantity_; }
    std::chrono::steady_clock::time_point getTimestamp() const { return timestamp_; }

    void setQuantity(uint64_t quantity) { quantity_ = quantity; }

    bool operator<(const Order& other) const;

private:
    uint64_t id_;
    OrderSide side_;
    double price_;
    uint64_t quantity_;
    std::chrono::steady_clock::time_point timestamp_;
};

#endif // ORDER_H
