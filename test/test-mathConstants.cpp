#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>

using namespace librapid;

TEST_CASE("Mathematical Constants - Accuracy Verification", "[math][constants]") {
    SECTION("Basic mathematical constants") {
        // Test PI and related constants
        REQUIRE(constants::pi == Catch::Approx(M_PI).epsilon(1e-15));
        REQUIRE(constants::halfPi == Catch::Approx(M_PI / 2.0).epsilon(1e-15));
        REQUIRE(constants::twoPi == Catch::Approx(2.0 * M_PI).epsilon(1e-15));
        REQUIRE(constants::tau == Catch::Approx(2.0 * M_PI).epsilon(1e-15));
        
        // Test E and related constants
        REQUIRE(constants::e == Catch::Approx(M_E).epsilon(1e-15));
        REQUIRE(constants::sqrtE == Catch::Approx(std::sqrt(M_E)).epsilon(1e-15));
        
        // Test square roots
        REQUIRE(constants::sqrt2 == Catch::Approx(M_SQRT2).epsilon(1e-15));
        REQUIRE(constants::sqrt3 == Catch::Approx(std::sqrt(3.0)).epsilon(1e-15));
        REQUIRE(constants::sqrt5 == Catch::Approx(std::sqrt(5.0)).epsilon(1e-15));
        REQUIRE(constants::sqrtPi == Catch::Approx(std::sqrt(M_PI)).epsilon(1e-15));
        
        // Test logarithmic constants
        REQUIRE(constants::ln2 == Catch::Approx(M_LN2).epsilon(1e-15));
        REQUIRE(constants::ln3 == Catch::Approx(std::log(3.0)).epsilon(1e-15));
        REQUIRE(constants::ln5 == Catch::Approx(std::log(5.0)).epsilon(1e-15));
    }

    SECTION("Angle conversion constants") {
        // Test degree/radian conversion factors
        REQUIRE(constants::radToDeg == Catch::Approx(180.0 / M_PI).epsilon(1e-15));
        REQUIRE(constants::degToRad == Catch::Approx(M_PI / 180.0).epsilon(1e-15));
        
        // Test conversion consistency
        double testAngle = 45.0;
        double radians = testAngle * constants::degToRad;
        double backToDegrees = radians * constants::radToDeg;
        REQUIRE(backToDegrees == Catch::Approx(testAngle).epsilon(1e-13));
    }

    SECTION("Mathematical constant relationships") {
        // Test golden ratio properties
        // Golden ratio = (1 + sqrt(5)) / 2
        double expectedGoldenRatio = (1.0 + std::sqrt(5.0)) / 2.0;
        REQUIRE(constants::goldenRatio == Catch::Approx(expectedGoldenRatio).epsilon(1e-15));
        
        // Test golden ratio property: φ² = φ + 1
        double phi = constants::goldenRatio;
        REQUIRE(phi * phi == Catch::Approx(phi + 1.0).epsilon(1e-15));
        
        // Test cube roots
        REQUIRE(constants::cbrt2 == Catch::Approx(std::cbrt(2.0)).epsilon(1e-15));
        REQUIRE(constants::cbrt3 == Catch::Approx(std::cbrt(3.0)).epsilon(1e-15));
        
        // Test 2^sqrt(2)
        REQUIRE(constants::twoToRoot2 == Catch::Approx(std::pow(2.0, std::sqrt(2.0))).epsilon(1e-14));
        
        // Test E^PI (Gelfond's constant)
        REQUIRE(constants::eToPi == Catch::Approx(std::pow(M_E, M_PI)).epsilon(1e-13));
    }

    SECTION("Physical constants") {
        // Test speed of light (exact value)
        REQUIRE(constants::lightSpeed == 299792458.0);
        
        // Test earth gravity (standard acceleration)
        REQUIRE(constants::earthGravity == 9.80665);
    }

    SECTION("Special mathematical constants") {
        // These constants are harder to verify analytically but we can check they're reasonable
        REQUIRE(constants::eulerMascheroni > 0.5);
        REQUIRE(constants::eulerMascheroni < 0.6);
        
        REQUIRE(constants::piSquaredOnSix > 1.6);
        REQUIRE(constants::piSquaredOnSix < 1.7);
        
        // Golden angle should be approximately 137.5 degrees
        double goldenAngleInDegrees = constants::goldenAngle * constants::radToDeg;
        REQUIRE(goldenAngleInDegrees == Catch::Approx(137.5).epsilon(1e-1));
    }

    SECTION("Precision and consistency checks") {
        // Test that constants are consistently defined
        REQUIRE(constants::tau == constants::twoPi);
        REQUIRE(constants::halfPi * 2.0 == Catch::Approx(constants::pi).epsilon(1e-15));
        
        // Test floating point limits
        REQUIRE(constants::epsilon32 == FLT_MIN);
        REQUIRE(constants::epsilon64 == DBL_MIN);
        
        // Test that all constants are finite and not NaN
        std::vector<double> allConstants = {
            constants::pi, constants::e, constants::sqrt2, constants::sqrt3,
            constants::goldenRatio, constants::ln2, constants::ln3, constants::eulerMascheroni,
            constants::lightSpeed, constants::earthGravity, constants::cbrt2, constants::cbrt3
        };
        
        for (double constant : allConstants) {
            REQUIRE(std::isfinite(constant));
            REQUIRE(!std::isnan(constant));
        }
    }
}

