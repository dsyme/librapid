#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <limits>
#include <cmath>

namespace lrc = librapid;

TEST_CASE("Test bitCast function", "[memory]") {
    SECTION("Basic float to int conversion") {
        float f = 3.14159f;
        auto i = lrc::bitCast<uint32_t>(f);
        auto f2 = lrc::bitCast<float>(i);
        
        REQUIRE(f == f2); // Should be identical
    }
    
    SECTION("Double to uint64_t conversion") {
        double d = 2.71828;
        auto i = lrc::bitCast<uint64_t>(d);
        auto d2 = lrc::bitCast<double>(i);
        
        REQUIRE(d == d2); // Should be identical
    }
    
    SECTION("Same size integer types") {
        int32_t original = -12345;
        auto converted = lrc::bitCast<uint32_t>(original);
        auto back = lrc::bitCast<int32_t>(converted);
        
        REQUIRE(back == original);
    }
    
    SECTION("Special float values") {
        // Test NaN
        float nan = std::numeric_limits<float>::quiet_NaN();
        auto nan_bits = lrc::bitCast<uint32_t>(nan);
        auto nan_back = lrc::bitCast<float>(nan_bits);
        REQUIRE(std::isnan(nan_back));
        
        // Test infinity
        float inf = std::numeric_limits<float>::infinity();
        auto inf_bits = lrc::bitCast<uint32_t>(inf);
        auto inf_back = lrc::bitCast<float>(inf_bits);
        REQUIRE(std::isinf(inf_back));
        REQUIRE(inf_back > 0);
        
        // Test negative infinity
        float neg_inf = -std::numeric_limits<float>::infinity();
        auto neg_inf_bits = lrc::bitCast<uint32_t>(neg_inf);
        auto neg_inf_back = lrc::bitCast<float>(neg_inf_bits);
        REQUIRE(std::isinf(neg_inf_back));
        REQUIRE(neg_inf_back < 0);
    }
}

TEST_CASE("Test isNaN function", "[memory]") {
    SECTION("Float values") {
        REQUIRE_FALSE(lrc::isNaN(1.0f));
        REQUIRE_FALSE(lrc::isNaN(0.0f));
        REQUIRE_FALSE(lrc::isNaN(-1.0f));
        REQUIRE_FALSE(lrc::isNaN(std::numeric_limits<float>::infinity()));
        REQUIRE_FALSE(lrc::isNaN(-std::numeric_limits<float>::infinity()));
        
        REQUIRE(lrc::isNaN(std::numeric_limits<float>::quiet_NaN()));
        REQUIRE(lrc::isNaN(std::numeric_limits<float>::signaling_NaN()));
    }
    
    SECTION("Double values") {
        REQUIRE_FALSE(lrc::isNaN(1.0));
        REQUIRE_FALSE(lrc::isNaN(0.0));
        REQUIRE_FALSE(lrc::isNaN(-1.0));
        REQUIRE_FALSE(lrc::isNaN(std::numeric_limits<double>::infinity()));
        REQUIRE_FALSE(lrc::isNaN(-std::numeric_limits<double>::infinity()));
        
        REQUIRE(lrc::isNaN(std::numeric_limits<double>::quiet_NaN()));
        REQUIRE(lrc::isNaN(std::numeric_limits<double>::signaling_NaN()));
    }
    
    SECTION("Long double values") {
        REQUIRE_FALSE(lrc::isNaN(1.0L));
        REQUIRE_FALSE(lrc::isNaN(0.0L));
        REQUIRE_FALSE(lrc::isNaN(-1.0L));
        
        REQUIRE(lrc::isNaN(std::numeric_limits<long double>::quiet_NaN()));
    }
}

TEST_CASE("Test isFinite function", "[memory]") {
    SECTION("Float values") {
        REQUIRE(lrc::isFinite(1.0f));
        REQUIRE(lrc::isFinite(0.0f));
        REQUIRE(lrc::isFinite(-1.0f));
        REQUIRE(lrc::isFinite(std::numeric_limits<float>::max()));
        REQUIRE(lrc::isFinite(std::numeric_limits<float>::lowest()));
        
        REQUIRE_FALSE(lrc::isFinite(std::numeric_limits<float>::infinity()));
        REQUIRE_FALSE(lrc::isFinite(-std::numeric_limits<float>::infinity()));
        REQUIRE_FALSE(lrc::isFinite(std::numeric_limits<float>::quiet_NaN()));
    }
    
    SECTION("Double values") {
        REQUIRE(lrc::isFinite(1.0));
        REQUIRE(lrc::isFinite(0.0));
        REQUIRE(lrc::isFinite(-1.0));
        REQUIRE(lrc::isFinite(std::numeric_limits<double>::max()));
        REQUIRE(lrc::isFinite(std::numeric_limits<double>::lowest()));
        
        REQUIRE_FALSE(lrc::isFinite(std::numeric_limits<double>::infinity()));
        REQUIRE_FALSE(lrc::isFinite(-std::numeric_limits<double>::infinity()));
        REQUIRE_FALSE(lrc::isFinite(std::numeric_limits<double>::quiet_NaN()));
    }
}

