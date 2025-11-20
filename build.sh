#!/bin/bash

# Limit Order Book - Build Script

echo "Building Limit Order Book Matching Engine..."
echo ""

# Compile main executable
echo "[1/2] Building main executable..."
clang++ -std=c++17 -Iinclude -pthread \
  src/Order.cpp \
  src/OrderBook.cpp \
  src/MatchingEngine.cpp \
  src/OrderProducer.cpp \
  src/EngineWorker.cpp \
  src/ConsoleRenderer.cpp \
  main.cpp \
  -o limit_order_book

if [ $? -eq 0 ]; then
    echo "✓ Main executable built successfully: limit_order_book"
else
    echo "✗ Failed to build main executable"
    exit 1
fi

echo ""

# Compile test executable
echo "[2/2] Building test executable..."
clang++ -std=c++17 -Iinclude -pthread \
  tests/simple_tests.cpp \
  src/Order.cpp \
  src/OrderBook.cpp \
  src/MatchingEngine.cpp \
  -o test_order_book

if [ $? -eq 0 ]; then
    echo "✓ Test executable built successfully: test_order_book"
else
    echo "✗ Failed to build test executable"
    exit 1
fi

echo ""
echo "Build complete!"
echo ""
echo "To run the application:"
echo "  ./limit_order_book              (random mode)"
echo "  ./limit_order_book --mode=stdin (manual input mode)"
echo ""
echo "To run tests:"
echo "  ./test_order_book"
