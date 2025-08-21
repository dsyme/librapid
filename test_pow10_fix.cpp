#include <librapid/librapid.hpp>
#include <iostream>
#include <cassert>

int main() {
    // Test the pow10 fix
    std::cout << "Testing pow10 function fix..." << std::endl;
    
    // Test the bug case that was wrong before
    double result = librapid::fastmath::pow10(7);
    std::cout << "pow10(7) = " << result << std::endl;
    assert(result == 10000000.0);  // Should be 10^7 = 10,000,000
    
    // Test some other cases
    assert(librapid::fastmath::pow10(0) == 1.0);
    assert(librapid::fastmath::pow10(1) == 10.0);
    assert(librapid::fastmath::pow10(2) == 100.0);
    assert(librapid::fastmath::pow10(6) == 1000000.0);
    assert(librapid::fastmath::pow10(-1) == 0.1);
    assert(librapid::fastmath::pow10(-2) == 0.01);
    
    // Test large exponents (uses optimized loop)
    assert(librapid::fastmath::pow10(8) == 100000000.0);
    assert(librapid::fastmath::pow10(12) == 1000000000000.0);
    
    std::cout << "All pow10 tests passed! ✓" << std::endl;
    return 0;
}