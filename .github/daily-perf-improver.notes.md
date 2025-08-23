# Perf Improvement Developer Guide

## Performance Testing in LibRapid

### Current Benchmarking Infrastructure
- **Google Benchmark**: Integrated micro-benchmarking framework with `BENCHMARK` macros
- **Catch2 Tests**: Test suite with benchmarking capabilities using `--skip-benchmarks` flag control  
- **Manual Performance Tests**: Sample benchmark executable in build steps action
- **Build Configuration**: Performance optimizations via CMake flags (`LIBRAPID_FAST_MATH`, `LIBRAPID_NATIVE_ARCH`)

### Micro-Benchmarking Approach
- **Array Operations**: Addition, multiplication, matrix operations benchmarked with range multipliers
- **SIMD Testing**: Vectorized operations with different data sizes (8-512 elements)
- **Memory Patterns**: Cache-aware benchmarking for different array sizes and access patterns
- **Linear Algebra**: BLAS-backed operations (GEMM, GEMV) with complexity analysis

### Typical Workloads & Use Cases
- **Numerical Computing**: Multi-dimensional array operations, linear algebra, FFT processing
- **Machine Learning**: Activation functions, tensor operations, GPU acceleration
- **Scientific Computing**: High-precision arithmetic (MPIR/MPFR), complex numbers, vectorization
- **Real-time Processing**: Small fixed arrays, SIMD optimization, minimal memory allocation

### Performance Bottlenecks (Historical Analysis)
1. **Memory-bound Operations**: Array assignments, data structure initialization, cache misses
2. **Algorithmic Inefficiencies**: O(n) set insertions, "heinously inefficient" transpose scalar access
3. **GPU Operations**: Incorrect transpose indexing, plan creation overhead in FFT
4. **Synchronization Issues**: Global static RNG state, thread contention in multithreaded code
5. **SIMD Utilization**: Missing vectorization for integer operations, bitset manipulation

### Performance Characteristics
- **I/O Bound**: Serialization operations, file system access
- **CPU Bound**: Mathematical functions, SIMD operations, loop-heavy computations  
- **Memory Bound**: Large array operations, memory prefetching opportunities
- **GPU Bound**: Linear algebra operations, parallel array processing

### Maintainer Priorities
1. **Correctness First**: Critical bug fixes (math function dispatch, GPU GEMM indexing)
2. **Zero API Impact**: All optimizations must maintain backward compatibility
3. **Cross-Platform**: Support for x86-64, ARM, various SIMD instruction sets
4. **Template Optimization**: Compile-time optimizations using C++20/23 features
5. **Hardware Utilization**: SIMD vectorization, cache optimization, thread-local optimizations

### Performance Improvement Rounds

#### Round 1 Goals (Foundational)
- **Infrastructure**: Build system, profiling tools, benchmark integration
- **Critical Fixes**: Correctness bugs in mathematical functions, GPU operations
- **Low-Hanging Fruit**: Memory prefetching, basic SIMD acceleration, thread-local optimizations

#### Round 2 Goals (Algorithmic)  
- **Data Structures**: O(n) → O(log n) improvements, cache-oblivious algorithms
- **Memory Optimization**: Custom allocators, storage pattern improvements
- **Advanced SIMD**: Complex number acceleration, bitset vectorization

#### Round 3 Goals (Advanced)
- **Auto-tuning**: Runtime adaptation based on hardware detection
- **Profile-guided**: Compiler PGO integration for optimal code generation
- **GPU Enhancement**: CUDA/OpenCL kernel optimization, memory coalescing

### Previous Optimization Areas (Avoid Overlap)
- Mathematical function bugs (exp2/exp10, inverse hyperbolic functions) ✓
- Memory prefetching in array operations ✓  
- Set data structure insertion algorithms ✓
- Transpose operation scalar access ✓
- FFT plan caching systems ✓
- Thread-local RNG state ✓
- Integer SIMD vectorization ✓
- GPU GEMM correctness and optimization ✓

### Realistic Performance Goals
- **10-100x**: Plan caching, algorithmic improvements (O(n) → O(log n))
- **2-10x**: SIMD vectorization, thread-local optimizations, GPU fixes
- **15-50%**: Memory prefetching, loop unrolling, cache optimization
- **Zero overhead**: Small fixed arrays, compile-time optimizations

### Performance Measurement Best Practices
- **Consistent Environment**: Fixed CPU governor, isolated cores when possible
- **Statistical Significance**: Multiple runs with outlier detection
- **Size Scaling**: Test across different problem sizes (small/medium/large)
- **Hardware Variation**: Account for virtualized environment limitations
- **Validation**: Ensure correctness before measuring performance improvements