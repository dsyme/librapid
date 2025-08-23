#include <librapid/librapid.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Cache Line Size", "[utils][cacheLineSize]") {
    SECTION("Basic functionality") {
        // Test that cacheLineSize returns a reasonable value
        size_t cacheSize = librapid::cacheLineSize();
        
        // Cache line sizes should be power of 2 and within reasonable bounds
        REQUIRE(cacheSize >= 32);
        REQUIRE(cacheSize <= 256);
        
        // Check that it's a power of 2
        REQUIRE((cacheSize & (cacheSize - 1)) == 0);
    }
    
    SECTION("Consistency check") {
        // Multiple calls should return the same value (cached result)
        size_t first = librapid::cacheLineSize();
        size_t second = librapid::cacheLineSize();
        size_t third = librapid::cacheLineSize();
        
        REQUIRE(first == second);
        REQUIRE(second == third);
    }
    
    SECTION("Common cache line sizes") {
        size_t cacheSize = librapid::cacheLineSize();
        
        // Should be one of the common cache line sizes
        bool isCommonSize = (cacheSize == 32) || (cacheSize == 64) || 
                           (cacheSize == 128) || (cacheSize == 256);
        REQUIRE(isCommonSize);
    }
    
    SECTION("Performance test") {
        // Cache should be fast on subsequent calls
        auto start = std::chrono::high_resolution_clock::now();
        
        // First call might be slower due to system call
        librapid::cacheLineSize();
        
        auto middle = std::chrono::high_resolution_clock::now();
        
        // Subsequent calls should be faster (cached)
        for (int i = 0; i < 1000; ++i) {
            librapid::cacheLineSize();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        
        auto firstCall = std::chrono::duration_cast<std::chrono::nanoseconds>(middle - start).count();
        auto cachedCalls = std::chrono::duration_cast<std::chrono::nanoseconds>(end - middle).count();
        
        // The cached calls should be significantly faster per call
        REQUIRE(cachedCalls < firstCall * 100); // Allow for some variance
    }
}