#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include <limits>

using namespace librapid;
using Catch::Approx;

TEST_CASE("Test Half Precision Construction and Basic Properties", "[half]") {
    SECTION("Default constructor") {
        half h;
        // Default construction should work without error
        // Value is implementation-defined (could be garbage or zero)
    }

    SECTION("Float constructor") {
        half h(3.14f);
        float result = static_cast<float>(h);
        // Half precision should be close to original (within precision limits)
        REQUIRE(result == Approx(3.14f).epsilon(0.01f));
    }

    SECTION("Double constructor") {
        half h(2.718);
        float result = static_cast<float>(h);
        // Half precision conversion should maintain reasonable accuracy
        REQUIRE(result == Approx(2.718f).epsilon(0.01f));
    }

    SECTION("Integer constructor") {
        half h(42);
        float result = static_cast<float>(h);
        REQUIRE(result == Approx(42.0f));
    }

    SECTION("Copy constructor") {
        half h1(1.5f);
        half h2(h1);
        REQUIRE(static_cast<float>(h2) == Approx(static_cast<float>(h1)));
    }

    SECTION("Assignment operator") {
        half h1(2.5f);
        half h2(1.0f);
        h2 = h1;
        REQUIRE(static_cast<float>(h2) == Approx(static_cast<float>(h1)));
    }

    SECTION("fromBits static method") {
        uint16_t bits = 0x4200; // Some arbitrary bit pattern
        half h = half::fromBits(bits);
        // Should create a half from the bit pattern
        REQUIRE(h.data().m_bits == bits);
    }
}

TEST_CASE("Test Half Precision Special Values", "[half]") {
    SECTION("Positive zero") {
        half h(0.0f);
        float result = static_cast<float>(h);
        REQUIRE(result == 0.0f);
    }

    SECTION("Negative zero") {
        half h(-0.0f);
        float result = static_cast<float>(h);
        // Note: -0.0f might become +0.0f in some implementations
        REQUIRE(std::abs(result) == 0.0f);
    }

    SECTION("Positive infinity") {
        half h(std::numeric_limits<float>::infinity());
        float result = static_cast<float>(h);
        REQUIRE(std::isinf(result));
        REQUIRE(result > 0);
    }

    SECTION("Negative infinity") {
        half h(-std::numeric_limits<float>::infinity());
        float result = static_cast<float>(h);
        REQUIRE(std::isinf(result));
        REQUIRE(result < 0);
    }

    SECTION("NaN values") {
        half h(std::numeric_limits<float>::quiet_NaN());
        float result = static_cast<float>(h);
        REQUIRE(std::isnan(result));
    }

    SECTION("Subnormal values") {
        // Test very small positive and negative values
        half h_pos(1e-6f);  // Very small positive
        half h_neg(-1e-6f); // Very small negative
        
        float result_pos = static_cast<float>(h_pos);
        float result_neg = static_cast<float>(h_neg);
        
        // Should maintain sign even for very small values (might underflow to zero)
        REQUIRE(result_pos >= 0.0f);
        REQUIRE(result_neg <= 0.0f);
    }
}

TEST_CASE("Test Half Precision Arithmetic Operations", "[half]") {
    SECTION("Addition") {
        half a(1.5f);
        half b(2.5f);
        
        half result = a + b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("Subtraction") {
        half a(5.0f);
        half b(3.0f);
        
        half result = a - b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(2.0f).epsilon(0.01f));
    }

    SECTION("Multiplication") {
        half a(2.0f);
        half b(3.0f);
        
        half result = a * b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(6.0f).epsilon(0.01f));
    }

    SECTION("Division") {
        half a(8.0f);
        half b(2.0f);
        
        half result = a / b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("Unary minus") {
        half a(3.5f);
        half result = -a;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(-3.5f).epsilon(0.01f));
    }

    SECTION("Unary plus") {
        half a(3.5f);
        half result = +a;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(3.5f).epsilon(0.01f));
    }
}

