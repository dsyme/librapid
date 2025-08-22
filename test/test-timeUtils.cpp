#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <chrono>

namespace lrc = librapid;

TEST_CASE("Test Time Constants", "[time]") {
    REQUIRE(lrc::time::nanosecond == 1);
    REQUIRE(lrc::time::microsecond == 1000);
    REQUIRE(lrc::time::millisecond == 1000000);
    REQUIRE(lrc::time::second == 1000000000);
    REQUIRE(lrc::time::minute == 60000000000LL);
    REQUIRE(lrc::time::hour == 3600000000000LL);
    REQUIRE(lrc::time::day == 86400000000000LL);
}

TEST_CASE("Test now() function", "[time]") {
    SECTION("Basic functionality") {
        auto time1 = lrc::now<>();
        auto time2 = lrc::now<>();
        
        // Time should advance between calls
        REQUIRE(time2 >= time1);
    }
    
    SECTION("Different scales") {
        auto timeSeconds = lrc::now<lrc::time::second>();
        auto timeMillis = lrc::now<lrc::time::millisecond>();
        auto timeMicros = lrc::now<lrc::time::microsecond>();
        auto timeNanos = lrc::now<lrc::time::nanosecond>();
        
        // Higher precision should give larger values
        REQUIRE(timeNanos > timeMicros);
        REQUIRE(timeMicros > timeMillis);
        REQUIRE(timeMillis > timeSeconds);
    }
    
    SECTION("Consistency across multiple calls") {
        std::vector<double> times;
        for (int i = 0; i < 10; ++i) {
            times.push_back(lrc::now<>());
        }
        
        // Times should be monotonically increasing
        for (size_t i = 1; i < times.size(); ++i) {
            REQUIRE(times[i] >= times[i-1]);
        }
    }
}

TEST_CASE("Test sleep function", "[time]") {
    SECTION("Short sleep duration") {
        auto start = lrc::now<lrc::time::millisecond>();
        lrc::sleep<lrc::time::millisecond>(10); // Sleep for 10ms
        auto end = lrc::now<lrc::time::millisecond>();
        
        auto elapsed = end - start;
        // Should be at least 10ms, but allow some tolerance
        REQUIRE(elapsed >= 9.0);
        REQUIRE(elapsed < 50.0); // Should not take too long
    }
    
    SECTION("Zero sleep duration") {
        auto start = lrc::now<lrc::time::microsecond>();
        lrc::sleep<lrc::time::microsecond>(0);
        auto end = lrc::now<lrc::time::microsecond>();
        
        auto elapsed = end - start;
        // Zero sleep should return quickly
        REQUIRE(elapsed < 1000.0); // Less than 1ms
    }
}

TEST_CASE("Test time scale relationships", "[time]") {
    SECTION("Scale conversions") {
        // Test that different scales give proportional results
        auto time_sec = lrc::now<lrc::time::second>();
        auto time_ms = lrc::now<lrc::time::millisecond>();
        
        // The ratio should be approximately 1000 (allowing for timing differences)
        double ratio = time_ms / time_sec;
        REQUIRE(ratio > 500); // Should be close to 1000
        REQUIRE(ratio < 2000);
    }
}