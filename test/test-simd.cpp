#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace librapid;

TEST_CASE("SIMD Type Traits", "[simd][type-traits]") {
    SECTION("IsSIMD trait detection") {
        // Test SIMD type trait detection
        STATIC_REQUIRE_FALSE(typetraits::IsSIMD<float>::value);
        STATIC_REQUIRE_FALSE(typetraits::IsSIMD<double>::value);
        STATIC_REQUIRE_FALSE(typetraits::IsSIMD<int>::value);
        
        // Test SIMD batch types
        using FloatBatch = xsimd::batch<float>;
        using DoubleBatch = xsimd::batch<double>;
        
        STATIC_REQUIRE(typetraits::IsSIMD<FloatBatch>::value);
        STATIC_REQUIRE(typetraits::IsSIMD<DoubleBatch>::value);
        
        // Test SIMD concept
        STATIC_REQUIRE(typetraits::SIMD<FloatBatch>);
        STATIC_REQUIRE(typetraits::SIMD<DoubleBatch>);
        STATIC_REQUIRE_FALSE(typetraits::SIMD<float>);
        STATIC_REQUIRE_FALSE(typetraits::SIMD<double>);
    }
}

TEST_CASE("SIMD Trigonometric Functions", "[simd][trigonometric]") {
    using FloatBatch = xsimd::batch<float>;
    using DoubleBatch = xsimd::batch<double>;
    
    constexpr float epsilon_f = 1e-6f;
    constexpr double epsilon_d = 1e-14;
    
    SECTION("sin function") {
        // Test float batch
        std::vector<float> test_values_f = {0.0f, M_PI/6, M_PI/4, M_PI/3, M_PI/2, M_PI};
        std::vector<float> expected_sin_f = {0.0f, 0.5f, M_SQRT1_2, std::sqrt(3.0f)/2.0f, 1.0f, 0.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::sin(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_sin_f[i], epsilon_f));
        }
        
        // Test double batch  
        std::vector<double> test_values_d = {0.0, M_PI/6, M_PI/4, M_PI/3, M_PI/2, M_PI};
        std::vector<double> expected_sin_d = {0.0, 0.5, M_SQRT1_2, std::sqrt(3.0)/2.0, 1.0, 0.0};
        
        DoubleBatch batch_d = DoubleBatch::load_unaligned(test_values_d.data());
        DoubleBatch result_d = librapid::sin(batch_d);
        
        for (size_t i = 0; i < test_values_d.size() && i < DoubleBatch::size; ++i) {
            REQUIRE_THAT(result_d.get(i), Catch::Matchers::WithinAbs(expected_sin_d[i], epsilon_d));
        }
    }
    
    SECTION("cos function") {
        // Test float batch
        std::vector<float> test_values_f = {0.0f, M_PI/6, M_PI/4, M_PI/3, M_PI/2, M_PI};
        std::vector<float> expected_cos_f = {1.0f, std::sqrt(3.0f)/2.0f, M_SQRT1_2, 0.5f, 0.0f, -1.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::cos(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_cos_f[i], epsilon_f));
        }
        
        // Test double batch
        std::vector<double> test_values_d = {0.0, M_PI/6, M_PI/4, M_PI/3, M_PI/2, M_PI};
        std::vector<double> expected_cos_d = {1.0, std::sqrt(3.0)/2.0, M_SQRT1_2, 0.5, 0.0, -1.0};
        
        DoubleBatch batch_d = DoubleBatch::load_unaligned(test_values_d.data());
        DoubleBatch result_d = librapid::cos(batch_d);
        
        for (size_t i = 0; i < test_values_d.size() && i < DoubleBatch::size; ++i) {
            REQUIRE_THAT(result_d.get(i), Catch::Matchers::WithinAbs(expected_cos_d[i], epsilon_d));
        }
    }
    
    SECTION("tan function") {
        std::vector<float> test_values_f = {0.0f, M_PI/6, M_PI/4, M_PI/3};
        std::vector<float> expected_tan_f = {0.0f, 1.0f/std::sqrt(3.0f), 1.0f, std::sqrt(3.0f)};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::tan(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_tan_f[i], epsilon_f));
        }
    }
    
    SECTION("inverse trigonometric functions") {
        // asin
        std::vector<float> test_asin_f = {0.0f, 0.5f, M_SQRT1_2, 1.0f};
        std::vector<float> expected_asin_f = {0.0f, M_PI/6, M_PI/4, M_PI/2};
        
        FloatBatch batch_asin_f = FloatBatch::load_unaligned(test_asin_f.data());
        FloatBatch result_asin_f = librapid::asin(batch_asin_f);
        
        for (size_t i = 0; i < test_asin_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_asin_f.get(i), Catch::Matchers::WithinAbs(expected_asin_f[i], epsilon_f));
        }
        
        // acos  
        std::vector<float> test_acos_f = {1.0f, std::sqrt(3.0f)/2.0f, M_SQRT1_2, 0.5f, 0.0f};
        std::vector<float> expected_acos_f = {0.0f, M_PI/6, M_PI/4, M_PI/3, M_PI/2};
        
        FloatBatch batch_acos_f = FloatBatch::load_unaligned(test_acos_f.data());
        FloatBatch result_acos_f = librapid::acos(batch_acos_f);
        
        for (size_t i = 0; i < test_acos_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_acos_f.get(i), Catch::Matchers::WithinAbs(expected_acos_f[i], epsilon_f));
        }
        
        // atan
        std::vector<float> test_atan_f = {0.0f, 1.0f/std::sqrt(3.0f), 1.0f, std::sqrt(3.0f)};
        std::vector<float> expected_atan_f = {0.0f, M_PI/6, M_PI/4, M_PI/3};
        
        FloatBatch batch_atan_f = FloatBatch::load_unaligned(test_atan_f.data());
        FloatBatch result_atan_f = librapid::atan(batch_atan_f);
        
        for (size_t i = 0; i < test_atan_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_atan_f.get(i), Catch::Matchers::WithinAbs(expected_atan_f[i], epsilon_f));
        }
    }
}

