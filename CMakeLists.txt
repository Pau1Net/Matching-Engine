cmake_minimum_required(VERSION 3.16)

project(limit_order_book LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/Order.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/OrderProducer.cpp
    src/EngineWorker.cpp
    src/ConsoleRenderer.cpp
    main.cpp
)

# Main executable
add_executable(limit_order_book ${SOURCES})

# Link threading library
find_package(Threads REQUIRED)
target_link_libraries(limit_order_book PRIVATE Threads::Threads)

# Test executable
add_executable(test_order_book
    tests/simple_tests.cpp
    src/Order.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
)
target_link_libraries(test_order_book PRIVATE Threads::Threads)

# Installation
include(GNUInstallDirs)
install(TARGETS limit_order_book
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
