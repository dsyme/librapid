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

        // For large exponents, use more efficient exponentiation
        if (exponent > 0) {
            // Use binary exponentiation for large positive powers
            double base = 10.0;
            double result = 1.0;
            int64_t exp = exponent;
            while (exp > 0) {
                if (exp & 1) result *= base;
                base *= base;
                exp >>= 1;
            }
            return result;
        } else {
            // Use binary exponentiation for large negative powers
            double base = 0.1;
            double result = 1.0;
            int64_t exp = -exponent;
            while (exp > 0) {
                if (exp & 1) result *= base;
                base *= base;
                exp >>= 1;
            }
            return result;
        }
    }
} // namespace librapid::fastmath