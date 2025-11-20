#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include "ThreadSafeQueue.h"
#include "OrderProducer.h"
#include "EngineWorker.h"
#include "ConsoleRenderer.h"
#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>
#include <string>

// Global flag for graceful shutdown
std::atomic<bool> g_shutdownRequested(false);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received (" << signum << "). Exiting gracefully..." << std::endl;
    g_shutdownRequested = true;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [--mode=<random|stdin>]" << std::endl;
    std::cout << "  --mode=random  : Generate random orders automatically (default)" << std::endl;
    std::cout << "  --mode=stdin   : Read orders from standard input" << std::endl;
    std::cout << std::endl;
    std::cout << "Stdin format: <BUY|SELL> <price> <quantity>" << std::endl;
    std::cout << "Example: BUY 100.50 1000" << std::endl;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    ProducerMode mode = ProducerMode::Random;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg.substr(0, 7) == "--mode=") {
            std::string modeStr = arg.substr(7);
            if (modeStr == "random") {
                mode = ProducerMode::Random;
            } else if (modeStr == "stdin") {
                mode = ProducerMode::Stdin;
            } else {
                std::cerr << "Invalid mode: " << modeStr << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Set up signal handler for graceful shutdown (Ctrl+C)
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "Starting Limit Order Book Matching Engine..." << std::endl;
    std::cout << "Mode: " << (mode == ProducerMode::Random ? "Random" : "Stdin") << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;

    // Wait a moment before starting
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Create core components
    ThreadSafeQueue<Order> orderQueue;
    OrderBook orderBook;
    MatchingEngine matchingEngine(orderBook);

    // Create worker objects
    OrderProducer producer(orderQueue, mode);
    EngineWorker engineWorker(orderQueue, matchingEngine);
    ConsoleRenderer renderer(orderBook, matchingEngine, orderQueue);

    // Launch threads
    std::thread producerThread([&producer]() { producer.run(); });
    std::thread engineThread([&engineWorker]() { engineWorker.run(); });
    std::thread rendererThread([&renderer]() { renderer.run(); });

    // Wait for shutdown signal
    while (!g_shutdownRequested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Graceful shutdown
    std::cout << "Shutting down threads..." << std::endl;
    producer.stop();
    engineWorker.stop();
    renderer.stop();

    // Push a dummy order to unblock the engine thread
    Order dummyOrder(0, OrderSide::Buy, 0.0, 0);
    orderQueue.push(dummyOrder);

    // Join threads
    if (producerThread.joinable()) {
        producerThread.join();
    }
    if (engineThread.joinable()) {
        engineThread.join();
    }
    if (rendererThread.joinable()) {
        rendererThread.join();
    }

    std::cout << "Shutdown complete." << std::endl;

    return 0;
}
