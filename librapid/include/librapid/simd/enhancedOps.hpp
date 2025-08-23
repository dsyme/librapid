#ifndef LIBRAPID_SIMD_ENHANCED_OPS_HPP
#define LIBRAPID_SIMD_ENHANCED_OPS_HPP

#include "../utils/memUtils.hpp"

namespace librapid {
	namespace simd {
		/// \brief Cache-optimized transpose for small matrices using SIMD
		/// \tparam T Scalar type
		/// \param src Source matrix data
		/// \param dst Destination matrix data
		/// \param rows Number of rows
		/// \param cols Number of columns
		/// \param srcStride Row stride for source matrix
		/// \param dstStride Row stride for destination matrix
		template<typename T>
		LIBRAPID_ALWAYS_INLINE void transpose4x4(
			const T* src, T* dst, 
			int64_t srcStride, int64_t dstStride) noexcept {
			
			// For 4x4 matrices, use register blocking for optimal performance
			constexpr int64_t blockSize = 4;
			
			// Load 4 rows into SIMD registers
			using PacketType = typename typetraits::TypeInfo<T>::Packet;
			constexpr int64_t packetWidth = typetraits::TypeInfo<T>::packetWidth;
			
			if constexpr (packetWidth >= 4) {
				// Can process full 4x4 block in SIMD
				PacketType row0 = xsimd::load_aligned(src + 0 * srcStride);
				PacketType row1 = xsimd::load_aligned(src + 1 * srcStride);
				PacketType row2 = xsimd::load_aligned(src + 2 * srcStride);
				PacketType row3 = xsimd::load_aligned(src + 3 * srcStride);
				
				// Transpose using SIMD shuffle operations
				auto tmp0 = xsimd::zip_lo(row0, row1);
				auto tmp1 = xsimd::zip_hi(row0, row1);
				auto tmp2 = xsimd::zip_lo(row2, row3);
				auto tmp3 = xsimd::zip_hi(row2, row3);
				
				auto col0 = xsimd::zip_lo(tmp0, tmp2);
				auto col1 = xsimd::zip_hi(tmp0, tmp2);
				auto col2 = xsimd::zip_lo(tmp1, tmp3);
				auto col3 = xsimd::zip_hi(tmp1, tmp3);
				
				// Store transposed columns
				xsimd::store_aligned(dst + 0 * dstStride, col0);
				xsimd::store_aligned(dst + 1 * dstStride, col1);
				xsimd::store_aligned(dst + 2 * dstStride, col2);
				xsimd::store_aligned(dst + 3 * dstStride, col3);
			} else {
				// Fallback to scalar transpose
				for (int64_t i = 0; i < 4; ++i) {
					for (int64_t j = 0; j < 4; ++j) {
						dst[i * dstStride + j] = src[j * srcStride + i];
					}
				}
			}
		}
		
		/// \brief Cache-oblivious transpose for larger matrices
		/// \tparam T Scalar type
		/// \param src Source matrix data
		/// \param dst Destination matrix data
		/// \param rows Number of rows
		/// \param cols Number of columns
		/// \param srcStride Row stride for source matrix
		/// \param dstStride Row stride for destination matrix
		template<typename T>
		LIBRAPID_ALWAYS_INLINE void transposeBlocked(
			const T* src, T* dst, 
			int64_t rows, int64_t cols,
			int64_t srcStride, int64_t dstStride) noexcept {
			
			constexpr int64_t blockSize = 64; // Cache line size
			constexpr int64_t prefetchDistance = 64;
			
			// Process in blocks for better cache utilization
			for (int64_t i = 0; i < rows; i += blockSize) {
				for (int64_t j = 0; j < cols; j += blockSize) {
					int64_t iEnd = std::min(i + blockSize, rows);
					int64_t jEnd = std::min(j + blockSize, cols);
					
					// Prefetch next block
					if (i + blockSize < rows && j + blockSize < cols) {
						prefetch<3>(&src[(i + blockSize) * srcStride + j]);
						prefetchWrite<3>(&dst[j * dstStride + (i + blockSize)]);
					}
					
					// Transpose the current block
					for (int64_t ii = i; ii < iEnd; ++ii) {
						for (int64_t jj = j; jj < jEnd; ++jj) {
							dst[jj * dstStride + ii] = src[ii * srcStride + jj];
						}
					}
				}
			}
		}
		
