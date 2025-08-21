#include <librapid>
#include <catch2/catch_test_macros.hpp>

namespace lrc = librapid;

TEST_CASE("Test Global Functions", "[core]") {
    SECTION("Thread management functions work correctly") {
        // Get initial thread count
        size_t originalThreads = lrc::getNumThreads();
        
        // Set a different thread count
        size_t newThreads = originalThreads == 8 ? 4 : 8;
        lrc::setNumThreads(newThreads);
        
        // Verify the change
        REQUIRE(lrc::getNumThreads() == newThreads);
        
        // Restore original thread count
        lrc::setNumThreads(originalThreads);
        REQUIRE(lrc::getNumThreads() == originalThreads);
        
        // Test edge cases
        lrc::setNumThreads(1);
        REQUIRE(lrc::getNumThreads() == 1);
        
        lrc::setNumThreads(16);
        REQUIRE(lrc::getNumThreads() == 16);
        
        // Restore original
        lrc::setNumThreads(originalThreads);
    }
    
    SECTION("Random seed functions work correctly") {
        // Get initial seed
        size_t originalSeed = lrc::getSeed();
        
        // Set a different seed
        size_t newSeed = originalSeed + 1000;
        lrc::setSeed(newSeed);
        
        // Verify the change
        REQUIRE(lrc::getSeed() == newSeed);
        
        // Test with zero seed
        lrc::setSeed(0);
        REQUIRE(lrc::getSeed() == 0);
        
        // Test with large seed
        lrc::setSeed(999999999);
        REQUIRE(lrc::getSeed() == 999999999);
        
        // Restore original seed
        lrc::setSeed(originalSeed);
        REQUIRE(lrc::getSeed() == originalSeed);
    }
}