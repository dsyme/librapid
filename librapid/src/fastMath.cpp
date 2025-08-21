#include <librapid/librapid.hpp>

namespace librapid::fastmath {
    double pow10(int64_t exponent) {
        const static double pows[] = {0.0000001,
                                      0.000001,
                                      0.00001,
                                      0.0001,
                                      0.001,
                                      0.01,
                                      0.1,
                                      1,
                                      10,
                                      100,
                                      1000,
                                      10000,
                                      100000,
                                      1000000,
                                      10000000};

        if (exponent >= -7 && exponent <= 7) return pows[exponent + 7];

        double res = 1;

        if (exponent > 0) {
            // Optimized loop unrolling for better performance
            int64_t remaining = exponent;
            while (remaining >= 4) {
                res *= 10000.0;  // 10^4
                remaining -= 4;
            }
            while (remaining >= 2) {
                res *= 100.0;    // 10^2
                remaining -= 2;
            }
            if (remaining == 1) {
                res *= 10.0;
            }
        } else {
            // Optimized loop for negative exponents
            int64_t remaining = -exponent;
            while (remaining >= 4) {
                res *= 0.0001;   // 10^-4
                remaining -= 4;
            }
            while (remaining >= 2) {
                res *= 0.01;     // 10^-2
                remaining -= 2;
            }
            if (remaining == 1) {
                res *= 0.1;
            }
        }

        return res;
    }
} // namespace librapid::fastmath