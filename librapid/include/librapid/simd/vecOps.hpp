#ifndef LIBRAPID_SIMD_TRIGONOMETRY
#define LIBRAPID_SIMD_TRIGONOMETRY

namespace librapid {
	namespace typetraits {
		template<typename T>
		struct IsSIMD : std::false_type {};

		template<typename T, typename U>
		struct IsSIMD<xsimd::batch<T, U>> : std::true_type {};

		template<typename T>
		struct IsSIMD<xsimd::batch_element_reference<T>> : std::true_type {};

		template<typename T>
		concept SIMD = IsSIMD<T>::value;
	} // namespace typetraits

#define IS_FLOATING(TYPE)  std::is_floating_point_v<TYPE>

#define SIMD_OP_IMPL(OP)                                                                           \
	using Scalar = typename typetraits::TypeInfo<T>::Scalar;                                       \
	if constexpr (IS_FLOATING(Scalar)) {                                                           \
		return xsimd::OP(x);                                                                       \
	} else {                                                                                       \
		T result;                                                                                  \
		constexpr uint64_t packetWidth = typetraits::TypeInfo<Scalar>::packetWidth;                \
		for (size_t i = 0; i < packetWidth; ++i) { result.set(i, std::OP(x.get(i))); }             \
		return result;                                                                             \
	}

// Optimized integer SIMD operations using actual vectorization
#define SIMD_INTEGER_OP_IMPL(OP)                                                                  \
	using Scalar = typename typetraits::TypeInfo<T>::Scalar;                                      \
	if constexpr (IS_FLOATING(Scalar)) {                                                          \
		return xsimd::OP(x);                                                                      \
	} else if constexpr (std::is_integral_v<Scalar>) {                                            \
		/* Use vectorized operations for integers where possible */                               \
		if constexpr (std::is_same_v<Scalar, int32_t> || std::is_same_v<Scalar, uint32_t>) {      \
			/* 32-bit integers have good SIMD support */                                         \
			return xsimd::OP(x);                                                                  \
		} else if constexpr (std::is_same_v<Scalar, int64_t> || std::is_same_v<Scalar, uint64_t>) { \
			/* 64-bit integers - limited SIMD support, but still better than scalar */          \
			return xsimd::OP(x);                                                                  \
		} else {                                                                                  \
			/* Fall back to element-wise for other integer types */                              \
			T result;                                                                             \
			constexpr uint64_t packetWidth = typetraits::TypeInfo<Scalar>::packetWidth;           \
			for (size_t i = 0; i < packetWidth; ++i) { result.set(i, std::OP(x.get(i))); }        \
			return result;                                                                        \
		}                                                                                         \
	} else {                                                                                      \
		T result;                                                                                 \
		constexpr uint64_t packetWidth = typetraits::TypeInfo<Scalar>::packetWidth;               \
		for (size_t i = 0; i < packetWidth; ++i) { result.set(i, std::OP(x.get(i))); }            \
		return result;                                                                            \
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto sin(const T &x) {
		SIMD_OP_IMPL(sin)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto cos(const T &x) {
		SIMD_OP_IMPL(cos)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto tan(const T &x) {
		SIMD_OP_IMPL(tan)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto asin(const T &x) {
		SIMD_OP_IMPL(asin)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto acos(const T &x) {
		SIMD_OP_IMPL(acos)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto atan(const T &x) {
		SIMD_OP_IMPL(atan)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto sinh(const T &x) {
		SIMD_OP_IMPL(sinh)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto cosh(const T &x) {
		SIMD_OP_IMPL(cosh)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto tanh(const T &x) {
		SIMD_OP_IMPL(tanh)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto exp(const T &x) {
		SIMD_OP_IMPL(exp)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto log(const T &x) {
		SIMD_OP_IMPL(log)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto log2(const T &x) {
		SIMD_OP_IMPL(log2)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto log10(const T &x) {
		SIMD_OP_IMPL(log10)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto sqrt(const T &x) {
		SIMD_OP_IMPL(sqrt)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto cbrt(const T &x) {
		SIMD_OP_IMPL(cbrt)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto abs(const T &x) {
		SIMD_INTEGER_OP_IMPL(abs)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto floor(const T &x) {
		SIMD_OP_IMPL(floor)
	}

	template<typename T>
		requires(typetraits::SIMD<T>)
	LIBRAPID_NODISCARD LIBRAPID_ALWAYS_INLINE auto ceil(const T &x) {
		SIMD_OP_IMPL(ceil)
	}
} // namespace librapid

#endif // LIBRAPID_SIMD_TRIGONOMETRY