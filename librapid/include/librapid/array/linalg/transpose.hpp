#ifndef LIBRAPID_ARRAY_TRANSPOSE_HPP
#define LIBRAPID_ARRAY_TRANSPOSE_HPP

namespace librapid {
	namespace typetraits {
		template<typename T>
		struct TypeInfo<array::Transpose<T>> {
			static constexpr detail::LibRapidType type = detail::LibRapidType::ArrayFunction;
			using Scalar							   = typename TypeInfo<std::decay_t<T>>::Scalar;
			using Backend	  = typename TypeInfo<std::decay_t<T>>::Backend;
			using ShapeType	  = typename TypeInfo<std::decay_t<T>>::ShapeType;
			using StorageType = typename TypeInfo<std::decay_t<T>>::StorageType;
			static constexpr bool allowVectorisation = false;
		};

		LIBRAPID_DEFINE_AS_TYPE(typename T, array::Transpose<T>);
	} // namespace typetraits

	namespace kernels {
#if defined(LIBRAPID_NATIVE_ARCH)
#	if !defined(LIBRAPID_APPLE) && LIBRAPID_ARCH >= ARCH_AVX2
#		define LIBRAPID_F64_TRANSPOSE_KERNEL_SIZE 4
#		define LIBRAPID_F32_TRANSPOSE_KERNEL_SIZE 8

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeFloatKernel(float *__restrict out,
														 float *__restrict in, Alpha alpha,
														 int64_t cols) {
			__m256 r0, r1, r2, r3, r4, r5, r6, r7;
			__m256 t0, t1, t2, t3, t4, t5, t6, t7;

#		define LOAD256_IMPL(LEFT_, RIGHT_)                                                        \
			_mm256_insertf128_ps(                                                                  \
			  _mm256_castps128_ps256(_mm_loadu_ps(&(LEFT_))), _mm_loadu_ps(&(RIGHT_)), 1)

			r0 = LOAD256_IMPL(in[0 * cols + 0], in[4 * cols + 0]);
			r1 = LOAD256_IMPL(in[1 * cols + 0], in[5 * cols + 0]);
			r2 = LOAD256_IMPL(in[2 * cols + 0], in[6 * cols + 0]);
			r3 = LOAD256_IMPL(in[3 * cols + 0], in[7 * cols + 0]);
			r4 = LOAD256_IMPL(in[0 * cols + 4], in[4 * cols + 4]);
			r5 = LOAD256_IMPL(in[1 * cols + 4], in[5 * cols + 4]);
			r6 = LOAD256_IMPL(in[2 * cols + 4], in[6 * cols + 4]);
			r7 = LOAD256_IMPL(in[3 * cols + 4], in[7 * cols + 4]);

#		undef LOAD256_IMPL

			t0 = _mm256_unpacklo_ps(r0, r1);
			t1 = _mm256_unpackhi_ps(r0, r1);
			t2 = _mm256_unpacklo_ps(r2, r3);
			t3 = _mm256_unpackhi_ps(r2, r3);
			t4 = _mm256_unpacklo_ps(r4, r5);
			t5 = _mm256_unpackhi_ps(r4, r5);
			t6 = _mm256_unpacklo_ps(r6, r7);
			t7 = _mm256_unpackhi_ps(r6, r7);

			r0 = _mm256_shuffle_ps(t0, t2, 0x44);
			r1 = _mm256_shuffle_ps(t0, t2, 0xEE);
			r2 = _mm256_shuffle_ps(t1, t3, 0x44);
			r3 = _mm256_shuffle_ps(t1, t3, 0xEE);
			r4 = _mm256_shuffle_ps(t4, t6, 0x44);
			r5 = _mm256_shuffle_ps(t4, t6, 0xEE);
			r6 = _mm256_shuffle_ps(t5, t7, 0x44);
			r7 = _mm256_shuffle_ps(t5, t7, 0xEE);

			__m256 alphaVec = _mm256_set1_ps(alpha);

			_mm256_storeu_ps(out + 0 * cols, _mm256_mul_ps(r0, alphaVec));
			_mm256_storeu_ps(out + 1 * cols, _mm256_mul_ps(r1, alphaVec));
			_mm256_storeu_ps(out + 2 * cols, _mm256_mul_ps(r2, alphaVec));
			_mm256_storeu_ps(out + 3 * cols, _mm256_mul_ps(r3, alphaVec));
			_mm256_storeu_ps(out + 4 * cols, _mm256_mul_ps(r4, alphaVec));
			_mm256_storeu_ps(out + 5 * cols, _mm256_mul_ps(r5, alphaVec));
			_mm256_storeu_ps(out + 6 * cols, _mm256_mul_ps(r6, alphaVec));
			_mm256_storeu_ps(out + 7 * cols, _mm256_mul_ps(r7, alphaVec));
		}

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeDoubleKernel(double *__restrict out,
														  double *__restrict in, Alpha alpha,
														  int64_t cols) {
			__m256d r0, r1, r2, r3;
			__m256d t0, t1, t2, t3;

			r0 = _mm256_loadu_pd(in + 0 * cols);
			r1 = _mm256_loadu_pd(in + 1 * cols);
			r2 = _mm256_loadu_pd(in + 2 * cols);
			r3 = _mm256_loadu_pd(in + 3 * cols);

			t0 = _mm256_unpacklo_pd(r0, r1);
			t1 = _mm256_unpackhi_pd(r0, r1);
			t2 = _mm256_unpacklo_pd(r2, r3);
			t3 = _mm256_unpackhi_pd(r2, r3);

			r0 = _mm256_permute2f128_pd(t0, t2, 0x20);
			r1 = _mm256_permute2f128_pd(t1, t3, 0x20);
			r2 = _mm256_permute2f128_pd(t0, t2, 0x31);
			r3 = _mm256_permute2f128_pd(t1, t3, 0x31);

			__m256d alphaVec = _mm256_set1_pd(alpha);

			_mm256_storeu_pd(out + 0 * cols, _mm256_mul_pd(r0, alphaVec));
			_mm256_storeu_pd(out + 1 * cols, _mm256_mul_pd(r1, alphaVec));
			_mm256_storeu_pd(out + 2 * cols, _mm256_mul_pd(r2, alphaVec));
			_mm256_storeu_pd(out + 3 * cols, _mm256_mul_pd(r3, alphaVec));
		}

#	elif !defined(LIBRAPID_APPLE) && LIBRAPID_ARCH >= ARCH_SSE
#		define LIBRAPID_F64_TRANSPOSE_KERNEL_SIZE 2
#		define LIBRAPID_F32_TRANSPOSE_KERNEL_SIZE 4

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeFloatKernel(float *__restrict out,
														 float *__restrict in, Alpha alpha,
														 int64_t cols) {
			__m128 tmp0, tmp1, tmp2, tmp3;

			tmp0 = _mm_shuffle_ps(_mm_loadu_ps(in + 0 * cols), _mm_loadu_ps(in + 1 * cols), 0x44);
			tmp2 = _mm_shuffle_ps(_mm_loadu_ps(in + 0 * cols), _mm_loadu_ps(in + 1 * cols), 0xEE);
			tmp1 = _mm_shuffle_ps(_mm_loadu_ps(in + 2 * cols), _mm_loadu_ps(in + 3 * cols), 0x44);
			tmp3 = _mm_shuffle_ps(_mm_loadu_ps(in + 2 * cols), _mm_loadu_ps(in + 3 * cols), 0xEE);

			__m128 alphaVec = _mm_set1_ps(alpha);

			_mm_storeu_ps(out + 0 * cols, _mm_mul_ps(_mm_shuffle_ps(tmp0, tmp1, 0x88), alphaVec));
			_mm_storeu_ps(out + 1 * cols, _mm_mul_ps(_mm_shuffle_ps(tmp0, tmp1, 0xDD), alphaVec));
			_mm_storeu_ps(out + 2 * cols, _mm_mul_ps(_mm_shuffle_ps(tmp2, tmp3, 0x88), alphaVec));
			_mm_storeu_ps(out + 3 * cols, _mm_mul_ps(_mm_shuffle_ps(tmp2, tmp3, 0xDD), alphaVec));
		}

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeDoubleKernel(double *__restrict out,
														  double *__restrict in, Alpha alpha,
														  int64_t cols) {
			__m128d tmp0, tmp1;

			tmp0 = _mm_loadu_pd(in + 0 * cols);
			tmp1 = _mm_loadu_pd(in + 1 * cols);

			// Transpose 2x2
			__m128d tmp0Unpck = _mm_unpacklo_pd(tmp0, tmp1);
			__m128d tmp1Unpck = _mm_unpackhi_pd(tmp0, tmp1);

			// Multiply with alpha
			__m128d alphaVec = _mm_set1_pd(alpha);
			_mm_storeu_pd(out + 0 * cols, _mm_mul_pd(tmp0Unpck, alphaVec));
			_mm_storeu_pd(out + 1 * cols, _mm_mul_pd(tmp1Unpck, alphaVec));
		}

#	else
#		define LIBRAPID_F64_TRANSPOSE_KERNEL_SIZE 4
#		define LIBRAPID_F32_TRANSPOSE_KERNEL_SIZE 4

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeFloatKernel(float *__restrict out,
														 float *__restrict in, Alpha alpha,
														 int64_t cols) {
			for (int r = 0; r < 4; ++r) {
				for (int c = 0; c < 4; ++c) { out[c * cols + r] = in[r * cols + c] * alpha; }
			}
		}

