#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "OrderBook.h"
#include "MatchingEngine.h"
#include "ThreadSafeQueue.h"
#include "Order.h"
#include <atomic>

class ConsoleRenderer {
public:
    ConsoleRenderer(OrderBook& orderBook, MatchingEngine& engine,
                   ThreadSafeQueue<Order>& queue);

    // Run the renderer thread
    void run();

    // Stop the renderer
    void stop();

private:
    OrderBook& orderBook_;
    MatchingEngine& engine_;
    ThreadSafeQueue<Order>& queue_;
    std::atomic<bool> running_;

    // Clear the console screen
    void clearScreen();

    // Render the order book
    void render();
};

#endif // CONSOLERENDERER_H
