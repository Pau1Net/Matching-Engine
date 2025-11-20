( RU версия ниже )
# Limit Order Book Matching Engine

![C++](https://img.shields.io/badge/std-C%2B%2B17%20%7C%20C%2B%2B20-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-orange.svg)



A high-performance simulation of a stock exchange trading core, implementing a limit order book with price-time priority matching algorithm.

## Features

- **Multi-threaded Architecture**: Three concurrent threads handling different responsibilities
  - Thread 1 (Producer): Generates or reads incoming orders
  - Thread 2 (Engine): Matches orders based on price-time priority
  - Thread 3 (Renderer): Real-time console visualization of the order book

- **Price-Time Priority Matching**: Orders are matched by best price first, then by arrival time (FIFO)

- **Partial Fill Support**: Orders can be partially filled if insufficient liquidity exists at a price level

- **Thread-Safe Operations**: All shared data structures use proper synchronization

- **Real-Time Visualization**: ASCII-based order book display updating every 500ms

## Architecture

### Core Components

- **Order**: Represents a buy/sell order with ID, side, price, quantity, and timestamp
- **OrderBook**: Maintains bid and ask levels with price-time priority
- **MatchingEngine**: Executes trades according to matching logic
- **ThreadSafeQueue**: Lock-based thread-safe queue for order flow
- **OrderProducer**: Generates random orders or reads from stdin
- **EngineWorker**: Consumes orders and executes matching
- **ConsoleRenderer**: Displays market depth in real-time

### Data Structures

- Bids: `std::map<double, PriceLevel, std::greater<double>>` (descending price)
- Asks: `std::map<double, PriceLevel, std::less<double>>` (ascending price)
- Each price level maintains a FIFO queue of orders

## Building

### Using the build script (recommended):
```bash
./build.sh
```

### Manual compilation:
```bash
# Main executable
clang++ -std=c++17 -Iinclude -pthread \
  src/Order.cpp \
  src/OrderBook.cpp \
  src/MatchingEngine.cpp \
  src/OrderProducer.cpp \
  src/EngineWorker.cpp \
  src/ConsoleRenderer.cpp \
  main.cpp \
  -o limit_order_book

# Test executable
clang++ -std=c++17 -Iinclude -pthread \
  tests/simple_tests.cpp \
  src/Order.cpp \
  src/OrderBook.cpp \
  src/MatchingEngine.cpp \
  -o test_order_book
```

## Usage

### Random Mode (Default)
Automatically generates random buy/sell orders:
```bash
./limit_order_book
```
or
```bash
./limit_order_book --mode=random
```

### Stdin Mode
Manually enter orders via standard input:
```bash
./limit_order_book --mode=stdin
```

Order format: `<BUY|SELL> <price> <quantity>`

Example:
```
BUY 100.50 1000
SELL 101.00 500
BUY 99.75 2000
```

### Running Tests
```bash
./test_order_book
```

## Console Output

The application displays:
- **Market Depth**: Top 10 bid and ask levels
- **Spread**: Difference between best bid and best ask
- **Last Trade**: Price and quantity of most recent execution
- **Queue Size**: Number of pending orders

Example:
```
╔════════════════════════════════════════════════════╗
║        LIMIT ORDER BOOK - MARKET DEPTH            ║
╚════════════════════════════════════════════════════╝

          PRICE │        QUANTITY │       SIDE
────────────────┼─────────────────┼───────────
         102.50 │            1500 │        ASK
         102.00 │            2000 │        ASK
         101.50 │             800 │        ASK
────────────────┴─────────────────┴───────────
           SPREAD: 1.00
────────────────┬─────────────────┬───────────
         100.50 │            1200 │        BID
         100.00 │            3000 │        BID
          99.50 │             900 │        BID
────────────────┴─────────────────┴───────────

┌────────────────────────────────────────────┐
│ LAST TRADE                                 │
├────────────────────────────────────────────┤
│ Price:        101.00                       │
│ Quantity:      500                         │
└────────────────────────────────────────────┘

┌────────────────────────────────────────────┐
│ SYSTEM STATUS                              │
├────────────────────────────────────────────┤
│ Pending Orders in Queue:     3             │
└────────────────────────────────────────────┘
```

## Technical Specification

- **Language**: C++17
- **Threading**: Standard library (`std::thread`, `std::mutex`, `std::condition_variable`)
- **Compiler**: Clang++ / G++
- **Platform**: macOS, Linux (Windows with minor modifications)

## Project Structure

```
limit_order_book/
├── include/              # Header files
│   ├── Order.h
│   ├── OrderBook.h
│   ├── MatchingEngine.h
│   ├── Trade.h
│   ├── ThreadSafeQueue.h
│   ├── OrderProducer.h
│   ├── EngineWorker.h
│   └── ConsoleRenderer.h
├── src/                  # Implementation files
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── MatchingEngine.cpp
│   ├── OrderProducer.cpp
│   ├── EngineWorker.cpp
│   └── ConsoleRenderer.cpp
├── tests/                # Test suite
│   └── simple_tests.cpp
├── main.cpp              # Application entry point
├── build.sh              # Build script
├── CMakeLists.txt        # CMake configuration
└── README.md             # This file
```

## Testing

The test suite includes:
- Order creation and modification
- Order book operations (add, remove, priority)
- Matching engine logic (full match, partial match, no match)
- Multi-level order matching

All tests use a simple custom test framework with macros:
- `ASSERT_EQUAL(expected, actual)`
- `ASSERT_TRUE(condition)`
- `ASSERT_FALSE(condition)`
- `ASSERT_DOUBLE_EQUAL(expected, actual, epsilon)`

## Graceful Shutdown

Press `Ctrl+C` to gracefully stop all threads and exit the application.

## License

This is an educational project demonstrating limit order book implementation. DONT USE FOR REAL LIFE TRADES

## RU Version

## Многопоточная симуляция биржевого ядра (Matching Engine) с визуализацией стакана в реальном времени.

![C++](https://img.shields.io/badge/std-C%2B%2B17%20%7C%20C%2B%2B20-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-orange.svg)

**Многопоточная симуляция биржевого ядра (Matching Engine) с визуализацией стакана в реальном времени.**

Проект написан на **чистом C++** (STL only) без использования тяжеловесных фреймворков вроде Qt или Boost. Основная цель — демонстрация работы с конкурентностью (`std::thread`, `std::mutex`, `std::condition_variable`), структурами данных и алгоритмами сведения ордеров (Price-Time Priority).

---

## 📊 Демонстрация работы

Приложение генерирует торговую активность, сводит ордера и рендерит "биржевой стакан" (Market Depth) прямо в консоли, используя ASCII-графику.

```text
╔════════════════════════════════════════════════════╗
║        LIMIT ORDER BOOK - MARKET DEPTH            ║
╚════════════════════════════════════════════════════╝

          PRICE │        QUANTITY │       SIDE
────────────────┼─────────────────┼───────────
         102.50 │            6808 │        ASK
         102.44 │            4304 │        ASK
         102.42 │            2743 │        ASK
         100.74 │            2356 │        ASK
────────────────┴─────────────────┴───────────
           SPREAD: 0.36
────────────────┬─────────────────┬───────────
          98.21 │            2045 │        BID
          98.16 │            7961 │        BID
          98.15 │            7202 │        BID
────────────────┴─────────────────┴───────────

┌────────────────────────────────────────────┐
│ LAST TRADE                                 │
├────────────────────────────────────────────┤
│ Price:         98.57                       │
│ Quantity:       3127                       │
└────────────────────────────────────────────┘
```
## Сборка и Запуск

```bash
# Клонируем репозиторий
git clone https://github.com/your-username/limit-order-book.git
cd limit-order-book

# Создаем директорию сборки
mkdir build && cd build

# Конфигурируем и собираем
cmake ..
cmake --build .

# Запускаем
./limit_order_book
```
Для сборки требуется компилятор с поддержкой C++17 и CMake версии 3.10+.

## Данный проект является учебной симуляцией. Не используйте этот код для реальной торговли на финансовых рынках (хотя он и выглядит круто) :)