		template<typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeDoubleKernel(double *__restrict out,
														  double *__restrict in, Alpha alpha,
														  int64_t cols) {
			for (int r = 0; r < 4; ++r) {
				for (int c = 0; c < 4; ++c) { out[c * cols + r] = in[r * cols + c] * alpha; }
			}
		}

#	endif

		template<typename T, typename Alpha>
		LIBRAPID_ALWAYS_INLINE void transposeInplace(Array<T> &array, Alpha alpha = Alpha(1.0)) {
			LIBRAPID_ASSERT(array.shape()[0] == array.shape()[1],
							"Cannot transpose non-square matrix in-place");

			auto *ptr = array.storage().begin();
			int64_t rows = array.shape()[0];

			for (int64_t i = 0; i < rows; ++i) {
				for (int64_t j = i + 1; j < rows; ++j) {
					T tmp		   = ptr[i * rows + j];
					ptr[i * rows + j] = ptr[j * rows + i] * alpha;
					ptr[j * rows + i] = tmp * alpha;
				}
			}

			// Scale diagonal if alpha is not 1
			if (alpha != Alpha(1.0)) {
				for (int64_t i = 0; i < rows; ++i) { ptr[i * rows + i] *= alpha; }
			}
		}

		namespace cpu {
			/// CPU specific transpose function that applies a memory transpose with SIMD kernels
			/// \tparam T The data type
			/// \tparam Alpha The alpha type (scalar multiplier)
			/// \param out The output array
			/// \param in The input array
			/// \param rows The number of rows in the input array
			/// \param cols The number of columns in the input array
			/// \param alpha The scalar multiplier
			/// \param blockSize The block size to use for the transpose
			template<typename T, typename Alpha>
			LIBRAPID_ALWAYS_INLINE void
			transpose(T *__restrict out, const T *__restrict in, int64_t rows, int64_t cols,
					  Alpha alpha, size_t blockSize = 64) {
				using Scalar = std::decay_t<T>;

				if constexpr (std::is_same_v<Scalar, float> || std::is_same_v<Scalar, double>) {
					constexpr size_t kernelSize =
					  (std::is_same_v<Scalar, float>) ? LIBRAPID_F32_TRANSPOSE_KERNEL_SIZE
															  : LIBRAPID_F64_TRANSPOSE_KERNEL_SIZE;

#if defined(LIBRAPID_NATIVE_ARCH)
					for (int64_t i = 0; i < rows; i += kernelSize) {
						for (int64_t j = 0; j < cols; j += kernelSize) {
							// Ensure we don't go out of bounds
							if (i + kernelSize <= rows && j + kernelSize <= cols) {
								// Use the kernel
								if constexpr (std::is_same_v<Scalar, float>) {
									transposeFloatKernel(
									  out + j * rows + i, const_cast<float *>(in) + i * cols + j,
									  alpha, cols);
								} else {
									transposeDoubleKernel(
									  out + j * rows + i, const_cast<double *>(in) + i * cols + j,
									  alpha, cols);
								}
							} else {
								// Fallback for out-of-bounds elements
								for (int64_t ib = i;
									 ib < std::min(i + (int64_t)kernelSize, rows); ++ib) {
									for (int64_t jb = j;
										 jb < std::min(j + (int64_t)kernelSize, cols); ++jb) {
										out[jb * rows + ib] = in[ib * cols + jb] * alpha;
									}
								}
							}
						}
					}
#else
					// Fallback implementation without SIMD
					for (int64_t i = 0; i < rows; ++i) {
						for (int64_t j = 0; j < cols; ++j) {
							out[j * rows + i] = in[i * cols + j] * alpha;
						}
					}
#endif
				} else {
					// Fallback for non-float types or blocked transpose for large matrices
					for (int64_t i = 0; i < rows; i += blockSize) {
						for (int64_t j = 0; j < cols; j += blockSize) {
							int64_t maxI = std::min(i + (int64_t)blockSize, rows);
							int64_t maxJ = std::min(j + (int64_t)blockSize, cols);
							for (int64_t ib = i; ib < maxI; ++ib) {
								for (int64_t jb = j; jb < maxJ; ++jb) {
									out[jb * rows + ib] = in[ib * cols + jb] * alpha;
								}
							}
						}
					}
				}
			}
		} // namespace cpu

