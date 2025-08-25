#ifndef LIBRAPID_ARRAY_FOURIER_TRANFORM_HPP
#define LIBRAPID_ARRAY_FOURIER_TRANFORM_HPP

#if defined(LIBRAPID_HAS_FFTW) || defined(LIBRAPID_HAS_CUDA)
#include <unordered_map>
#include <mutex>
#endif

namespace librapid::fft {
    namespace detail {
#if defined(LIBRAPID_HAS_FFTW) || defined(LIBRAPID_HAS_CUDA)
        // Plan cache infrastructure for FFTW plans
        struct FFTWPlanKey {
            size_t n;
            bool forward;
            
            bool operator==(const FFTWPlanKey& other) const {
                return n == other.n && forward == other.forward;
            }
        };
        
        struct FFTWPlanKeyHash {
            size_t operator()(const FFTWPlanKey& key) const {
                return std::hash<size_t>{}(key.n) ^ (std::hash<bool>{}(key.forward) << 1);
            }
        };
        
        using FFTWDoublePlanCache = std::unordered_map<FFTWPlanKey, fftw_plan, FFTWPlanKeyHash>;
        using FFTWFloatPlanCache = std::unordered_map<FFTWPlanKey, fftwf_plan, FFTWPlanKeyHash>;
#endif

#if defined(LIBRAPID_HAS_CUDA)
        // Plan cache infrastructure for cuFFT plans  
        struct CuFFTPlanKey {
            size_t n;
            cufftType type;
            
            bool operator==(const CuFFTPlanKey& other) const {
                return n == other.n && type == other.type;
            }
        };
        
        struct CuFFTPlanKeyHash {
            size_t operator()(const CuFFTPlanKey& key) const {
                return std::hash<size_t>{}(key.n) ^ (std::hash<int>{}(static_cast<int>(key.type)) << 1);
            }
        };
        
        using CuFFTPlanCache = std::unordered_map<CuFFTPlanKey, cufftHandle, CuFFTPlanKeyHash>;
#endif

        namespace cpu {
            template<typename T>
            void rfft(Complex<T> *output, T *input, size_t n) {
                pocketfft::shape_t shape      = {n};
                pocketfft::stride_t strideIn  = {sizeof(T)};
                pocketfft::stride_t strideOut = {sizeof(Complex<T>)};
                size_t axis                   = 0;
                bool forward                  = true;
                T fct                         = 1.0;
                pocketfft::r2c(shape,
                               strideIn,
                               strideOut,
                               axis,
                               forward,
                               input,
                               reinterpret_cast<std::complex<T> *>(output),
                               fct,
                               global::numThreads);
            }

#if defined(LIBRAPID_HAS_CUDA) || defined(LIBRAPID_HAS_FFTW)
            LIBRAPID_INLINE void rfft(Complex<double> *output, double *input, size_t n) {
                // Thread-local plan cache for optimal performance
                thread_local FFTWDoublePlanCache fftw_double_plans;
                
                FFTWPlanKey key{n, true}; // true = forward transform
                auto it = fftw_double_plans.find(key);
                fftw_plan plan;
                
                if (it == fftw_double_plans.end()) {
                    // Create new plan with FFTW_MEASURE for better performance
                    unsigned int mode = FFTW_MEASURE | FFTW_PRESERVE_INPUT;
#if !defined(LIBRAPID_HAS_CUDA) && defined(LIBRAPID_HAS_FFTW)
                    fftw_plan_with_nthreads((int)global::numThreads);
#endif
                    plan = fftw_plan_dft_r2c_1d(
                        (int)n, input, reinterpret_cast<fftw_complex *>(output), mode);
                    fftw_double_plans[key] = plan;
                } else {
                    plan = it->second;
                }
                
                fftw_execute_dft_r2c(plan, input, reinterpret_cast<fftw_complex *>(output));
            }