TEST_CASE("Mathematical Utility Functions", "[math][utilities]") {
    SECTION("clamp function") {
        // Basic clamping tests
        REQUIRE(clamp(5.0, 0.0, 10.0) == 5.0);
        REQUIRE(clamp(-5.0, 0.0, 10.0) == 0.0);
        REQUIRE(clamp(15.0, 0.0, 10.0) == 10.0);
        
        // Edge cases
        REQUIRE(clamp(0.0, 0.0, 10.0) == 0.0);
        REQUIRE(clamp(10.0, 0.0, 10.0) == 10.0);
        
        // Integer types
        REQUIRE(clamp(5, 0, 10) == 5);
        REQUIRE(clamp(-5, 0, 10) == 0);
        REQUIRE(clamp(15, 0, 10) == 10);
        
        // Mixed types
        REQUIRE(clamp(5.5f, 0, 10.0) == Catch::Approx(5.5).epsilon(1e-6));
    }

    SECTION("lerp function - floating point") {
        // Basic linear interpolation
        REQUIRE(lerp(0.0, 0.0, 10.0) == Catch::Approx(0.0).epsilon(1e-15));
        REQUIRE(lerp(1.0, 0.0, 10.0) == Catch::Approx(10.0).epsilon(1e-15));
        REQUIRE(lerp(0.5, 0.0, 10.0) == Catch::Approx(5.0).epsilon(1e-15));
        
        // Negative values
        REQUIRE(lerp(0.5, -10.0, 10.0) == Catch::Approx(0.0).epsilon(1e-15));
        REQUIRE(lerp(0.25, -10.0, 10.0) == Catch::Approx(-5.0).epsilon(1e-15));
        
        // Out of range interpolation (extrapolation)
        REQUIRE(lerp(1.5, 0.0, 10.0) == Catch::Approx(15.0).epsilon(1e-14));
        REQUIRE(lerp(-0.5, 0.0, 10.0) == Catch::Approx(-5.0).epsilon(1e-14));
    }

    SECTION("lerp function - special values") {
        // NaN handling
        double result = lerp(std::numeric_limits<double>::quiet_NaN(), 0.0, 10.0);
        REQUIRE(std::isnan(result));
        
        result = lerp(0.5, std::numeric_limits<double>::quiet_NaN(), 10.0);
        REQUIRE(std::isnan(result));
        
        result = lerp(0.5, 0.0, std::numeric_limits<double>::quiet_NaN());
        REQUIRE(std::isnan(result));
    }

    SECTION("smoothStep function") {
        // Basic smooth step tests
        REQUIRE(smoothStep(0.0) == Catch::Approx(0.0).epsilon(1e-15));
        REQUIRE(smoothStep(1.0) == Catch::Approx(1.0).epsilon(1e-15));
        REQUIRE(smoothStep(0.5) == Catch::Approx(0.5).epsilon(1e-15));
        
        // Out of range values should be clamped
        REQUIRE(smoothStep(-0.5) == Catch::Approx(0.0).epsilon(1e-15));
        REQUIRE(smoothStep(1.5) == Catch::Approx(1.0).epsilon(1e-15));
        
        // Custom range
        REQUIRE(smoothStep(5.0, 0.0, 10.0) == Catch::Approx(0.5).epsilon(1e-15));
        REQUIRE(smoothStep(0.0, 0.0, 10.0) == Catch::Approx(0.0).epsilon(1e-15));
        REQUIRE(smoothStep(10.0, 0.0, 10.0) == Catch::Approx(1.0).epsilon(1e-15));
        
        // Smooth step should be smoother than linear interpolation at edges
        double smoothAt25 = smoothStep(0.25);
        double lerpAt25 = 0.25;
        REQUIRE(smoothAt25 < lerpAt25); // Should be below linear
        
        double smoothAt75 = smoothStep(0.75);
        double lerpAt75 = 0.75;
        REQUIRE(smoothAt75 > lerpAt75); // Should be above linear
    }

    SECTION("isClose function") {
        // Basic equality tests
        REQUIRE(isClose(1.0, 1.0));
        REQUIRE(isClose(1.0, 1.0000001, 1e-6));
        REQUIRE(!isClose(1.0, 1.001, 1e-6));
        
        // Different magnitude values
        REQUIRE(isClose(1000.0, 1000.1, 1e-3));
        REQUIRE(isClose(0.001, 0.0010001, 1e-6));
        
        // Zero handling
        REQUIRE(isClose(0.0, 0.0));
        REQUIRE(isClose(0.0, 1e-10, 1e-9));
        REQUIRE(!isClose(0.0, 1e-4, 1e-6));
        
        // Negative values
        REQUIRE(isClose(-1.0, -1.0000001, 1e-6));
        REQUIRE(isClose(-1000.0, -1000.1, 1e-3));
        
        // Mixed types
        REQUIRE(isClose(1.0f, 1.0));
        REQUIRE(isClose(1, 1.0000001, 1e-6));
        
        // Custom tolerances
        REQUIRE(isClose(1.0, 1.1, 0.1, 0.0)); // Absolute tolerance only
        REQUIRE(isClose(100.0, 105.0, 0.0, 0.1)); // Relative tolerance only
    }
}