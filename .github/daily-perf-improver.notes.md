# Perf Improvement Developer Guide

## Performance Testing in LibRapid

### How Performance Testing is Done
- External benchmark repository: https://github.com/LibRapid/BenchmarksCPP.git (referenced in CI)
- Uses Catch2 testing framework for unit tests
- Built-in Timer class in `librapid/include/librapid/utils/time.hpp` for micro-benchmarks
- CTest integration for test execution
- Tests compile in both Debug and Release modes

### Micro Benchmarking Tools
- `librapid::Timer` class with nanosecond precision timing
- Supports target time-based benchmarking with automatic iteration counting
- Built-in time formatting from ns to years
- Average time calculation over multiple iterations
- Cross-platform timing (Windows QueryPerformanceCounter, Unix high_resolution_clock)

### Typical Workloads
- Array arithmetic operations (element-wise operations)
- Linear algebra: GEMM, GEMV operations via BLAS
- FFT operations via FFTW
- SIMD vectorized operations
- GPU computations (CUDA/OpenCL backends)
- Multi-precision arithmetic (MPIR/MPFR)
- Complex number operations

### Performance Bottlenecks
- Memory allocation/deallocation (lazy evaluation helps)
- Cache misses in large array operations
- Suboptimal BLAS library usage
- Inefficient lazy evaluation chains
- Non-vectorized operations
- Thread synchronization overhead

### Performance Characteristics
- **CPU bound**: Mathematical operations, vectorization opportunities
- **Memory bound**: Large array operations, cache efficiency critical
- **I/O bound**: Minimal, mainly computational workload

### Maintainer Priorities
1. Lazy evaluation efficiency
2. BLAS/LAPACK integration optimization
3. SIMD vectorization
4. Multi-threading scalability
5. GPU acceleration effectiveness
6. Minimal memory overhead

### Performance Goals
**Round 1**: Basic optimizations
- Improve lazy evaluation chains
- Optimize small array operations
- Better SIMD utilization

**Round 2**: Advanced optimizations  
- GPU kernel optimization
- Memory layout improvements
- Cache-friendly algorithms

**Round 3**: Specialized optimizations
- Custom BLAS routines
- Architecture-specific tuning
- Advanced vectorization

### Build Commands
```bash
# Configure for performance testing
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \\
         -DLIBRAPID_BUILD_TESTS=ON \\
         -DLIBRAPID_BUILD_EXAMPLES=ON \\
         -DLIBRAPID_USE_BLAS=ON \\
         -DLIBRAPID_GET_BLAS=ON \\
         -DLIBRAPID_USE_OMP=ON \\
         -DLIBRAPID_NATIVE_ARCH=ON \\
         -DLIBRAPID_FAST_MATH=ON \\
         -DLIBRAPID_USE_OPENCL=OFF \\
         -DLIBRAPID_USE_CUDA=OFF

# Build
cmake --build . --config Release

# Run tests
ctest -C Release --output-on-failure
```

### Benchmarking Setup
```bash
# Clone benchmark repo
git clone https://github.com/LibRapid/BenchmarksCPP.git --recursive
cd BenchmarksCPP/vendor/librapid
# Update to current branch
git pull && git submodule update --init --recursive

# Build benchmarks  
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \\
         -DLIBRAPID_OPTIMISE_SMALL_ARRAYS=ON \\
         -DLIBRAPID_GET_BLAS=ON \\
         -DLIBRAPID_USE_BLAS=ON

cmake --build . --config Release
./BenchmarksCPP
```

### Profiling Tools
- Built-in `librapid::Timer` for precise timing
- Valgrind integration available (`cmake/valgrindTarget.cmake`)
- Coverage analysis with gcov/gcovr
- Use compiler optimization flags: `-O3`, `-ffast-math`, `-march=native`

### Key Performance Files
- `librapid/include/librapid/array/operations.hpp` - Core array operations
- `librapid/include/librapid/array/linalg/` - Linear algebra routines
- `librapid/include/librapid/simd/` - SIMD optimizations
- `librapid/cuda/kernels/` - GPU kernels
- `librapid/cxxblas/` - BLAS interface

### Environment Setup
1. Install dependencies: OpenBLAS, FFTW, OpenMP
2. Clone with submodules: `git clone --recursive`
3. Use Release build configuration
4. Enable native architecture optimizations
5. Link with optimized BLAS library

### Micro-benchmark Template
```cpp
#include <librapid>

void benchmarkFunction() {
    librapid::Timer timer("Function Benchmark");
    timer.setTargetTime<librapid::time::second>(2.0); // Run for 2 seconds
    
    while (timer.isRunning()) {
        // Code to benchmark
        performOperation();
    }
    
    fmt::print("Results: {}\\n", timer);
}
```