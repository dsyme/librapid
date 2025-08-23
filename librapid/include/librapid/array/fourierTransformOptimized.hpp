#ifndef LIBRAPID_ARRAY_FOURIER_TRANSFORM_OPTIMIZED_HPP
#define LIBRAPID_ARRAY_FOURIER_TRANSFORM_OPTIMIZED_HPP

#if defined(LIBRAPID_HAS_FFTW)
#include <fftw3.h>
#include <unordered_map>
#include <mutex>

namespace librapid {
    namespace array {
        namespace detail {
            
            // Thread-local FFT plan cache for better performance
            struct FFTWPlanKey {
                size_t n;
                bool isDouble;
                
                bool operator==(const FFTWPlanKey &other) const {
                    return n == other.n && isDouble == other.isDouble;
                }
            };
            
            struct FFTWPlanKeyHash {
                size_t operator()(const FFTWPlanKey &key) const {
                    return std::hash<size_t>{}(key.n) ^ (std::hash<bool>{}(key.isDouble) << 1);
                }
            };
            
            using FFTWDoublePlanCache = std::unordered_map<FFTWPlanKey, fftw_plan, FFTWPlanKeyHash>;
            using FFTWFloatPlanCache = std::unordered_map<FFTWPlanKey, fftwf_plan, FFTWPlanKeyHash>;
            
            // Thread-local storage for plan caches
            thread_local FFTWDoublePlanCache fftw_double_plans;
            thread_local FFTWFloatPlanCache fftw_float_plans;
            
            // Cached RFFT implementation for double precision
            LIBRAPID_INLINE void rfft_cached(Complex<double> *output, double *input, size_t n) {
                FFTWPlanKey key{n, true};
                auto it = fftw_double_plans.find(key);
                
                fftw_plan plan;
                if (it == fftw_double_plans.end()) {
                    // Use MEASURE for better performance characteristics
                    unsigned int mode = FFTW_MEASURE | FFTW_PRESERVE_INPUT;
                    fftw_plan_with_nthreads((int)global::numThreads);
                    plan = fftw_plan_dft_r2c_1d((int)n, input, 
                                               reinterpret_cast<fftw_complex *>(output), mode);
                    fftw_double_plans[key] = plan;
                } else {
                    plan = it->second;
                }
                
                fftw_execute_dft_r2c(plan, input, reinterpret_cast<fftw_complex *>(output));
            }
            
            // Cached RFFT implementation for float precision
            LIBRAPID_INLINE void rfft_cached(Complex<float> *output, float *input, size_t n) {
                FFTWPlanKey key{n, false};
                auto it = fftw_float_plans.find(key);
                
                fftwf_plan plan;
                if (it == fftw_float_plans.end()) {
                    // Use MEASURE for better performance characteristics
                    unsigned int mode = FFTW_MEASURE | FFTW_PRESERVE_INPUT;
                    fftwf_plan_with_nthreads((int)global::numThreads);
                    plan = fftwf_plan_dft_r2c_1d((int)n, input, 
                                                reinterpret_cast<fftwf_complex *>(output), mode);
                    fftw_float_plans[key] = plan;
                } else {
                    plan = it->second;
                }
                
                fftwf_execute_dft_r2c(plan, input, reinterpret_cast<fftwf_complex *>(output));
            }
            
        } // namespace detail
        
        namespace cpu {
            // Optimized RFFT functions that use plan caching
            LIBRAPID_INLINE void rfft_optimized(Complex<double> *output, double *input, size_t n) {
                detail::rfft_cached(output, input, n);
            }
            
            LIBRAPID_INLINE void rfft_optimized(Complex<float> *output, float *input, size_t n) {
                detail::rfft_cached(output, input, n);
            }
        } // namespace cpu
        
    } // namespace array
} // namespace librapid

#endif // LIBRAPID_HAS_FFTW

#endif // LIBRAPID_ARRAY_FOURIER_TRANSFORM_OPTIMIZED_HPP