TEST_CASE("SIMD Hyperbolic Functions", "[simd][hyperbolic]") {
    using FloatBatch = xsimd::batch<float>;
    using DoubleBatch = xsimd::batch<double>;
    
    constexpr float epsilon_f = 1e-6f;
    constexpr double epsilon_d = 1e-14;
    
    SECTION("sinh function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, -1.0f, 2.0f};
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::sinh(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            float expected = std::sinh(test_values_f[i]);
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected, epsilon_f));
        }
    }
    
    SECTION("cosh function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, -1.0f, 2.0f};
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::cosh(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            float expected = std::cosh(test_values_f[i]);
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected, epsilon_f));
        }
        
        // Test cosh(0) = 1 specifically
        std::vector<float> zero_f = {0.0f, 0.0f, 0.0f, 0.0f};
        FloatBatch zero_batch_f = FloatBatch::load_unaligned(zero_f.data());
        FloatBatch cosh_zero_f = librapid::cosh(zero_batch_f);
        
        for (size_t i = 0; i < FloatBatch::size; ++i) {
            REQUIRE_THAT(cosh_zero_f.get(i), Catch::Matchers::WithinAbs(1.0f, epsilon_f));
        }
    }
    
    SECTION("tanh function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, -1.0f, 2.0f, 10.0f};
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::tanh(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            float expected = std::tanh(test_values_f[i]);
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected, epsilon_f));
        }
        
        // Test tanh(0) = 0 specifically
        std::vector<float> zero_f = {0.0f, 0.0f, 0.0f, 0.0f};
        FloatBatch zero_batch_f = FloatBatch::load_unaligned(zero_f.data());
        FloatBatch tanh_zero_f = librapid::tanh(zero_batch_f);
        
        for (size_t i = 0; i < FloatBatch::size; ++i) {
            REQUIRE_THAT(tanh_zero_f.get(i), Catch::Matchers::WithinAbs(0.0f, epsilon_f));
        }
    }
}

TEST_CASE("SIMD Exponential and Logarithmic Functions", "[simd][exponential][logarithmic]") {
    using FloatBatch = xsimd::batch<float>;
    using DoubleBatch = xsimd::batch<double>;
    
    constexpr float epsilon_f = 1e-6f;
    constexpr double epsilon_d = 1e-14;
    
    SECTION("exp function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, -1.0f, 2.0f, -2.0f};
        std::vector<float> expected_exp_f = {1.0f, M_E, 1.0f/M_E, M_E*M_E, 1.0f/(M_E*M_E)};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::exp(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_exp_f[i], epsilon_f));
        }
    }
    
    SECTION("log function") {
        std::vector<float> test_values_f = {1.0f, M_E, M_E*M_E, 1.0f/M_E};
        std::vector<float> expected_log_f = {0.0f, 1.0f, 2.0f, -1.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::log(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_log_f[i], epsilon_f));
        }
    }
    
    SECTION("log2 function") {
        std::vector<float> test_values_f = {1.0f, 2.0f, 4.0f, 8.0f, 0.5f};
        std::vector<float> expected_log2_f = {0.0f, 1.0f, 2.0f, 3.0f, -1.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::log2(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_log2_f[i], epsilon_f));
        }
    }
    
    SECTION("log10 function") {
        std::vector<float> test_values_f = {1.0f, 10.0f, 100.0f, 1000.0f, 0.1f};
        std::vector<float> expected_log10_f = {0.0f, 1.0f, 2.0f, 3.0f, -1.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::log10(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_log10_f[i], epsilon_f));
        }
    }
}