            LIBRAPID_INLINE void rfft(Complex<float> *output, float *input, size_t n) {
                // Thread-local plan cache for optimal performance
                thread_local FFTWFloatPlanCache fftw_float_plans;
                
                FFTWPlanKey key{n, true}; // true = forward transform
                auto it = fftw_float_plans.find(key);
                fftwf_plan plan;
                
                if (it == fftw_float_plans.end()) {
                    // Create new plan with FFTW_MEASURE for better performance
                    unsigned int mode = FFTW_MEASURE | FFTW_PRESERVE_INPUT;
#if !defined(LIBRAPID_HAS_CUDA) && defined(LIBRAPID_HAS_FFTW)
                    fftwf_plan_with_nthreads((int)global::numThreads);
#endif
                    plan = fftwf_plan_dft_r2c_1d(
                        (int)n, input, reinterpret_cast<fftwf_complex *>(output), mode);
                    fftw_float_plans[key] = plan;
                } else {
                    plan = it->second;
                }
                
                fftwf_execute_dft_r2c(plan, input, reinterpret_cast<fftwf_complex *>(output));
            }
#endif
        } // namespace cpu

#if defined(LIBRAPID_HAS_CUDA)
        namespace gpu {
            LIBRAPID_INLINE void rfft(Complex<double> *output, double *input, size_t n) {
                // Thread-local plan cache for optimal GPU performance
                thread_local CuFFTPlanCache cufft_plans;
                
                CuFFTPlanKey key{n, CUFFT_D2Z};
                auto it = cufft_plans.find(key);
                cufftHandle plan;
                
                if (it == cufft_plans.end()) {
                    // Create new cuFFT plan
                    cufftPlan1d(&plan, (int)n, CUFFT_D2Z, 1);
                    cufftSetStream(plan, global::cudaStream);
                    cufft_plans[key] = plan;
                } else {
                    plan = it->second;
                    cufftSetStream(plan, global::cudaStream); // Ensure correct stream
                }
                
                cufftExecD2Z(plan, input, reinterpret_cast<cufftDoubleComplex *>(output));
            }

            LIBRAPID_INLINE void rfft(Complex<float> *output, float *input, size_t n) {
                // Thread-local plan cache for optimal GPU performance
                thread_local CuFFTPlanCache cufft_plans;
                
                CuFFTPlanKey key{n, CUFFT_R2C};
                auto it = cufft_plans.find(key);
                cufftHandle plan;
                
                if (it == cufft_plans.end()) {
                    // Create new cuFFT plan
                    cufftPlan1d(&plan, (int)n, CUFFT_R2C, 1);
                    cufftSetStream(plan, global::cudaStream);
                    cufft_plans[key] = plan;
                } else {
                    plan = it->second;
                    cufftSetStream(plan, global::cudaStream); // Ensure correct stream
                }
                
                cufftExecR2C(plan, input, reinterpret_cast<cufftComplex *>(output));
            }
        } // namespace gpu
#endif    // LIBRAPID_HAS_CUDA
    }     // namespace detail

    /// \brief Clean up FFT plan caches (called automatically at thread exit)
    namespace detail {
#if defined(LIBRAPID_HAS_FFTW) || defined(LIBRAPID_HAS_CUDA)
        inline void cleanup_thread_local_plans() {
            // Note: Thread-local destructors will automatically clean up plans
            // This function is provided for manual cleanup if needed
            // Plans are automatically destroyed when threads exit
        }
#endif
    }

    /// \brief Compute the real-valued discrete Fourier transform of a 1D array
    ///
    /// Given a 1D array of real numbers, compute the discrete Fourier transform of the array. This
    /// returns an array of length \f$\frac{n}{2} + 1\f$ where \f$n\f$ is the length of the input
    /// array. The returned array contains the non-redundant half of the resulting transform, since
    /// the other half can be obtained by taking the complex conjugate of the first half.
    ///
    /// \tparam ShapeType The shape type of the input array
    /// \tparam StorageScalar The scalar type of the input array
    /// \param array The input array
    /// \return The discrete Fourier transform of the input array
    template<typename ShapeType, typename StorageScalar>
    LIBRAPID_NODISCARD auto rfft(array::ArrayContainer<ShapeType, Storage<StorageScalar>> &array)
      -> Array<Complex<StorageScalar>, backend::CPU> {
        LIBRAPID_ASSERT(array.ndim() == 1, "RFFT only implemented for 1D arrays");
        int64_t outSize = array.shape()[0] / 2 + 1;
        Array<Complex<StorageScalar>, backend::CPU> res(Shape({outSize}));
        StorageScalar *input           = array.storage().begin();
        Complex<StorageScalar> *output = res.storage().begin();
        detail::cpu::rfft(output, input, array.shape()[0]);
        return res;
    }

#if defined(LIBRAPID_HAS_CUDA)
    template<typename ShapeType, typename StorageScalar>
    LIBRAPID_NODISCARD auto
    rfft(array::ArrayContainer<ShapeType, CudaStorage<StorageScalar>> &array)
      -> Array<Complex<StorageScalar>, backend::CUDA> {
        LIBRAPID_ASSERT(array.ndim() == 1, "RFFT only implemented for 1D arrays");
        int64_t outSize = array.shape()[0] / 2 + 1;
        Array<Complex<StorageScalar>, backend::CUDA> res(Shape({outSize}));
        StorageScalar *input           = array.storage().begin().get();
        Complex<StorageScalar> *output = res.storage().begin().get();
        detail::gpu::rfft(output, input, array.shape()[0]);
        return res;
    }
#endif // LIBRAPID_HAS_CUDA
} // namespace librapid::fft

#endif // LIBRAPID_ARRAY_FOURIER_TRANFORM_HPP