		namespace opencl {
#if defined(LIBRAPID_HAS_OPENCL)
			template<typename T, typename Alpha>
			LIBRAPID_ALWAYS_INLINE void transpose(const cl::Buffer &out, const cl::Buffer &in,
													  int64_t rows, int64_t cols, Alpha alpha,
													  int64_t outOffset, int64_t inOffset = 0) {
				std::string kernelNameTmp = "transpose_" + typetraits::typeName<T>();
				std::string kernelName	   = kernelNameTmp;

				cl::Kernel kernel = opencl::getKernel(kernelName);
				kernel.setArg(0, out);
				kernel.setArg(1, in);
				kernel.setArg(2, (cl_long)rows);
				kernel.setArg(3, (cl_long)cols);
				kernel.setArg(4, (cl_long)outOffset);
				kernel.setArg(5, (cl_long)inOffset);
				kernel.setArg(6, alpha);

				cl::NDRange globalWorkSize(rows, cols);
				opencl::enqueueKernel(kernel, globalWorkSize);
			}
#endif
		} // namespace opencl

#endif
	} // namespace kernels

	namespace array {
		template<typename TransposeType>
		class Transpose {
		public:
			using ArrayType = TransposeType;
			using BaseType	= typename std::decay_t<TransposeType>;
			using Scalar	= typename typetraits::TypeInfo<BaseType>::Scalar;
			using ShapeType = typename BaseType::ShapeType;
			using Backend	= typename typetraits::TypeInfo<BaseType>::Backend;

