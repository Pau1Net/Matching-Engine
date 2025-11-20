#include "OrderProducer.h"
#include <random>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

OrderProducer::OrderProducer(ThreadSafeQueue<Order>& queue, ProducerMode mode)
    : queue_(queue), mode_(mode), running_(true), nextOrderId_(1) {}

void OrderProducer::run() {
    if (mode_ == ProducerMode::Random) {
        // Random mode: generate orders continuously
        while (running_) {
            Order order = generateRandomOrder();
            queue_.push(order);

            // Sleep for a short time to simulate realistic order flow
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        // Stdin mode: read orders from standard input
        std::cout << "Enter orders in format: <BUY|SELL> <price> <quantity>" << std::endl;
        std::cout << "Example: BUY 100.50 1000" << std::endl;
        std::cout << "Type 'quit' to exit" << std::endl;

        while (running_) {
            Order order(0, OrderSide::Buy, 0.0, 0);
            if (readOrderFromStdin(order)) {
                queue_.push(order);
            } else {
                break;
            }
        }
    }
}

void OrderProducer::stop() {
    running_ = false;
}

Order OrderProducer::generateRandomOrder() {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Random side (50/50 buy/sell)
    std::uniform_int_distribution<> sideDist(0, 1);
    OrderSide side = (sideDist(gen) == 0) ? OrderSide::Buy : OrderSide::Sell;

    // Random price between 95.0 and 105.0
    std::uniform_real_distribution<> priceDist(95.0, 105.0);
    double price = std::round(priceDist(gen) * 100.0) / 100.0; // Round to 2 decimals

    // Random quantity between 100 and 10000
    std::uniform_int_distribution<> qtyDist(100, 10000);
    uint64_t quantity = qtyDist(gen);

    uint64_t orderId = nextOrderId_++;

    return Order(orderId, side, price, quantity);
}

bool OrderProducer::readOrderFromStdin(Order& order) {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return false;
    }

    if (line == "quit" || line == "exit") {
        return false;
    }

    std::istringstream iss(line);
    std::string sideStr;
    double price;
    uint64_t quantity;

    if (!(iss >> sideStr >> price >> quantity)) {
        std::cerr << "Invalid format. Use: <BUY|SELL> <price> <quantity>" << std::endl;
        return true; // Continue reading
    }

    OrderSide side;
    if (sideStr == "BUY" || sideStr == "buy") {
        side = OrderSide::Buy;
    } else if (sideStr == "SELL" || sideStr == "sell") {
        side = OrderSide::Sell;
    } else {
        std::cerr << "Invalid side. Use BUY or SELL" << std::endl;
        return true;
    }

    uint64_t orderId = nextOrderId_++;
    order = Order(orderId, side, price, quantity);

    return true;
}