TEST_CASE("Test isInf function", "[memory]") {
    SECTION("Float values") {
        REQUIRE_FALSE(lrc::isInf(1.0f));
        REQUIRE_FALSE(lrc::isInf(0.0f));
        REQUIRE_FALSE(lrc::isInf(-1.0f));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<float>::max()));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<float>::lowest()));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<float>::quiet_NaN()));
        
        REQUIRE(lrc::isInf(std::numeric_limits<float>::infinity()));
        REQUIRE(lrc::isInf(-std::numeric_limits<float>::infinity()));
    }
    
    SECTION("Double values") {
        REQUIRE_FALSE(lrc::isInf(1.0));
        REQUIRE_FALSE(lrc::isInf(0.0));
        REQUIRE_FALSE(lrc::isInf(-1.0));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<double>::max()));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<double>::lowest()));
        REQUIRE_FALSE(lrc::isInf(std::numeric_limits<double>::quiet_NaN()));
        
        REQUIRE(lrc::isInf(std::numeric_limits<double>::infinity()));
        REQUIRE(lrc::isInf(-std::numeric_limits<double>::infinity()));
    }
}

TEST_CASE("Test copySign function", "[memory]") {
    SECTION("Float values") {
        REQUIRE(lrc::copySign(5.0f, 1.0f) == 5.0f);
        REQUIRE(lrc::copySign(5.0f, -1.0f) == -5.0f);
        REQUIRE(lrc::copySign(-5.0f, 1.0f) == 5.0f);
        REQUIRE(lrc::copySign(-5.0f, -1.0f) == -5.0f);
        
        // Test with zero
        REQUIRE(lrc::copySign(0.0f, 1.0f) == 0.0f);
        REQUIRE(lrc::copySign(0.0f, -1.0f) == -0.0f);
    }
    
    SECTION("Double values") {
        REQUIRE(lrc::copySign(5.0, 1.0) == 5.0);
        REQUIRE(lrc::copySign(5.0, -1.0) == -5.0);
        REQUIRE(lrc::copySign(-5.0, 1.0) == 5.0);
        REQUIRE(lrc::copySign(-5.0, -1.0) == -5.0);
    }
    
    SECTION("Mixed types") {
        REQUIRE(lrc::copySign(5.0f, 1) == 5.0f);
        REQUIRE(lrc::copySign(5.0f, -1) == -5.0f);
        REQUIRE(lrc::copySign(5.0, 1.0f) == 5.0);
        REQUIRE(lrc::copySign(5.0, -1.0f) == -5.0);
    }
    
    SECTION("Special values") {
        // Test with infinity
        float inf = std::numeric_limits<float>::infinity();
        REQUIRE(lrc::copySign(inf, -1.0f) == -inf);
        REQUIRE(lrc::copySign(-inf, 1.0f) == inf);
        
        // Test with NaN (magnitude should be preserved)
        float nan = std::numeric_limits<float>::quiet_NaN();
        auto result_pos = lrc::copySign(nan, 1.0f);
        auto result_neg = lrc::copySign(nan, -1.0f);
        REQUIRE(std::isnan(result_pos));
        REQUIRE(std::isnan(result_neg));
    }
}

TEST_CASE("Test signBit function", "[memory]") {
    SECTION("Float specializations") {
        REQUIRE_FALSE(lrc::signBit(1.0f));
        REQUIRE_FALSE(lrc::signBit(0.0f));
        REQUIRE(lrc::signBit(-1.0f));
        REQUIRE(lrc::signBit(-0.0f));
        
        // Special values
        REQUIRE_FALSE(lrc::signBit(std::numeric_limits<float>::infinity()));
        REQUIRE(lrc::signBit(-std::numeric_limits<float>::infinity()));
    }
    
    SECTION("Double specializations") {
        REQUIRE_FALSE(lrc::signBit(1.0));
        REQUIRE_FALSE(lrc::signBit(0.0));
        REQUIRE(lrc::signBit(-1.0));
        REQUIRE(lrc::signBit(-0.0));
        
        // Special values
        REQUIRE_FALSE(lrc::signBit(std::numeric_limits<double>::infinity()));
        REQUIRE(lrc::signBit(-std::numeric_limits<double>::infinity()));
    }
    
    SECTION("Long double specializations") {
        REQUIRE_FALSE(lrc::signBit(1.0L));
        REQUIRE_FALSE(lrc::signBit(0.0L));
        REQUIRE(lrc::signBit(-1.0L));
        REQUIRE(lrc::signBit(-0.0L));
        
        // Special values
        REQUIRE_FALSE(lrc::signBit(std::numeric_limits<long double>::infinity()));
        REQUIRE(lrc::signBit(-std::numeric_limits<long double>::infinity()));
    }
    
    SECTION("Generic template (should convert to double)") {
        REQUIRE_FALSE(lrc::signBit(1));
        REQUIRE(lrc::signBit(-1));
    }
}

