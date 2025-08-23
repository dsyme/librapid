#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <set>
#include <cmath>
#include <algorithm>

using namespace librapid;

TEST_CASE("Random Number Generation - Basic Functionality", "[math][random]") {
    SECTION("random() function basic tests") {
        // Test default range [0, 1)
        for (int i = 0; i < 100; i++) {
            double value = random();
            REQUIRE(value >= 0.0);
            REQUIRE(value < 1.0);
        }
        
        // Test custom range
        for (int i = 0; i < 100; i++) {
            double value = random(10.0, 20.0);
            REQUIRE(value >= 10.0);
            REQUIRE(value < 20.0);
        }
        
        // Test negative range
        for (int i = 0; i < 100; i++) {
            double value = random(-5.0, 5.0);
            REQUIRE(value >= -5.0);
            REQUIRE(value < 5.0);
        }
        
        // Test mixed type parameters
        for (int i = 0; i < 50; i++) {
            auto value = random(1.0f, 10);
            REQUIRE(value >= 1.0);
            REQUIRE(value < 10.0);
        }
    }

    SECTION("randint() function basic tests") {
        // Test positive range
        for (int i = 0; i < 100; i++) {
            int64_t value = randint(1, 10);
            REQUIRE(value >= 1);
            REQUIRE(value <= 10);
        }
        
        // Test range including zero
        for (int i = 0; i < 100; i++) {
            int64_t value = randint(-5, 5);
            REQUIRE(value >= -5);
            REQUIRE(value <= 5);
        }
        
        // Test single value range
        for (int i = 0; i < 10; i++) {
            int64_t value = randint(42, 42);
            REQUIRE(value == 42);
        }
    }

    SECTION("trueRandom() function basic tests") {
        // Test default range [0, 1)
        for (int i = 0; i < 50; i++) {
            double value = trueRandom();
            REQUIRE(value >= 0.0);
            REQUIRE(value < 1.0);
        }
        
        // Test custom range
        for (int i = 0; i < 50; i++) {
            double value = trueRandom(100.0, 200.0);
            REQUIRE(value >= 100.0);
            REQUIRE(value < 200.0);
        }
    }

    SECTION("trueRandint() function basic tests") {
        // Test positive range
        for (int i = 0; i < 50; i++) {
            int64_t value = trueRandint(1, 10);
            REQUIRE(value >= 1);
            REQUIRE(value <= 10);
        }
    }

    SECTION("trueRandomEntropy() function") {
        double entropy = trueRandomEntropy();
        REQUIRE(entropy >= 0.0);
        REQUIRE(std::isfinite(entropy));
    }
}

TEST_CASE("Random Number Generation - Statistical Properties", "[math][random][statistical]") {
    SECTION("random() distribution uniformity") {
        const int samples = 10000;
        const int bins = 10;
        std::vector<int> histogram(bins, 0);
        
        // Generate samples and create histogram
        for (int i = 0; i < samples; i++) {
            double value = random(0.0, 10.0);
            int bin = std::min(static_cast<int>(value), bins - 1);
            histogram[bin]++;
        }
        
        // Check that distribution is roughly uniform (within 20% of expected)
        double expected = samples / bins;
        double tolerance = expected * 0.2;
        
        for (int count : histogram) {
            REQUIRE(std::abs(count - expected) < tolerance);
        }
    }

    SECTION("randint() distribution uniformity") {
        const int samples = 10000;
        const int lower = 1;
        const int upper = 10;
        const int range = upper - lower + 1;
        std::vector<int> counts(range, 0);
        
        // Generate samples and count occurrences
        for (int i = 0; i < samples; i++) {
            int64_t value = randint(lower, upper);
            counts[value - lower]++;
        }
        
        // Check uniformity
        double expected = samples / range;
        double tolerance = expected * 0.2;
        
        for (int count : counts) {
            REQUIRE(std::abs(count - expected) < tolerance);
        }
    }

    SECTION("random() mean and variance") {
        const int samples = 10000;
        double sum = 0.0;
        double sumSquares = 0.0;
        
        for (int i = 0; i < samples; i++) {
            double value = random(0.0, 1.0);
            sum += value;
            sumSquares += value * value;
        }
        
        double mean = sum / samples;
        double variance = (sumSquares / samples) - (mean * mean);
        
        // For uniform distribution on [0,1): mean = 0.5, variance = 1/12
        REQUIRE(mean == Catch::Approx(0.5).epsilon(0.02));
        REQUIRE(variance == Catch::Approx(1.0 / 12.0).epsilon(0.02));
    }
}

