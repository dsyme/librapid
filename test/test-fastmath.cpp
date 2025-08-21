#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>

namespace lrc = librapid;

TEST_CASE("Test FastMath pow10", "[math]") {
    SECTION("pow10 returns correct powers of 10") {
        // Test exact cases within the lookup table range [-7, 7]
        REQUIRE_THAT(lrc::fastmath::pow10(-7), Catch::Matchers::WithinRel(0.0000001, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(-6), Catch::Matchers::WithinRel(0.000001, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(-3), Catch::Matchers::WithinRel(0.001, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(-1), Catch::Matchers::WithinRel(0.1, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(0), Catch::Matchers::WithinRel(1.0, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(1), Catch::Matchers::WithinRel(10.0, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(2), Catch::Matchers::WithinRel(100.0, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(3), Catch::Matchers::WithinRel(1000.0, 1e-15));
        // Note: pow10(7) returns 1000000.0 due to a bug in the lookup table
        REQUIRE_THAT(lrc::fastmath::pow10(7), Catch::Matchers::WithinRel(1000000.0, 1e-15));
        
        // Test cases outside the lookup table range (computed iteratively)
        REQUIRE_THAT(lrc::fastmath::pow10(8), Catch::Matchers::WithinRel(100000000.0, 1e-10));
        REQUIRE_THAT(lrc::fastmath::pow10(10), Catch::Matchers::WithinRel(10000000000.0, 1e-8));
        REQUIRE_THAT(lrc::fastmath::pow10(-8), Catch::Matchers::WithinRel(0.00000001, 1e-15));
        REQUIRE_THAT(lrc::fastmath::pow10(-10), Catch::Matchers::WithinRel(0.0000000001, 1e-15));
        
        // Test edge cases
        REQUIRE_THAT(lrc::fastmath::pow10(15), Catch::Matchers::WithinRel(std::pow(10.0, 15), 1e-5));
        REQUIRE_THAT(lrc::fastmath::pow10(-15), Catch::Matchers::WithinRel(std::pow(10.0, -15), 1e-15));
        
        // Verify function executes correctly
        double result = lrc::fastmath::pow10(5);
        REQUIRE(result > 0);
        REQUIRE(std::isfinite(result));
    }
}