			static constexpr bool allowVectorisation =
			  typetraits::TypeInfo<Scalar>::allowVectorisation;
			static constexpr bool isArray  = typetraits::IsArrayContainer<BaseType>::value;
			static constexpr bool isHost   = std::is_same_v<Backend, backend::CPU>;
			static constexpr bool isOpenCL = std::is_same_v<Backend, backend::OpenCL>;
			static constexpr bool isCUDA   = std::is_same_v<Backend, backend::CUDA>;

			/// Default constructor should never be used
			Transpose() = delete;

			/// Create a Transpose object from an array/operation
			/// \param array The array to copy
			/// \param axes The transposition axes
			Transpose(TransposeType &&array, const ShapeType &axes, Scalar alpha = Scalar(1.0));

			/// Copy a Transpose object
			Transpose(const Transpose &other) = default;

			/// Move constructor
			Transpose(Transpose &&other) noexcept = default;

			/// Assign another Transpose object to this one
			/// \param other The Transpose object to copy
			/// \return A reference to this object
			auto operator=(const Transpose &other) -> Transpose & = default;

			/// Move assign another Transpose object to this one
			/// \param other The Transpose object to move
			/// \return A reference to this object
			auto operator=(Transpose &&other) noexcept -> Transpose & = default;

			/// Access the shape of the transposed array
			/// \return The shape of the transposed array
			LIBRAPID_NODISCARD auto shape() const -> const ShapeType &;

			/// Access the number of dimensions of the transposed array
			/// \return The number of dimensions of the transposed array
			LIBRAPID_NODISCARD auto ndim() const -> int64_t;

			/// Access the size of the transposed array
			/// \return The size of the transposed array
			LIBRAPID_NODISCARD auto size() const -> int64_t;

			/// Access a scalar element of the transposed array
			/// \param index The index of the element to access
			/// \return The element at the given index
			template<typename T = Transpose>
			LIBRAPID_NODISCARD auto scalar(int64_t index) const -> auto;