TEST_CASE("SIMD Power and Root Functions", "[simd][power][root]") {
    using FloatBatch = xsimd::batch<float>;
    using DoubleBatch = xsimd::batch<double>;
    
    constexpr float epsilon_f = 1e-6f;
    constexpr double epsilon_d = 1e-14;
    
    SECTION("sqrt function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f};
        std::vector<float> expected_sqrt_f = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::sqrt(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_sqrt_f[i], epsilon_f));
        }
        
        // Test double precision
        std::vector<double> test_values_d = {0.0, 1.0, 4.0, 9.0, 16.0, 25.0};
        std::vector<double> expected_sqrt_d = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
        
        DoubleBatch batch_d = DoubleBatch::load_unaligned(test_values_d.data());
        DoubleBatch result_d = librapid::sqrt(batch_d);
        
        for (size_t i = 0; i < test_values_d.size() && i < DoubleBatch::size; ++i) {
            REQUIRE_THAT(result_d.get(i), Catch::Matchers::WithinAbs(expected_sqrt_d[i], epsilon_d));
        }
    }
    
    SECTION("cbrt function") {
        std::vector<float> test_values_f = {0.0f, 1.0f, 8.0f, 27.0f, -8.0f};
        std::vector<float> expected_cbrt_f = {0.0f, 1.0f, 2.0f, 3.0f, -2.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::cbrt(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_cbrt_f[i], epsilon_f));
        }
    }
}

TEST_CASE("SIMD Utility Functions", "[simd][utility]") {
    using FloatBatch = xsimd::batch<float>;
    using DoubleBatch = xsimd::batch<double>;
    
    constexpr float epsilon_f = 1e-6f;
    constexpr double epsilon_d = 1e-14;
    
    SECTION("abs function") {
        std::vector<float> test_values_f = {-5.0f, -2.5f, 0.0f, 2.5f, 5.0f};
        std::vector<float> expected_abs_f = {5.0f, 2.5f, 0.0f, 2.5f, 5.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::abs(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_abs_f[i], epsilon_f));
        }
        
        // Test double precision
        std::vector<double> test_values_d = {-5.0, -2.5, 0.0, 2.5, 5.0};
        std::vector<double> expected_abs_d = {5.0, 2.5, 0.0, 2.5, 5.0};
        
        DoubleBatch batch_d = DoubleBatch::load_unaligned(test_values_d.data());
        DoubleBatch result_d = librapid::abs(batch_d);
        
        for (size_t i = 0; i < test_values_d.size() && i < DoubleBatch::size; ++i) {
            REQUIRE_THAT(result_d.get(i), Catch::Matchers::WithinAbs(expected_abs_d[i], epsilon_d));
        }
    }
    
    SECTION("floor function") {
        std::vector<float> test_values_f = {-2.7f, -1.1f, 0.0f, 1.1f, 2.7f};
        std::vector<float> expected_floor_f = {-3.0f, -2.0f, 0.0f, 1.0f, 2.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::floor(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_floor_f[i], epsilon_f));
        }
    }
    
    SECTION("ceil function") {
        std::vector<float> test_values_f = {-2.7f, -1.1f, 0.0f, 1.1f, 2.7f};
        std::vector<float> expected_ceil_f = {-2.0f, -1.0f, 0.0f, 2.0f, 3.0f};
        
        FloatBatch batch_f = FloatBatch::load_unaligned(test_values_f.data());
        FloatBatch result_f = librapid::ceil(batch_f);
        
        for (size_t i = 0; i < test_values_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(result_f.get(i), Catch::Matchers::WithinAbs(expected_ceil_f[i], epsilon_f));
        }
    }
}