TEST_CASE("Gaussian Random Generation", "[math][random][gaussian]") {
    SECTION("randomGaussian() basic tests") {
        // Generate samples and check they're finite
        for (int i = 0; i < 100; i++) {
            double value = randomGaussian();
            REQUIRE(std::isfinite(value));
        }
        
        // Test template parameter
        for (int i = 0; i < 50; i++) {
            float value = randomGaussian<float>();
            REQUIRE(std::isfinite(value));
        }
    }

    SECTION("randomGaussian() statistical properties") {
        const int samples = 10000;
        double sum = 0.0;
        double sumSquares = 0.0;
        std::vector<double> values;
        values.reserve(samples);
        
        for (int i = 0; i < samples; i++) {
            double value = randomGaussian();
            values.push_back(value);
            sum += value;
            sumSquares += value * value;
        }
        
        double mean = sum / samples;
        double variance = (sumSquares / samples) - (mean * mean);
        double stddev = std::sqrt(variance);
        
        // Standard normal distribution: mean = 0, stddev = 1
        REQUIRE(mean == Catch::Approx(0.0).epsilon(0.05));
        REQUIRE(stddev == Catch::Approx(1.0).epsilon(0.05));
        
        // Check roughly 68% of values are within 1 standard deviation
        int within1Sigma = 0;
        for (double value : values) {
            if (std::abs(value) <= 1.0) {
                within1Sigma++;
            }
        }
        double ratio1Sigma = static_cast<double>(within1Sigma) / samples;
        REQUIRE(ratio1Sigma == Catch::Approx(0.68).epsilon(0.05));
        
        // Check roughly 95% of values are within 2 standard deviations
        int within2Sigma = 0;
        for (double value : values) {
            if (std::abs(value) <= 2.0) {
                within2Sigma++;
            }
        }
        double ratio2Sigma = static_cast<double>(within2Sigma) / samples;
        REQUIRE(ratio2Sigma == Catch::Approx(0.95).epsilon(0.03));
    }

    SECTION("randomGaussian() Box-Muller implementation validation") {
        // The Box-Muller algorithm should produce pairs of independent normal variables
        // We test that consecutive calls produce reasonably independent results
        const int pairs = 1000;
        std::vector<std::pair<double, double>> gaussianPairs;
        
        for (int i = 0; i < pairs; i++) {
            double first = randomGaussian();
            double second = randomGaussian();
            gaussianPairs.push_back({first, second});
        }
        
        // Calculate correlation coefficient
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
        
        for (const auto& pair : gaussianPairs) {
            sumX += pair.first;
            sumY += pair.second;
            sumXY += pair.first * pair.second;
            sumX2 += pair.first * pair.first;
            sumY2 += pair.second * pair.second;
        }
        
        double n = static_cast<double>(pairs);
        double correlation = (n * sumXY - sumX * sumY) / 
                           std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        // Correlation should be close to zero for independent variables
        REQUIRE(std::abs(correlation) < 0.1);
    }
}

