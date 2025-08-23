#include <librapid/librapid.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test global configuration functions", "[globalConfig]") {
    SECTION("Thread management functions") {
        // Test getting current number of threads
        size_t originalThreads = librapid::getNumThreads();
        REQUIRE(originalThreads > 0);
        
        // Test setting number of threads to a specific value
        size_t testThreads = 4;
        librapid::setNumThreads(testThreads);
        
        // Verify the value was set correctly
        REQUIRE(librapid::getNumThreads() == testThreads);
        
        // Test setting to 1 thread (single-threaded)
        librapid::setNumThreads(1);
        REQUIRE(librapid::getNumThreads() == 1);
        
        // Test setting to a reasonable number of threads
        librapid::setNumThreads(8);
        REQUIRE(librapid::getNumThreads() == 8);
        
        // Restore original thread count
        librapid::setNumThreads(originalThreads);
        REQUIRE(librapid::getNumThreads() == originalThreads);
    }
    
    SECTION("Seed management functions") {
        // Test getting current seed
        size_t originalSeed = librapid::getSeed();
        
        // Test setting seed to a specific value
        size_t testSeed = 12345;
        librapid::setSeed(testSeed);
        
        // Verify the seed was set correctly
        REQUIRE(librapid::getSeed() == testSeed);
        
        // Test setting seed to 0 (often a special case)
        librapid::setSeed(0);
        REQUIRE(librapid::getSeed() == 0);
        
        // Test setting to a large seed value
        size_t largeSeed = 9876543210UL;
        librapid::setSeed(largeSeed);
        REQUIRE(librapid::getSeed() == largeSeed);
        
        // Restore original seed
        librapid::setSeed(originalSeed);
        REQUIRE(librapid::getSeed() == originalSeed);
    }
    
    SECTION("Global variables access") {
        // Test that global variables are accessible and have sensible defaults
        REQUIRE(librapid::global::multithreadThreshold > 0);
        REQUIRE(librapid::global::gemmMultithreadThreshold > 0);
        REQUIRE(librapid::global::gemvMultithreadThreshold > 0);
        REQUIRE(librapid::global::numThreads > 0);
        REQUIRE(librapid::global::cacheLineSize > 0);
        
        // Test that we can modify global variables
        size_t originalMultithreadThreshold = librapid::global::multithreadThreshold;
        librapid::global::multithreadThreshold = 1000;
        REQUIRE(librapid::global::multithreadThreshold == 1000);
        
        // Restore original value
        librapid::global::multithreadThreshold = originalMultithreadThreshold;
    }
    
    SECTION("Thread count boundary conditions") {
        // Test setting very low thread count
        librapid::setNumThreads(1);
        REQUIRE(librapid::getNumThreads() == 1);
        
        // Test setting reasonable thread count
        librapid::setNumThreads(16);
        REQUIRE(librapid::getNumThreads() == 16);
    }
}