		/// \brief Optimized complex number multiplication using SIMD
		/// \tparam T Scalar type (float or double)
		/// \param a Complex number a (real, imag)
		/// \param b Complex number b (real, imag)
		/// \return Complex product a * b
		template<typename T>
		LIBRAPID_ALWAYS_INLINE auto complexMul(
			const typename typetraits::TypeInfo<std::complex<T>>::Packet& a,
			const typename typetraits::TypeInfo<std::complex<T>>::Packet& b) noexcept {
			
			// Complex multiplication: (a + bi)(c + di) = (ac - bd) + (ad + bc)i
			// Using SIMD for vectorized complex arithmetic
			using PacketType = typename typetraits::TypeInfo<T>::Packet;
			
			// Extract real and imaginary parts
			auto a_real = xsimd::real(a);
			auto a_imag = xsimd::imag(a);
			auto b_real = xsimd::real(b);
			auto b_imag = xsimd::imag(b);
			
			// Compute real part: ac - bd
			auto real_part = a_real * b_real - a_imag * b_imag;
			
			// Compute imaginary part: ad + bc
			auto imag_part = a_real * b_imag + a_imag * b_real;
			
			return std::complex<PacketType>(real_part, imag_part);
		}
		
		/// \brief Fast reciprocal approximation using SIMD
		/// \tparam T Scalar type
		/// \param x Input values
		/// \return Approximate reciprocals (1/x)
		template<typename T>
		LIBRAPID_ALWAYS_INLINE auto fastReciprocal(
			const typename typetraits::TypeInfo<T>::Packet& x) noexcept {
			
			// Use SIMD reciprocal approximation followed by Newton-Raphson refinement
			auto approx = xsimd::rcp(x); // Initial approximation
			
			// One Newton-Raphson iteration: x' = x * (2 - a * x)
			auto refined = approx * (T(2) - x * approx);
			
			return refined;
		}
		
		/// \brief SIMD-optimized dot product
		/// \tparam T Scalar type
		/// \param a First vector
		/// \param b Second vector
		/// \param n Vector length
		/// \return Dot product a · b
		template<typename T>
		LIBRAPID_ALWAYS_INLINE T dotProduct(const T* a, const T* b, int64_t n) noexcept {
			using PacketType = typename typetraits::TypeInfo<T>::Packet;
			constexpr int64_t packetWidth = typetraits::TypeInfo<T>::packetWidth;
			
			const int64_t vectorSize = n - (n % packetWidth);
			PacketType sum = PacketType(T(0));
			
			// Vectorized main loop with unrolling and prefetching
			constexpr int64_t unrollFactor = 4;
			const int64_t unrolledVectorSize = vectorSize - (vectorSize % (packetWidth * unrollFactor));
			
			for (int64_t i = 0; i < unrolledVectorSize; i += packetWidth * unrollFactor) {
				// Prefetch next cache line
				if (i + 64 < vectorSize) {
					prefetch<3>(&a[i + 64]);
					prefetch<3>(&b[i + 64]);
				}
				
				// Unrolled computation
				auto a0 = xsimd::load_aligned(&a[i]);
				auto b0 = xsimd::load_aligned(&b[i]);
				auto a1 = xsimd::load_aligned(&a[i + packetWidth]);
				auto b1 = xsimd::load_aligned(&b[i + packetWidth]);
				auto a2 = xsimd::load_aligned(&a[i + 2 * packetWidth]);
				auto b2 = xsimd::load_aligned(&b[i + 2 * packetWidth]);
				auto a3 = xsimd::load_aligned(&a[i + 3 * packetWidth]);
				auto b3 = xsimd::load_aligned(&b[i + 3 * packetWidth]);
				
				sum = xsimd::fma(a0, b0, sum);
				sum = xsimd::fma(a1, b1, sum);
				sum = xsimd::fma(a2, b2, sum);
				sum = xsimd::fma(a3, b3, sum);
			}
			
			// Handle remaining vectorized elements
			for (int64_t i = unrolledVectorSize; i < vectorSize; i += packetWidth) {
				auto a_vec = xsimd::load_aligned(&a[i]);
				auto b_vec = xsimd::load_aligned(&b[i]);
				sum = xsimd::fma(a_vec, b_vec, sum);
			}
			
			// Horizontal sum
			T result = xsimd::hadd(sum);
			
			// Handle remaining scalar elements
			for (int64_t i = vectorSize; i < n; ++i) {
				result += a[i] * b[i];
			}
			
			return result;
		}
	} // namespace simd
} // namespace librapid

#endif // LIBRAPID_SIMD_ENHANCED_OPS_HPP