#include "ConsoleRenderer.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

ConsoleRenderer::ConsoleRenderer(OrderBook& orderBook, MatchingEngine& engine,
                                ThreadSafeQueue<Order>& queue)
    : orderBook_(orderBook), engine_(engine), queue_(queue), running_(true) {}

void ConsoleRenderer::run() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        render();
    }
}

void ConsoleRenderer::stop() {
    running_ = false;
}

void ConsoleRenderer::clearScreen() {
    // ANSI escape code to clear screen and move cursor to top-left
    std::cout << "\033[2J\033[1;1H";
}

void ConsoleRenderer::render() {
    clearScreen();

    std::cout << "╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        LIMIT ORDER BOOK - MARKET DEPTH            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // Get market data
    auto asks = orderBook_.getTopAsks(10);
    auto bids = orderBook_.getTopBids(10);
    auto lastTrade = engine_.getLastTrade();
    size_t queueSize = queue_.size();

    // Display header
    std::cout << std::setw(15) << "PRICE" << " │ "
              << std::setw(15) << "QUANTITY" << " │ "
              << std::setw(10) << "SIDE" << std::endl;
    std::cout << "────────────────┼─────────────────┼───────────" << std::endl;

    // Display asks (sellers) in reverse order (highest first for visual effect)
    std::vector<PriceLevel> reversedAsks(asks.rbegin(), asks.rend());
    for (const auto& level : reversedAsks) {
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) << level.price << " │ "
                  << std::setw(15) << level.totalQuantity << " │ "
                  << std::setw(10) << "ASK" << std::endl;
    }

    // Display spread line
    if (!asks.empty() && !bids.empty()) {
        double spread = asks[0].price - bids[0].price;
        std::cout << "────────────────┴─────────────────┴───────────" << std::endl;
        std::cout << "           SPREAD: " << std::fixed << std::setprecision(2)
                  << spread << std::endl;
        std::cout << "────────────────┬─────────────────┬───────────" << std::endl;
    } else {
        std::cout << "════════════════╪═════════════════╪═══════════" << std::endl;
    }

    // Display bids (buyers)
    for (const auto& level : bids) {
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) << level.price << " │ "
                  << std::setw(15) << level.totalQuantity << " │ "
                  << std::setw(10) << "BID" << std::endl;
    }

    std::cout << "────────────────┴─────────────────┴───────────" << std::endl;
    std::cout << std::endl;

    // Display last trade information
    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ LAST TRADE                                 │" << std::endl;
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    if (lastTrade.has_value()) {
        std::cout << "│ Price:    " << std::setw(10) << std::fixed << std::setprecision(2)
                  << lastTrade->price << "                       │" << std::endl;
        std::cout << "│ Quantity: " << std::setw(10) << lastTrade->quantity
                  << "                       │" << std::endl;
    } else {
        std::cout << "│ No trades executed yet                     │" << std::endl;
    }
    std::cout << "└────────────────────────────────────────────┘" << std::endl;
    std::cout << std::endl;

    // Display queue size
    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ SYSTEM STATUS                              │" << std::endl;
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ Pending Orders in Queue: " << std::setw(5) << queueSize
              << "              │" << std::endl;
    std::cout << "└────────────────────────────────────────────┘" << std::endl;
}