			/// Access the transposition axes
			/// \return The transposition axes
			LIBRAPID_NODISCARD auto axes() const -> const ShapeType &;

			/// Access the alpha scaling factor
			/// \return The alpha scaling factor
			LIBRAPID_NODISCARD auto alpha() const -> const Scalar &;

			/// Access the array being transposed
			/// \return The array being transposed
			LIBRAPID_NODISCARD auto array() const -> const ArrayType &;

			/// Access the array being transposed
			/// \return The array being transposed
			LIBRAPID_NODISCARD auto array() -> ArrayType &;

			/// Assign the result of this transpose operation to an array
			/// \tparam T_ The type of the array to assign to
			/// \param out The array to assign to
			template<typename T_>
			void assignTo(T_ &out) const;

			/// Evaluate the transpose operation, returning a new array
			/// \return A new array containing the result of the transpose operation
			template<typename T = Transpose>
			LIBRAPID_NODISCARD auto eval() const;

		private:
			ArrayType m_array;
			ShapeType m_inputShape;
			ShapeType m_outputShape;
			size_t m_outputSize;
			ShapeType m_axes;
			Scalar m_alpha;
		};

		// Implementation
		template<typename TransposeType>
		Transpose<TransposeType>::Transpose(TransposeType &&array, const ShapeType &axes,
											Scalar alpha) :
				m_array(std::forward<TransposeType>(array)), m_inputShape(m_array.shape()),
				m_axes(axes), m_alpha(alpha) {
			m_outputShape = m_inputShape;
			for (size_t i = 0; i < m_inputShape.ndim(); i++) {
				m_outputShape[i] = m_inputShape[m_axes[i]];
			}

			m_outputSize = m_outputShape.size();
		}

		template<typename T>
		auto Transpose<T>::shape() const -> const ShapeType & {
			return m_outputShape;
		}

		template<typename T>
		auto Transpose<T>::size() const -> int64_t {
			return m_outputSize;
		}

		template<typename T>
		auto Transpose<T>::ndim() const -> int64_t {
			return m_outputShape.ndim();
		}

		template<typename T>
		auto Transpose<T>::scalar(int64_t index) const -> auto {
			// Optimized scalar access: calculate original index directly without full evaluation
			auto outputCoords = m_outputShape.coords(index);
			ShapeType originalCoords = ShapeType::zeros(m_inputShape.ndim());
			
			// Apply reverse transpose mapping to get original coordinates
			for (size_t i = 0; i < m_inputShape.ndim(); i++) {
				originalCoords[m_axes[i]] = outputCoords[i];
			}
			
			// Calculate original linear index and access the underlying data
			int64_t originalIndex = m_inputShape.index(originalCoords);
			return m_array.scalar(originalIndex) * m_alpha;
		}

		template<typename T>
		auto Transpose<T>::axes() const -> const ShapeType & {
			return m_axes;
		}

		template<typename T>
		auto Transpose<T>::alpha() const -> const Scalar & {
			return m_alpha;
		}

		template<typename T>
		auto Transpose<T>::array() const -> const ArrayType & {
			return m_array;
		}

		template<typename T>
		auto Transpose<T>::array() -> ArrayType & {
			return m_array;
		}

		template<typename T>
		template<typename T_>
		void Transpose<T>::assignTo(T_ &out) const {
			bool inplace = ((void *)&out) == ((void *)&m_array);

			LIBRAPID_ASSERT(out.shape() == m_outputShape, "Transpose assignment shape mismatch");

			if (m_inputShape.ndim() == 2 && m_axes[0] == 1 && m_axes[1] == 0) {
				// 2D transpose -- use optimized kernel
				if constexpr (isHost) {
					auto *__restrict outPtr = out.storage().begin();
					auto *__restrict inPtr	= m_array.storage().begin();

					constexpr size_t blockSize = 64;
					kernels::cpu::transpose(
					  outPtr, inPtr, m_inputShape[0], m_inputShape[1], m_alpha, blockSize);
				} else if constexpr (isOpenCL) {
#if defined(LIBRAPID_HAS_OPENCL)
					const cl::Buffer &outBuffer = out.storage().data();
					const cl::Buffer &inBuffer = m_array.storage().data();

					kernels::opencl::transpose<Scalar>(
					  outBuffer, inBuffer, m_inputShape[0], m_inputShape[1], m_alpha, 0);
#else
					LIBRAPID_ERROR(
					  "LibRapid was not compiled with OpenCL support, so this operation is not "
					  "supported. Please recompile LibRapid with OpenCL support or switch to the "
					  "CPU backend.");
#endif
				} else if constexpr (isCUDA) {
#if defined(LIBRAPID_HAS_CUDA)
					if (inplace) {
						kernels::transposeInplace(out, m_alpha);
					} else {
						auto *__restrict outPtr = out.storage().begin();
						auto *__restrict inPtr	= m_array.storage().begin();
						kernels::cpu::transpose(
						  outPtr, inPtr, m_inputShape[0], m_inputShape[1], m_alpha, blockSize);
					}
#endif
				}
			} else {
				// General transpose -- evaluate and assign
				out = eval();
			}
		}

