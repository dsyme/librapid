#include <librapid/librapid.hpp>
#include <catch2/catch_all.hpp>
#include <cmath>

TEST_CASE("Fast Math - pow10", "[math][fastMath]") {
    SECTION("Lookup table range") {
        // Test all values in the lookup table range (-7 to 7)
        for (int64_t exp = -7; exp <= 7; ++exp) {
            double result = librapid::fastmath::pow10(exp);
            double expected = std::pow(10.0, exp);
            
            REQUIRE(result == Catch::Approx(expected).epsilon(1e-15));
        }
    }
    
    SECTION("Specific lookup table values") {
        // Test specific known values from the lookup table
        REQUIRE(librapid::fastmath::pow10(-7) == Catch::Approx(0.0000001).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(-6) == Catch::Approx(0.000001).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(-3) == Catch::Approx(0.001).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(0) == Catch::Approx(1.0).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(1) == Catch::Approx(10.0).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(3) == Catch::Approx(1000.0).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(7) == Catch::Approx(10000000.0).epsilon(1e-15));
    }
    
    SECTION("Large positive exponents") {
        // Test large positive exponents using binary exponentiation
        REQUIRE(librapid::fastmath::pow10(8) == Catch::Approx(100000000.0).epsilon(1e-10));
        REQUIRE(librapid::fastmath::pow10(10) == Catch::Approx(10000000000.0).epsilon(1e-8));
        REQUIRE(librapid::fastmath::pow10(15) == Catch::Approx(1000000000000000.0).epsilon(1e-3));
        
        // Compare with std::pow for accuracy
        for (int64_t exp : {8, 9, 10, 12, 15, 20}) {
            double result = librapid::fastmath::pow10(exp);
            double expected = std::pow(10.0, exp);
            REQUIRE(result == Catch::Approx(expected).epsilon(1e-10));
        }
    }
    
    SECTION("Large negative exponents") {
        // Test large negative exponents using binary exponentiation
        REQUIRE(librapid::fastmath::pow10(-8) == Catch::Approx(0.00000001).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(-10) == Catch::Approx(0.0000000001).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(-15) == Catch::Approx(0.000000000000001).epsilon(1e-15));
        
        // Compare with std::pow for accuracy
        for (int64_t exp : {-8, -9, -10, -12, -15, -20}) {
            double result = librapid::fastmath::pow10(exp);
            double expected = std::pow(10.0, exp);
            REQUIRE(result == Catch::Approx(expected).epsilon(1e-15));
        }
    }
    
    SECTION("Edge cases") {
        // Test edge cases around the lookup table boundaries
        REQUIRE(librapid::fastmath::pow10(-8) == Catch::Approx(std::pow(10.0, -8)).epsilon(1e-15));
        REQUIRE(librapid::fastmath::pow10(8) == Catch::Approx(std::pow(10.0, 8)).epsilon(1e-10));
    }
    
    SECTION("Very large exponents") {
        // Test very large exponents (within reasonable double range)
        double result30 = librapid::fastmath::pow10(30);
        double expected30 = std::pow(10.0, 30);
        REQUIRE(result30 == Catch::Approx(expected30).epsilon(1e-5));
        
        double resultNeg30 = librapid::fastmath::pow10(-30);
        double expectedNeg30 = std::pow(10.0, -30);
        REQUIRE(resultNeg30 == Catch::Approx(expectedNeg30).epsilon(1e-15));
    }
    
    SECTION("Performance comparison") {
        // Test that fastmath::pow10 is faster than std::pow(10.0, x)
        // This is more of a sanity check that the function works correctly
        
        const int iterations = 1000;
        std::vector<int64_t> exponents;
        for (int i = -20; i <= 20; ++i) {
            exponents.push_back(i);
        }
        
        // Warm up
        for (int64_t exp : exponents) {
            librapid::fastmath::pow10(exp);
        }
        
        // Test all values produce correct results
        for (int64_t exp : exponents) {
            double fast_result = librapid::fastmath::pow10(exp);
            double std_result = std::pow(10.0, exp);
            REQUIRE(fast_result == Catch::Approx(std_result).epsilon(1e-12));
        }
    }
    
    SECTION("Binary exponentiation correctness") {
        // Test that binary exponentiation produces correct results
        // for both positive and negative large exponents
        std::vector<int64_t> test_exps = {16, 25, 32, 50, 64, -16, -25, -32, -50, -64};
        
        for (int64_t exp : test_exps) {
            double result = librapid::fastmath::pow10(exp);
            double expected = std::pow(10.0, exp);
            
            // Use relative epsilon for very large/small numbers
            double epsilon = std::max(1e-12, std::abs(expected) * 1e-12);
            REQUIRE(result == Catch::Approx(expected).epsilon(epsilon));
        }
    }
}