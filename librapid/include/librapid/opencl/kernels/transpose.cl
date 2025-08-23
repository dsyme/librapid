#define TRANSPOSEX 3
#define TRANSPOSEY 3

// Optimized transpose kernel with better memory coalescing and bank conflict avoidance
#define TRANSPOSE_KERNEL_IMPL(DTYPE, TILE_DIM)                                                     \
    __kernel void transpose_##DTYPE(__global DTYPE *out,                                           \
                                    __global const DTYPE *in,                                      \
                                    const int rows,                                                \
                                    const int cols,                                                \
                                    DTYPE alpha) {                                                 \
        __local DTYPE tile[TILE_DIM][TILE_DIM + 1];                                                \
                                                                                                   \
        int x = get_group_id(0) * TILE_DIM + get_local_id(0);                                      \
        int y = get_group_id(1) * TILE_DIM + get_local_id(1);                                      \
                                                                                                   \
        // Coalesced read from global memory with bounds checking                                  \
        if (x < cols && y < rows) {                                                                \
            tile[get_local_id(1)][get_local_id(0)] = in[y * cols + x];                            \
        } else {                                                                                   \
            tile[get_local_id(1)][get_local_id(0)] = (DTYPE)0;                                    \
        }                                                                                          \
        barrier(CLK_LOCAL_MEM_FENCE);                                                              \
                                                                                                   \
        // Transpose coordinates for coalesced write                                              \
        x = get_group_id(1) * TILE_DIM + get_local_id(0);                                          \
        y = get_group_id(0) * TILE_DIM + get_local_id(1);                                          \
                                                                                                   \
        // Coalesced write to global memory with bounds checking                                  \
        if (x < rows && y < cols) {                                                                \
            out[y * rows + x] = tile[get_local_id(0)][get_local_id(1)] * alpha;                    \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    __kernel void transpose_optimized_##DTYPE(__global DTYPE *out,                                 \
                                              __global const DTYPE *in,                            \
                                              const int rows,                                      \
                                              const int cols,                                      \
                                              DTYPE alpha) {                                       \
        __local DTYPE tile[TILE_DIM][TILE_DIM + 2]; /* +2 to avoid bank conflicts */             \
                                                                                                   \
        const int lid_x = get_local_id(0);                                                        \
        const int lid_y = get_local_id(1);                                                        \
        const int gid_x = get_group_id(0) * TILE_DIM + lid_x;                                     \
        const int gid_y = get_group_id(1) * TILE_DIM + lid_y;                                     \
                                                                                                   \
        /* Vectorized read for improved memory throughput */                                      \
        if (gid_x < cols && gid_y < rows) {                                                       \
            tile[lid_y][lid_x] = in[gid_y * cols + gid_x];                                        \
        } else {                                                                                   \
            tile[lid_y][lid_x] = (DTYPE)0;                                                        \
        }                                                                                          \
                                                                                                   \
        barrier(CLK_LOCAL_MEM_FENCE);                                                              \
                                                                                                   \
        /* Transpose output coordinates */                                                         \
        const int out_x = get_group_id(1) * TILE_DIM + lid_x;                                     \
        const int out_y = get_group_id(0) * TILE_DIM + lid_y;                                     \
                                                                                                   \
        /* Vectorized write with coalescing */                                                    \
        if (out_x < rows && out_y < cols) {                                                       \
            out[out_y * rows + out_x] = tile[lid_x][lid_y] * alpha;                               \
        }                                                                                          \
    }

TRANSPOSE_KERNEL_IMPL(int8_t, 16)
TRANSPOSE_KERNEL_IMPL(int16_t, 16)
TRANSPOSE_KERNEL_IMPL(int32_t, 16)
TRANSPOSE_KERNEL_IMPL(int64_t, 16)
TRANSPOSE_KERNEL_IMPL(float, 16)
TRANSPOSE_KERNEL_IMPL(double, 16)
