#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace lrc = librapid;

TEST_CASE("Test Console Size", "[utils]") {
    SECTION("Console size returns valid dimensions") {
        auto size = lrc::consoleSize();
        
        // Console size should be positive
        REQUIRE(size.rows > 0);
        REQUIRE(size.cols > 0);
        
        // Should return minimum reasonable values
        REQUIRE(size.rows >= 1);    // At least 1 row
        REQUIRE(size.cols >= 1);    // At least 1 column
        
        // Function should complete without crashing
        // In CI environments, console size might be unusual, so we just verify the function works
        INFO("Console size: " << size.rows << "x" << size.cols);
        
        // Basic sanity check - values should be finite
        REQUIRE(std::isfinite(static_cast<double>(size.rows)));
        REQUIRE(std::isfinite(static_cast<double>(size.cols)));
    }
}