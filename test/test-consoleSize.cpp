#include <librapid/librapid.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test consoleSize function", "[consoleSize]") {
    SECTION("Console size returns valid dimensions") {
        librapid::ConsoleSize size = librapid::consoleSize();
        
        // Console dimensions should be positive
        REQUIRE(size.rows > 0);
        REQUIRE(size.cols > 0);
        
        // Reasonable bounds for console size
        // Values should be positive and within reasonable bounds
        REQUIRE(size.rows >= 1);
        REQUIRE(size.cols >= 1);
        REQUIRE(size.rows <= 100000);
        REQUIRE(size.cols <= 100000);
    }
    
    SECTION("Console size struct properties") {
        librapid::ConsoleSize size = librapid::consoleSize();
        
        // Test that the struct members are accessible
        REQUIRE_NOTHROW(size.rows);
        REQUIRE_NOTHROW(size.cols);
        
        // Test struct assignment
        librapid::ConsoleSize size2;
        size2 = size;
        REQUIRE(size2.rows == size.rows);
        REQUIRE(size2.cols == size.cols);
    }
    
    SECTION("Multiple calls return consistent results") {
        librapid::ConsoleSize size1 = librapid::consoleSize();
        librapid::ConsoleSize size2 = librapid::consoleSize();
        
        // Console size should be consistent between calls
        // (assuming no window resizing between calls)
        REQUIRE(size1.rows == size2.rows);
        REQUIRE(size1.cols == size2.cols);
    }
}