TEST_CASE("Test ldexp function", "[memory]") {
    SECTION("Float values") {
        REQUIRE(lrc::ldexp(1.0f, 0) == 1.0f);
        REQUIRE(lrc::ldexp(1.0f, 1) == 2.0f);
        REQUIRE(lrc::ldexp(1.0f, 2) == 4.0f);
        REQUIRE(lrc::ldexp(1.0f, -1) == 0.5f);
        REQUIRE(lrc::ldexp(1.0f, -2) == 0.25f);
        
        REQUIRE(lrc::ldexp(3.0f, 2) == 12.0f); // 3 * 2^2 = 3 * 4 = 12
        REQUIRE(lrc::ldexp(5.0f, -3) == 0.625f); // 5 * 2^-3 = 5 / 8 = 0.625
    }
    
    SECTION("Double values") {
        REQUIRE(lrc::ldexp(1.0, 0) == 1.0);
        REQUIRE(lrc::ldexp(1.0, 1) == 2.0);
        REQUIRE(lrc::ldexp(1.0, 2) == 4.0);
        REQUIRE(lrc::ldexp(1.0, -1) == 0.5);
        REQUIRE(lrc::ldexp(1.0, -2) == 0.25);
        
        REQUIRE(lrc::ldexp(3.0, 2) == 12.0); // 3 * 2^2 = 3 * 4 = 12
        REQUIRE(lrc::ldexp(5.0, -3) == 0.625); // 5 * 2^-3 = 5 / 8 = 0.625
    }
    
    SECTION("Large exponents") {
        // Test with 64-bit exponent values
        int64_t large_exp = 10;
        REQUIRE(lrc::ldexp(1.0f, large_exp) == 1024.0f); // 2^10 = 1024
        
        int64_t neg_exp = -10;
        REQUIRE(lrc::ldexp(1.0f, neg_exp) == Approx(0.0009765625f)); // 2^-10 ≈ 0.0009765625
    }
    
    SECTION("Edge cases") {
        REQUIRE(lrc::ldexp(0.0f, 100) == 0.0f); // 0 * anything = 0
        REQUIRE(lrc::ldexp(-0.0f, 100) == -0.0f); // Preserve sign of zero
    }
}

TEST_CASE("Test memcpy function", "[memory]") {
    SECTION("Basic integer copying") {
        int src[5] = {1, 2, 3, 4, 5};
        int dest[5] = {0};
        
        lrc::memcpy(dest, src, sizeof(src));
        
        for (int i = 0; i < 5; ++i) {
            REQUIRE(dest[i] == src[i]);
        }
    }
    
    SECTION("Float copying") {
        float src[3] = {1.1f, 2.2f, 3.3f};
        float dest[3] = {0.0f};
        
        lrc::memcpy(dest, src, sizeof(src));
        
        for (int i = 0; i < 3; ++i) {
            REQUIRE(dest[i] == src[i]);
        }
    }
    
    SECTION("Partial copying") {
        int src[5] = {1, 2, 3, 4, 5};
        int dest[5] = {0};
        
        // Copy only first 3 elements
        lrc::memcpy(dest, src, sizeof(int) * 3);
        
        REQUIRE(dest[0] == 1);
        REQUIRE(dest[1] == 2);
        REQUIRE(dest[2] == 3);
        REQUIRE(dest[3] == 0); // Should remain unchanged
        REQUIRE(dest[4] == 0); // Should remain unchanged
    }
    
    SECTION("Different type copying") {
        // Copy bytes from one type to another (raw memory)
        uint32_t src = 0x12345678;
        uint8_t dest[4];
        
        lrc::memcpy(dest, &src, sizeof(src));
        
        // Result depends on endianness, but total should be preserved
        uint32_t reconstructed;
        lrc::memcpy(&reconstructed, dest, sizeof(reconstructed));
        REQUIRE(reconstructed == src);
    }
    
    SECTION("Struct copying") {
        struct TestStruct {
            int a;
            float b;
            double c;
        };
        
        TestStruct src{42, 3.14f, 2.71828};
        TestStruct dest{0, 0.0f, 0.0};
        
        lrc::memcpy(&dest, &src, sizeof(TestStruct));
        
        REQUIRE(dest.a == src.a);
        REQUIRE(dest.b == src.b);
        REQUIRE(dest.c == src.c);
    }
}