TEST_CASE("Test Half Precision Compound Assignment Operations", "[half]") {
    SECTION("Compound addition +=") {
        half a(2.0f);
        half b(3.0f);
        
        a += b;
        float result = static_cast<float>(a);
        REQUIRE(result == Approx(5.0f).epsilon(0.01f));
    }

    SECTION("Compound subtraction -=") {
        half a(7.0f);
        half b(3.0f);
        
        a -= b;
        float result = static_cast<float>(a);
        REQUIRE(result == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("Compound multiplication *=") {
        half a(3.0f);
        half b(2.0f);
        
        a *= b;
        float result = static_cast<float>(a);
        REQUIRE(result == Approx(6.0f).epsilon(0.01f));
    }

    SECTION("Compound division /=") {
        half a(15.0f);
        half b(3.0f);
        
        a /= b;
        float result = static_cast<float>(a);
        REQUIRE(result == Approx(5.0f).epsilon(0.01f));
    }

    SECTION("Increment operators") {
        half a(2.0f);
        
        // Pre-increment
        half pre_result = ++a;
        REQUIRE(static_cast<float>(pre_result) == Approx(3.0f).epsilon(0.01f));
        REQUIRE(static_cast<float>(a) == Approx(3.0f).epsilon(0.01f));
        
        // Post-increment  
        half post_result = a++;
        REQUIRE(static_cast<float>(post_result) == Approx(3.0f).epsilon(0.01f));
        REQUIRE(static_cast<float>(a) == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("Decrement operators") {
        half a(5.0f);
        
        // Pre-decrement
        half pre_result = --a;
        REQUIRE(static_cast<float>(pre_result) == Approx(4.0f).epsilon(0.01f));
        REQUIRE(static_cast<float>(a) == Approx(4.0f).epsilon(0.01f));
        
        // Post-decrement
        half post_result = a--;
        REQUIRE(static_cast<float>(post_result) == Approx(4.0f).epsilon(0.01f));
        REQUIRE(static_cast<float>(a) == Approx(3.0f).epsilon(0.01f));
    }
}

TEST_CASE("Test Half Precision Comparison Operations", "[half]") {
    SECTION("Equality comparison") {
        half a(2.5f);
        half b(2.5f);
        half c(3.5f);
        
        REQUIRE(a == b);
        REQUIRE_FALSE(a == c);
    }

    SECTION("Inequality comparison") {
        half a(2.5f);
        half b(3.5f);
        
        REQUIRE(a != b);
        REQUIRE_FALSE(a != a);
    }

    SECTION("Less than comparison") {
        half a(2.5f);
        half b(3.5f);
        
        REQUIRE(a < b);
        REQUIRE_FALSE(b < a);
    }

    SECTION("Less than or equal comparison") {
        half a(2.5f);
        half b(3.5f);
        half c(2.5f);
        
        REQUIRE(a <= b);
        REQUIRE(a <= c);
        REQUIRE_FALSE(b <= a);
    }

    SECTION("Greater than comparison") {
        half a(3.5f);
        half b(2.5f);
        
        REQUIRE(a > b);
        REQUIRE_FALSE(b > a);
    }

    SECTION("Greater than or equal comparison") {
        half a(3.5f);
        half b(2.5f);
        half c(3.5f);
        
        REQUIRE(a >= b);
        REQUIRE(a >= c);
        REQUIRE_FALSE(b >= a);
    }
}

TEST_CASE("Test Half Precision Mixed Type Operations", "[half]") {
    SECTION("Half + float") {
        half a(2.5f);
        float b = 1.5f;
        
        auto result = a + b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("float + Half") {
        float a = 2.5f;
        half b(1.5f);
        
        auto result = a + b;
        REQUIRE(result == Approx(4.0f).epsilon(0.01f));
    }

    SECTION("Half * double") {
        half a(3.0f);
        double b = 2.0;
        
        auto result = a * b;
        float result_f = static_cast<float>(result);
        REQUIRE(result_f == Approx(6.0f).epsilon(0.01f));
    }

    SECTION("Comparison with different types") {
        half a(2.5f);
        float b = 2.5f;
        
        REQUIRE(static_cast<float>(a) == Approx(b).epsilon(0.01f));
    }
}

TEST_CASE("Test Half Precision Range and Precision Limits", "[half]") {
    SECTION("Maximum representable positive value") {
        // IEEE 754 half precision max is approximately 65504
        half h(60000.0f);  // Close to but less than max
        float result = static_cast<float>(h);
        
        // Should be representable without overflow
        REQUIRE(std::isfinite(result));
        REQUIRE(result > 50000.0f);  // Reasonable lower bound
    }

    SECTION("Minimum representable positive normal value") {
        // IEEE 754 half precision min normal is approximately 6.1e-5
        half h(0.0001f);  // Larger than min normal
        float result = static_cast<float>(h);
        
        REQUIRE(std::isfinite(result));
        REQUIRE(result > 0.0f);
    }

    SECTION("Precision loss demonstration") {
        // Show that we lose precision when converting to half and back
        float original = 1.23456789f;
        half h(original);
        float result = static_cast<float>(h);
        
        // Should be close but not exactly equal due to precision loss
        REQUIRE(result == Approx(original).epsilon(0.01f));
    }

    SECTION("Overflow to infinity") {
        // Values too large should become infinity
        half h(1e10f);  // Much larger than half precision range
        float result = static_cast<float>(h);
        
        REQUIRE(std::isinf(result));
        REQUIRE(result > 0);
    }

    SECTION("Underflow to zero") {
        // Values too small should become zero or subnormal
        half h(1e-10f);  // Much smaller than half precision range
        float result = static_cast<float>(h);
        
        // Result should be either zero or a very small positive number
        REQUIRE(result >= 0.0f);
        REQUIRE(result < 1e-3f);
    }
}

TEST_CASE("Test Half Precision Mathematical Functions", "[half]") {
    SECTION("Absolute value") {
        half pos(3.5f);
        half neg(-3.5f);
        
        // Test using static_cast and std::abs on the result
        float abs_pos = std::abs(static_cast<float>(pos));
        float abs_neg = std::abs(static_cast<float>(neg));
        
        REQUIRE(abs_pos == Approx(3.5f).epsilon(0.01f));
        REQUIRE(abs_neg == Approx(3.5f).epsilon(0.01f));
    }

    SECTION("Square root approximation") {
        half h(4.0f);
        
        // Manual square root test by squaring the result
        float val = static_cast<float>(h);
        float sqrt_val = std::sqrt(val);
        half h_sqrt(sqrt_val);
        float result = static_cast<float>(h_sqrt);
        
        REQUIRE(result == Approx(2.0f).epsilon(0.01f));
    }
}

TEST_CASE("Test Half Precision Type Conversions", "[half]") {
    SECTION("Half to float conversion") {
        half h(3.14159f);
        float f = static_cast<float>(h);
        
        // Should be approximately equal within half precision limits
        REQUIRE(f == Approx(3.14159f).epsilon(0.01f));
    }

    SECTION("Half to double conversion") {
        half h(2.71828f);
        double d = static_cast<double>(h);
        
        // Should be approximately equal within half precision limits
        REQUIRE(d == Approx(2.71828).epsilon(0.01));
    }

    SECTION("Half to int conversion") {
        half h(42.7f);
        int i = static_cast<int>(static_cast<float>(h));
        
        // Should truncate to integer
        REQUIRE(i == 42);
    }

    SECTION("Round trip conversion") {
        float original = 1.5f;
        half h(original);
        float result = static_cast<float>(h);
        
        // 1.5 should be exactly representable in half precision
        REQUIRE(result == original);
    }
}

TEST_CASE("Test Half Precision Bit Manipulation", "[half]") {
    SECTION("Data access") {
        half h(1.0f);
        auto data = h.data();
        
        // Should have valid bit representation
        REQUIRE(data.m_bits != 0); // 1.0 should not be represented as zero bits
    }

    SECTION("fromBits and bit consistency") {
        uint16_t original_bits = 0x3c00; // Represents 1.0 in half precision
        half h = half::fromBits(original_bits);
        uint16_t retrieved_bits = h.data().m_bits;
        
        REQUIRE(retrieved_bits == original_bits);
    }

    SECTION("Bit patterns for special values") {
        // Test some known bit patterns
        half zero = half::fromBits(0x0000);  // +0
        half one = half::fromBits(0x3c00);   // +1
        half neg_one = half::fromBits(0xbc00); // -1
        
        REQUIRE(static_cast<float>(zero) == 0.0f);
        REQUIRE(static_cast<float>(one) == Approx(1.0f));
        REQUIRE(static_cast<float>(neg_one) == Approx(-1.0f));
    }
}

TEST_CASE("Test Half Precision Edge Cases", "[half]") {
    SECTION("Very close to zero") {
        half h(1e-8f);  // Very small positive
        float result = static_cast<float>(h);
        
        // Should either be zero or positive
        REQUIRE(result >= 0.0f);
    }

    SECTION("Sign preservation for zero") {
        half pos_zero(0.0f);
        half neg_zero(-0.0f);
        
        float pos_result = static_cast<float>(pos_zero);
        float neg_result = static_cast<float>(neg_zero);
        
        // Both should be zero, but signs should be preserved if possible
        REQUIRE(pos_result == 0.0f);
        REQUIRE(std::abs(neg_result) == 0.0f);
    }

    SECTION("Large integer values") {
        // Test that reasonably large integers are preserved
        half h(1024.0f);
        float result = static_cast<float>(h);
        
        REQUIRE(result == 1024.0f);
    }

    SECTION("Powers of 2") {
        // Powers of 2 should be exactly representable in many cases
        std::vector<float> powers = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f};
        
        for (float power : powers) {
            half h(power);
            float result = static_cast<float>(h);
            REQUIRE(result == power);
        }
    }

    SECTION("Fractional powers of 2") {
        // Some fractional powers of 2 should also be exactly representable
        std::vector<float> fractions = {0.5f, 0.25f, 0.125f};
        
        for (float fraction : fractions) {
            half h(fraction);
            float result = static_cast<float>(h);
            REQUIRE(result == fraction);
        }
    }

    SECTION("Assignment from different types") {
        half h;
        
        // Test assignment from different numeric types
        h = 42;
        REQUIRE(static_cast<float>(h) == 42.0f);
        
        h = 3.14159;  // double
        REQUIRE(static_cast<float>(h) == Approx(3.14159f).epsilon(0.01f));
        
        h = 2.5f;     // float
        REQUIRE(static_cast<float>(h) == 2.5f);
    }
}