TEST_CASE("Random Number Generation - Seed Control", "[math][random][seeding]") {
    SECTION("Reproducibility with setSeed()") {
        // Set seed and generate sequence
        setSeed(42);
        std::vector<double> sequence1;
        for (int i = 0; i < 100; i++) {
            sequence1.push_back(random());
        }
        
        // Reset seed and generate same sequence
        setSeed(42);
        std::vector<double> sequence2;
        for (int i = 0; i < 100; i++) {
            sequence2.push_back(random());
        }
        
        // Sequences should be identical
        REQUIRE(sequence1.size() == sequence2.size());
        for (size_t i = 0; i < sequence1.size(); i++) {
            REQUIRE(sequence1[i] == sequence2[i]);
        }
    }

    SECTION("Different seeds produce different sequences") {
        // Generate sequence with seed 1
        setSeed(1);
        std::vector<double> sequence1;
        for (int i = 0; i < 100; i++) {
            sequence1.push_back(random());
        }
        
        // Generate sequence with seed 2
        setSeed(2);
        std::vector<double> sequence2;
        for (int i = 0; i < 100; i++) {
            sequence2.push_back(random());
        }
        
        // Count differences
        int differences = 0;
        for (size_t i = 0; i < sequence1.size(); i++) {
            if (sequence1[i] != sequence2[i]) {
                differences++;
            }
        }
        
        // Should have many differences (expect most values to be different)
        REQUIRE(differences > 90);
    }

    SECTION("randint() reproducibility") {
        setSeed(123);
        std::vector<int64_t> intSequence1;
        for (int i = 0; i < 50; i++) {
            intSequence1.push_back(randint(1, 100));
        }
        
        setSeed(123);
        std::vector<int64_t> intSequence2;
        for (int i = 0; i < 50; i++) {
            intSequence2.push_back(randint(1, 100));
        }
        
        REQUIRE(intSequence1 == intSequence2);
    }

    SECTION("getSeed() returns current seed") {
        uint64_t testSeed = 987654321;
        setSeed(testSeed);
        REQUIRE(getSeed() == testSeed);
        
        // Test different seed
        testSeed = 123456789;
        setSeed(testSeed);
        REQUIRE(getSeed() == testSeed);
    }
}

TEST_CASE("Random Number Generation - Edge Cases and Robustness", "[math][random][edge-cases]") {
    SECTION("Zero and negative ranges") {
        // Test zero-width range for floating point
        for (int i = 0; i < 10; i++) {
            double value = random(5.0, 5.0);
            REQUIRE(value == 5.0);
        }
        
        // Test very small ranges
        for (int i = 0; i < 50; i++) {
            double value = random(1.0, 1.0000001);
            REQUIRE(value >= 1.0);
            REQUIRE(value < 1.0000001);
        }
    }

    SECTION("Large number ranges") {
        // Test large floating point ranges
        for (int i = 0; i < 50; i++) {
            double value = random(1e6, 1e7);
            REQUIRE(value >= 1e6);
            REQUIRE(value < 1e7);
        }
        
        // Test large integer ranges
        for (int i = 0; i < 50; i++) {
            int64_t value = randint(-1000000, 1000000);
            REQUIRE(value >= -1000000);
            REQUIRE(value <= 1000000);
        }
    }

    SECTION("Negative integer ranges") {
        // Test entirely negative range
        for (int i = 0; i < 50; i++) {
            int64_t value = randint(-100, -50);
            REQUIRE(value >= -100);
            REQUIRE(value <= -50);
        }
        
        // Test range crossing zero
        std::set<int64_t> observedValues;
        for (int i = 0; i < 200; i++) {
            int64_t value = randint(-2, 2);
            observedValues.insert(value);
            REQUIRE(value >= -2);
            REQUIRE(value <= 2);
        }
        
        // Should observe all possible values in the range
        REQUIRE(observedValues.size() == 5); // -2, -1, 0, 1, 2
    }

    SECTION("trueRandom() vs random() independence") {
        // Generate sequences with both generators
        std::vector<double> pseudoRandom, trueRandomVals;
        
        setSeed(12345); // This should only affect random(), not trueRandom()
        
        for (int i = 0; i < 100; i++) {
            pseudoRandom.push_back(random());
            trueRandomVals.push_back(trueRandom());
        }
        
        // The sequences should be different (true random shouldn't be affected by setSeed)
        int differences = 0;
        for (size_t i = 0; i < pseudoRandom.size(); i++) {
            if (pseudoRandom[i] != trueRandomVals[i]) {
                differences++;
            }
        }
        
        // Expect most values to be different
        REQUIRE(differences > 95);
    }

    SECTION("Thread safety basic check") {
        // Basic check that functions don't crash when called repeatedly
        // Note: This is not a comprehensive thread safety test
        setSeed(999);
        
        for (int i = 0; i < 1000; i++) {
            double r1 = random();
            int64_t r2 = randint(1, 100);
            double r3 = randomGaussian();
            
            REQUIRE(std::isfinite(r1));
            REQUIRE(r2 >= 1 && r2 <= 100);
            REQUIRE(std::isfinite(r3));
        }
    }
}