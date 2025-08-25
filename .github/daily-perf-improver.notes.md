# Perf Improvement Developer Guide

## Overview
LibRapid is a high-performance C++20/23 mathematical library focused on:
- Multi-dimensional arrays with lazy evaluation
- SIMD vectorization and OpenMP parallelization
- BLAS/LAPACK integration for linear algebra
- GPU compute via CUDA/OpenCL
- Multiprecision arithmetic (MPIR/MPFR)

## Performance Architecture

### Core Performance Features
1. **Lazy Evaluation**: Operations are deferred and optimized at compile-time
2. **SIMD Vectorization**: Using Vc library for automatic vectorization
3. **Parallel Execution**: OpenMP for CPU threading, CUDA/OpenCL for GPU
4. **BLAS Integration**: OpenBLAS for optimized linear algebra operations
5. **Native Architecture Optimization**: `-march=native` compilation

### Performance Testing Setup

#### Build Commands
```bash
# Debug build with profiling
mkdir buildDebugProfile && cd buildDebugProfile
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_FLAGS="-g -O0 -fprofile-instr-generate -fcoverage-mapping" \
  -DLIBRAPID_BUILD_TESTS=ON -DLIBRAPID_USE_BLAS=ON -DLIBRAPID_USE_OMP=ON

# Optimized release build  
mkdir buildReleaseOptimized && cd buildReleaseOptimized
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_FLAGS="-O3 -march=native -DNDEBUG" \
  -DLIBRAPID_BUILD_TESTS=ON -DLIBRAPID_USE_BLAS=ON \
  -DLIBRAPID_FAST_MATH=ON -DLIBRAPID_NATIVE_ARCH=ON \
  -DLIBRAPID_OPTIMISE_SMALL_ARRAYS=ON
```

#### Testing Commands
```bash
# Run unit tests
cd buildReleaseOptimized && ctest -C Release --output-on-failure

# Run benchmarks (if available)
./librapid_perf_test --benchmark_format=json --benchmark_out=results.json
```

### Profiling and Analysis

#### CPU Profiling with perf
```bash
# Profile specific functions
perf record -g ./your_benchmark
perf report

# Generate flame graphs
perf script | flamegraph.pl > flamegraph.svg
```

#### Memory Analysis with Valgrind
```bash
# Memory profiling
valgrind --tool=callgrind ./your_benchmark
kcachegrind callgrind.out.*
```

### Typical Performance Bottlenecks

1. **Memory Access Patterns**
   - Non-contiguous array access
   - Cache misses in large array operations
   - Memory allocation in hot paths

2. **Algorithm Efficiency**  
   - Unnecessary temporary objects (avoid with lazy evaluation)
   - Suboptimal BLAS usage (ensure proper eval() calls)
   - Loop vectorization issues

3. **Compilation Issues**
   - Missing SIMD optimization flags
   - Compiler not optimizing lazy expressions
   - Link-time optimization not enabled

### Performance Improvement Goals

#### Round 1: Low-hanging Fruit
- Fix obvious algorithmic inefficiencies
- Enable compiler optimizations (`-O3`, `-march=native`, `-flto`)
- Reduce memory allocations in hot paths
- Improve cache locality

#### Round 2: Advanced Optimizations  
- SIMD vectorization of custom kernels
- OpenMP parallelization of array operations
- CUDA kernel optimization for GPU operations
- Template metaprogramming optimizations

#### Round 3: Architecture-specific Tuning
- CPU-specific instruction sets (AVX2, AVX-512)
- Memory prefetching hints
- Branch prediction optimization
- Custom BLAS kernel implementations

### Micro-benchmarking Best Practices

#### Google Benchmark Integration
```cpp
#include <benchmark/benchmark.h>
#include <librapid/librapid.hpp>

static void BM_ArrayOp(benchmark::State& state) {
    auto size = state.range(0);
    Array<float> a = random<float>({size, size});
    Array<float> b = random<float>({size, size});
    
    for (auto _ : state) {
        auto result = a + b;
        benchmark::DoNotOptimize(result);
    }
    state.SetComplexityN(size * size);
}
BENCHMARK(BM_ArrayOp)->RangeMultiplier(2)->Range(8, 1024)->Complexity();
```

#### Key Measurements
- CPU cycles per operation
- Memory bandwidth utilization  
- Cache hit ratios
- Vectorization efficiency
- Parallel scalability

### Environment Setup
The build-steps action configures:
- Performance CPU governor
- Profiling tools (perf, valgrind, flamegraph)
- Optimized compilers (Clang 16+ with LTO)
- BLAS libraries (OpenBLAS)
- Benchmark frameworks (Google Benchmark)

### Realistic Performance Expectations
- **Array operations**: 80-90% of theoretical memory bandwidth
- **Linear algebra**: Near-BLAS performance (5-15% overhead max)  
- **Parallel scaling**: Linear up to memory bandwidth limit
- **SIMD efficiency**: 3-4x speedup on 256-bit vector units

### Key Performance Hotspots
1. Array expression evaluation (`array/operations.hpp`)
2. BLAS integration (`array/linalg/level*.hpp`)  
3. Memory allocation (`array/storage.hpp`)
4. SIMD operations (`simd/vecOps.hpp`)
5. CUDA kernels (`cuda/kernels/*.cu`)

### Measurement Infrastructure
- Unit: Individual function performance
- Integration: Full expression evaluation chains
- System: Large-scale mathematical workloads  
- Regression: Continuous benchmarking vs baseline