		template<typename T>
		template<typename T_>
		auto Transpose<T>::eval() const {
			using NonConstType		= typename std::decay_t<T>;
			using NonConstArrayType = typename typetraits::TypeInfo<NonConstType>::StorageType;
			using Type				= typename typetraits::TypeInfo<NonConstType>::Scalar;

				NonConstArrayType res(m_outputShape);
				assignTo(res);
				return res;
			} else {
				auto tmp   = m_array.eval();
				using Type = typename typetraits::TypeInfo<decltype(tmp)>::Scalar;
				return Transpose<Type>(std::forward<Type>(tmp), m_axes, m_alpha).eval();
			}
		}

		/// \brief Create a Transpose functor object
		/// \tparam T The type of the array to transpose
		/// \param array The array to transpose
		/// \param alpha The alpha scaling factor
		/// \return A Transpose functor object
		template<typename T>
		LIBRAPID_NODISCARD auto transpose(T &&array, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			using ArrayType = std::decay_t<T>;
			using ShapeType = typename ArrayType::ShapeType;

			// Default transpose axes (reverse order)
			ShapeType axes = ShapeType::zeros(array.ndim());
			for (int64_t i = 0; i < array.ndim(); ++i) { axes[i] = array.ndim() - 1 - i; }

			return Transpose<T>(std::forward<T>(array), axes, alpha);
		}

		/// \brief Create a Transpose functor object with custom axes
		/// \tparam T The type of the array to transpose
		/// \param array The array to transpose
		/// \param axes The transposition axes
		/// \param alpha The alpha scaling factor
		/// \return A Transpose functor object
		template<typename T, typename ShapeType_>
		LIBRAPID_NODISCARD auto transpose(T &&array, const ShapeType_ &axes, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			return Transpose<T>(std::forward<T>(array), axes, alpha);
		}

	} // namespace array

	namespace linalg {
		/// \brief Transpose an array
		/// \tparam T The type of the array
		/// \param array The array to transpose
		/// \param alpha The alpha scaling factor
		/// \return A transpose functor object
		template<typename T>
		LIBRAPID_NODISCARD auto transpose(T &&array, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			return array::transpose(std::forward<T>(array), alpha);
		}

		/// \brief Transpose an array with custom axes
		/// \tparam T The type of the array
		/// \param array The array to transpose
		/// \param axes The transposition axes
		/// \param alpha The alpha scaling factor
		/// \return A transpose functor object
		template<typename T, typename ShapeType_>
		LIBRAPID_NODISCARD auto transpose(T &&array, const ShapeType_ &axes, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			return array::transpose(std::forward<T>(array), axes, alpha);
		}

		/// \brief Transpose an array
		/// \tparam T The type of the array
		/// \param array The array to transpose
		/// \param alpha The alpha scaling factor
		/// \return A transpose functor object
		template<typename T>
		LIBRAPID_NODISCARD auto T(T &&array, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			return array::transpose(std::forward<T>(array), alpha);
		}

		/// \brief Transpose an array with custom axes
		/// \tparam T The type of the array
		/// \param array The array to transpose
		/// \param axes The transposition axes
		/// \param alpha The alpha scaling factor
		/// \return A transpose functor object
		template<typename T, typename ShapeType_>
		LIBRAPID_NODISCARD auto T(T &&array, const ShapeType_ &axes, typename typetraits::TypeInfo<std::decay_t<T>>::Scalar alpha = 1) {
			return array::transpose(std::forward<T>(array), axes, alpha);
		}
	} // namespace linalg
} // namespace librapid

#endif // LIBRAPID_ARRAY_TRANSPOSE_HPP