TEST_CASE("SIMD Edge Cases and Special Values", "[simd][edge-cases]") {
    using FloatBatch = xsimd::batch<float>;
    
    SECTION("Zero and negative values") {
        // Test sqrt of zero
        std::vector<float> zero_f = {0.0f, 0.0f, 0.0f, 0.0f};
        FloatBatch zero_batch_f = FloatBatch::load_unaligned(zero_f.data());
        FloatBatch sqrt_zero_f = librapid::sqrt(zero_batch_f);
        
        for (size_t i = 0; i < FloatBatch::size; ++i) {
            REQUIRE(sqrt_zero_f.get(i) == 0.0f);
        }
        
        // Test exp of zero
        FloatBatch exp_zero_f = librapid::exp(zero_batch_f);
        for (size_t i = 0; i < FloatBatch::size; ++i) {
            REQUIRE_THAT(exp_zero_f.get(i), Catch::Matchers::WithinAbs(1.0f, 1e-6f));
        }
    }
    
    SECTION("Large and small values") {
        // Test with larger values to ensure no overflow issues in reasonable range
        std::vector<float> large_vals_f = {10.0f, 20.0f, 50.0f, 100.0f};
        FloatBatch large_batch_f = FloatBatch::load_unaligned(large_vals_f.data());
        
        // sqrt should work fine with large values
        FloatBatch sqrt_large_f = librapid::sqrt(large_batch_f);
        for (size_t i = 0; i < large_vals_f.size() && i < FloatBatch::size; ++i) {
            float expected = std::sqrt(large_vals_f[i]);
            REQUIRE_THAT(sqrt_large_f.get(i), Catch::Matchers::WithinAbs(expected, 1e-5f));
        }
        
        // abs should preserve large values
        FloatBatch abs_large_f = librapid::abs(large_batch_f);
        for (size_t i = 0; i < large_vals_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(abs_large_f.get(i), Catch::Matchers::WithinAbs(large_vals_f[i], 1e-6f));
        }
    }
    
    SECTION("Mixed positive and negative values") {
        std::vector<float> mixed_f = {-3.14f, -1.0f, 0.0f, 1.0f, 3.14f};
        FloatBatch mixed_batch_f = FloatBatch::load_unaligned(mixed_f.data());
        
        // Test abs with mixed values
        FloatBatch abs_mixed_f = librapid::abs(mixed_batch_f);
        std::vector<float> expected_abs_mixed_f = {3.14f, 1.0f, 0.0f, 1.0f, 3.14f};
        
        for (size_t i = 0; i < mixed_f.size() && i < FloatBatch::size; ++i) {
            REQUIRE_THAT(abs_mixed_f.get(i), Catch::Matchers::WithinAbs(expected_abs_mixed_f[i], 1e-6f));
        }
    }
}

TEST_CASE("SIMD Performance Consistency", "[simd][performance]") {
    using FloatBatch = xsimd::batch<float>;
    
    SECTION("Batch operations vs scalar operations") {
        // This test ensures SIMD operations produce the same results as scalar operations
        std::vector<float> test_values = {1.0f, 2.5f, -1.5f, 0.0f, 3.14159f, -2.71828f};
        
        if (test_values.size() >= FloatBatch::size) {
            FloatBatch batch = FloatBatch::load_unaligned(test_values.data());
            
            // Test sin consistency
            FloatBatch sin_batch = librapid::sin(batch);
            for (size_t i = 0; i < FloatBatch::size; ++i) {
                float scalar_result = std::sin(test_values[i]);
                REQUIRE_THAT(sin_batch.get(i), Catch::Matchers::WithinAbs(scalar_result, 1e-6f));
            }
            
            // Test exp consistency
            FloatBatch exp_batch = librapid::exp(batch);
            for (size_t i = 0; i < FloatBatch::size; ++i) {
                float scalar_result = std::exp(test_values[i]);
                REQUIRE_THAT(exp_batch.get(i), Catch::Matchers::WithinAbs(scalar_result, 1e-6f));
            }
            
            // Test sqrt consistency (only for non-negative values)
            std::vector<float> positive_values = {0.0f, 1.0f, 2.5f, 3.14159f, 9.0f, 16.0f};
            if (positive_values.size() >= FloatBatch::size) {
                FloatBatch pos_batch = FloatBatch::load_unaligned(positive_values.data());
                FloatBatch sqrt_batch = librapid::sqrt(pos_batch);
                
                for (size_t i = 0; i < FloatBatch::size; ++i) {
                    float scalar_result = std::sqrt(positive_values[i]);
                    REQUIRE_THAT(sqrt_batch.get(i), Catch::Matchers::WithinAbs(scalar_result, 1e-6f));
                }
            }
        }
    }
}