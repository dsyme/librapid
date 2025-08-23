#include <librapid/librapid.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Compile Time Math - Product", "[math][compileTime]") {
    SECTION("Single argument") {
        // Test single argument
        constexpr size_t result1 = librapid::product<5>();
        STATIC_REQUIRE(result1 == 5);
        
        constexpr size_t result2 = librapid::product<1>();
        STATIC_REQUIRE(result2 == 1);
        
        constexpr size_t result3 = librapid::product<0>();
        STATIC_REQUIRE(result3 == 0);
    }
    
    SECTION("Two arguments") {
        constexpr size_t result1 = librapid::product<3, 4>();
        STATIC_REQUIRE(result1 == 12);
        
        constexpr size_t result2 = librapid::product<7, 8>();
        STATIC_REQUIRE(result2 == 56);
        
        constexpr size_t result3 = librapid::product<1, 1>();
        STATIC_REQUIRE(result3 == 1);
    }
    
    SECTION("Three arguments") {
        constexpr size_t result1 = librapid::product<2, 3, 4>();
        STATIC_REQUIRE(result1 == 24);
        
        constexpr size_t result2 = librapid::product<5, 6, 7>();
        STATIC_REQUIRE(result2 == 210);
        
        constexpr size_t result3 = librapid::product<1, 2, 3>();
        STATIC_REQUIRE(result3 == 6);
    }
    
    SECTION("Four arguments") {
        constexpr size_t result1 = librapid::product<2, 3, 4, 5>();
        STATIC_REQUIRE(result1 == 120);
        
        constexpr size_t result2 = librapid::product<1, 1, 1, 1>();
        STATIC_REQUIRE(result2 == 1);
    }
    
    SECTION("Many arguments") {
        constexpr size_t result1 = librapid::product<1, 2, 3, 4, 5, 6>();
        STATIC_REQUIRE(result1 == 720); // 6!
        
        constexpr size_t result2 = librapid::product<2, 2, 2, 2, 2>();
        STATIC_REQUIRE(result2 == 32); // 2^5
    }
    
    SECTION("Zero handling") {
        constexpr size_t result1 = librapid::product<0, 5, 10>();
        STATIC_REQUIRE(result1 == 0);
        
        constexpr size_t result2 = librapid::product<3, 0, 7>();
        STATIC_REQUIRE(result2 == 0);
        
        constexpr size_t result3 = librapid::product<4, 5, 0>();
        STATIC_REQUIRE(result3 == 0);
    }
    
    SECTION("Large numbers") {
        // Test with larger numbers that still fit in size_t
        constexpr size_t result1 = librapid::product<10, 10>();
        STATIC_REQUIRE(result1 == 100);
        
        constexpr size_t result2 = librapid::product<16, 16>();
        STATIC_REQUIRE(result2 == 256);
        
        constexpr size_t result3 = librapid::product<3, 3, 3, 3>();
        STATIC_REQUIRE(result3 == 81); // 3^4
    }
    
    SECTION("Runtime verification") {
        // Verify that constexpr results match runtime calculations
        REQUIRE(librapid::product<2, 3, 4>() == 2 * 3 * 4);
        REQUIRE(librapid::product<5, 7>() == 5 * 7);
        REQUIRE(librapid::product<1, 2, 3, 4, 5>() == 120);
    }
}