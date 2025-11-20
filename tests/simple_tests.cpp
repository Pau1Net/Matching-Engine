#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include <iostream>
#include <string>
#include <cmath>

// Simple test framework
#define ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Expected " << (expected) << " but got " << (actual) << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Condition '" #condition "' is false" << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Condition '" #condition "' is true" << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_DOUBLE_EQUAL(expected, actual, epsilon) \
    do { \
        if (std::abs((expected) - (actual)) > (epsilon)) { \
            std::cerr << "FAILED: " << __FILE__ << ":" << __LINE__ \
                      << " Expected " << (expected) << " but got " << (actual) << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        std::cout << "Running " << #test_func << "... "; \
        if (test_func()) { \
            std::cout << "PASSED" << std::endl; \
            passedTests++; \
        } else { \
            failedTests++; \
        } \
        totalTests++; \
    } while(0)

// Test functions

bool test_order_creation() {
    Order order(1, OrderSide::Buy, 100.50, 1000);

    ASSERT_EQUAL(1u, order.getId());
    ASSERT_TRUE(order.getSide() == OrderSide::Buy);
    ASSERT_DOUBLE_EQUAL(100.50, order.getPrice(), 0.01);
    ASSERT_EQUAL(1000u, order.getQuantity());

    return true;
}

bool test_order_modification() {
    Order order(2, OrderSide::Sell, 99.75, 500);
    order.setQuantity(750);

    ASSERT_EQUAL(750u, order.getQuantity());

    return true;
}

bool test_orderbook_add_bid() {
    OrderBook book;
    Order bid(1, OrderSide::Buy, 100.00, 1000);

    book.addOrder(bid);
    auto bestBid = book.getBestBid();

    ASSERT_TRUE(bestBid.has_value());
    ASSERT_DOUBLE_EQUAL(100.00, bestBid->getPrice(), 0.01);
    ASSERT_EQUAL(1000u, bestBid->getQuantity());

    return true;
}

bool test_orderbook_add_ask() {
    OrderBook book;
    Order ask(1, OrderSide::Sell, 101.00, 500);

    book.addOrder(ask);
    auto bestAsk = book.getBestAsk();

    ASSERT_TRUE(bestAsk.has_value());
    ASSERT_DOUBLE_EQUAL(101.00, bestAsk->getPrice(), 0.01);
    ASSERT_EQUAL(500u, bestAsk->getQuantity());

    return true;
}

bool test_orderbook_price_priority_bids() {
    OrderBook book;
    Order bid1(1, OrderSide::Buy, 99.00, 100);
    Order bid2(2, OrderSide::Buy, 100.00, 200);
    Order bid3(3, OrderSide::Buy, 98.00, 300);

    book.addOrder(bid1);
    book.addOrder(bid2);
    book.addOrder(bid3);

    auto bestBid = book.getBestBid();
    ASSERT_TRUE(bestBid.has_value());
    ASSERT_DOUBLE_EQUAL(100.00, bestBid->getPrice(), 0.01);

    return true;
}

bool test_orderbook_price_priority_asks() {
    OrderBook book;
    Order ask1(1, OrderSide::Sell, 102.00, 100);
    Order ask2(2, OrderSide::Sell, 101.00, 200);
    Order ask3(3, OrderSide::Sell, 103.00, 300);

    book.addOrder(ask1);
    book.addOrder(ask2);
    book.addOrder(ask3);

    auto bestAsk = book.getBestAsk();
    ASSERT_TRUE(bestAsk.has_value());
    ASSERT_DOUBLE_EQUAL(101.00, bestAsk->getPrice(), 0.01);

    return true;
}

bool test_matching_engine_full_match() {
    OrderBook book;
    MatchingEngine engine(book);

    // Add a sell order to the book
    Order ask(1, OrderSide::Sell, 100.00, 1000);
    book.addOrder(ask);

    // Match with a buy order
    Order bid(2, OrderSide::Buy, 100.00, 1000);
    auto trades = engine.processOrder(bid);

    ASSERT_EQUAL(1u, trades.size());
    ASSERT_DOUBLE_EQUAL(100.00, trades[0].price, 0.01);
    ASSERT_EQUAL(1000u, trades[0].quantity);

    // Order book should be empty after full match
    auto bestAsk = book.getBestAsk();
    ASSERT_FALSE(bestAsk.has_value());

    return true;
}

bool test_matching_engine_partial_match() {
    OrderBook book;
    MatchingEngine engine(book);

    // Add a sell order to the book
    Order ask(1, OrderSide::Sell, 100.00, 1000);
    book.addOrder(ask);

    // Match with a smaller buy order
    Order bid(2, OrderSide::Buy, 100.00, 400);
    auto trades = engine.processOrder(bid);

    ASSERT_EQUAL(1u, trades.size());
    ASSERT_EQUAL(400u, trades[0].quantity);

    // Remaining quantity should be in the book
    auto bestAsk = book.getBestAsk();
    ASSERT_TRUE(bestAsk.has_value());
    ASSERT_EQUAL(600u, bestAsk->getQuantity());

    return true;
}

bool test_matching_engine_no_match() {
    OrderBook book;
    MatchingEngine engine(book);

    // Add a sell order at 101
    Order ask(1, OrderSide::Sell, 101.00, 1000);
    book.addOrder(ask);

    // Try to buy at 100 (price too low)
    Order bid(2, OrderSide::Buy, 100.00, 500);
    auto trades = engine.processOrder(bid);

    ASSERT_EQUAL(0u, trades.size());

    // Both orders should be in the book
    auto bestAsk = book.getBestAsk();
    auto bestBid = book.getBestBid();
    ASSERT_TRUE(bestAsk.has_value());
    ASSERT_TRUE(bestBid.has_value());

    return true;
}

bool test_matching_engine_multiple_levels() {
    OrderBook book;
    MatchingEngine engine(book);

    // Add multiple sell orders
    Order ask1(1, OrderSide::Sell, 100.00, 300);
    Order ask2(2, OrderSide::Sell, 101.00, 400);
    Order ask3(3, OrderSide::Sell, 102.00, 500);
    book.addOrder(ask1);
    book.addOrder(ask2);
    book.addOrder(ask3);

    // Large buy order that should match multiple levels
    Order bid(4, OrderSide::Buy, 101.50, 800);
    auto trades = engine.processOrder(bid);

    // Should have 2 trades: 300 @ 100.00, 400 @ 101.00
    ASSERT_EQUAL(2u, trades.size());
    ASSERT_EQUAL(300u, trades[0].quantity);
    ASSERT_EQUAL(400u, trades[1].quantity);

    return true;
}

int main() {
    std::cout << "═══════════════════════════════════════" << std::endl;
    std::cout << "   LIMIT ORDER BOOK - TEST SUITE" << std::endl;
    std::cout << "═══════════════════════════════════════" << std::endl;
    std::cout << std::endl;

    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;

    // Run all tests
    RUN_TEST(test_order_creation);
    RUN_TEST(test_order_modification);
    RUN_TEST(test_orderbook_add_bid);
    RUN_TEST(test_orderbook_add_ask);
    RUN_TEST(test_orderbook_price_priority_bids);
    RUN_TEST(test_orderbook_price_priority_asks);
    RUN_TEST(test_matching_engine_full_match);
    RUN_TEST(test_matching_engine_partial_match);
    RUN_TEST(test_matching_engine_no_match);
    RUN_TEST(test_matching_engine_multiple_levels);

    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════" << std::endl;
    std::cout << "Total tests:  " << totalTests << std::endl;
    std::cout << "Passed:       " << passedTests << std::endl;
    std::cout << "Failed:       " << failedTests << std::endl;
    std::cout << "═══════════════════════════════════════" << std::endl;

    return (failedTests == 0) ? 0 : 1;
}
