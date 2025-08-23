#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

using namespace librapid;
using Catch::Approx;

TEST_CASE("Test Dual Number Construction and Basic Properties", "[autodiff]") {
    SECTION("Default constructor") {
        Dual<double> d;
        // Note: Default constructor leaves value and derivative uninitialized
        // This is just testing that it compiles
    }

    SECTION("Single value constructor") {
        Dual<double> d(3.14);
        REQUIRE(d.value == Approx(3.14));
        REQUIRE(d.derivative == Approx(0.0));
    }

    SECTION("Value and derivative constructor") {
        Dual<double> d(3.14, 2.71);
        REQUIRE(d.value == Approx(3.14));
        REQUIRE(d.derivative == Approx(2.71));
    }

    SECTION("Copy constructor from different type") {
        Dual<float> df(1.5f, 2.5f);
        Dual<double> dd(df);
        REQUIRE(dd.value == Approx(1.5));
        REQUIRE(dd.derivative == Approx(2.5));
    }

    SECTION("Assignment operators") {
        Dual<double> d1(1.0, 2.0);
        Dual<double> d2(3.0, 4.0);
        
        d1 = d2;
        REQUIRE(d1.value == Approx(3.0));
        REQUIRE(d1.derivative == Approx(4.0));
    }

    SECTION("Size method") {
        Dual<double> d;
        REQUIRE(d.size() > 0); // Should return a positive size
    }
}

TEST_CASE("Test Dual Number Arithmetic Operations", "[autodiff]") {
    SECTION("Addition - Dual + Dual") {
        Dual<double> a(2.0, 1.0);  // f(x) = 2, f'(x) = 1
        Dual<double> b(3.0, 2.0);  // g(x) = 3, g'(x) = 2
        
        auto result = a + b;       // (f + g)(x) = 5, (f + g)'(x) = 3
        REQUIRE(result.value == Approx(5.0));
        REQUIRE(result.derivative == Approx(3.0));
    }

    SECTION("Addition - Dual + Scalar") {
        Dual<double> a(2.0, 3.0);  // f(x) = 2, f'(x) = 3
        double b = 5.0;
        
        auto result = a + b;       // (f + c)(x) = 7, (f + c)'(x) = 3
        REQUIRE(result.value == Approx(7.0));
        REQUIRE(result.derivative == Approx(3.0));
    }

    SECTION("Addition - Scalar + Dual") {
        double a = 5.0;
        Dual<double> b(2.0, 3.0);  // g(x) = 2, g'(x) = 3
        
        auto result = a + b;       // (c + g)(x) = 7, (c + g)'(x) = 3
        REQUIRE(result.value == Approx(7.0));
        REQUIRE(result.derivative == Approx(3.0));
    }

    SECTION("Subtraction - Dual - Dual") {
        Dual<double> a(5.0, 2.0);  // f(x) = 5, f'(x) = 2
        Dual<double> b(3.0, 1.0);  // g(x) = 3, g'(x) = 1
        
        auto result = a - b;       // (f - g)(x) = 2, (f - g)'(x) = 1
        REQUIRE(result.value == Approx(2.0));
        REQUIRE(result.derivative == Approx(1.0));
    }

    SECTION("Subtraction - Dual - Scalar") {
        Dual<double> a(5.0, 3.0);  // f(x) = 5, f'(x) = 3
        double b = 2.0;
        
        auto result = a - b;       // (f - c)(x) = 3, (f - c)'(x) = 3
        REQUIRE(result.value == Approx(3.0));
        REQUIRE(result.derivative == Approx(3.0));
    }

    SECTION("Subtraction - Scalar - Dual") {
        double a = 10.0;
        Dual<double> b(3.0, 2.0);  // g(x) = 3, g'(x) = 2
        
        auto result = a - b;       // (c - g)(x) = 7, (c - g)'(x) = -2
        REQUIRE(result.value == Approx(7.0));
        REQUIRE(result.derivative == Approx(-2.0));
    }

    SECTION("Multiplication - Dual * Dual (Product Rule)") {
        Dual<double> a(2.0, 3.0);  // f(x) = 2, f'(x) = 3
        Dual<double> b(4.0, 5.0);  // g(x) = 4, g'(x) = 5
        
        auto result = a * b;       // (fg)(x) = 8, (fg)'(x) = f'g + fg' = 3*4 + 2*5 = 22
        REQUIRE(result.value == Approx(8.0));
        REQUIRE(result.derivative == Approx(22.0));
    }

    SECTION("Multiplication - Dual * Scalar") {
        Dual<double> a(3.0, 2.0);  // f(x) = 3, f'(x) = 2
        double b = 4.0;
        
        auto result = a * b;       // (f * c)(x) = 12, (f * c)'(x) = c * f' = 4 * 2 = 8
        REQUIRE(result.value == Approx(12.0));
        REQUIRE(result.derivative == Approx(8.0));
    }

    SECTION("Multiplication - Scalar * Dual") {
        double a = 4.0;
        Dual<double> b(3.0, 2.0);  // g(x) = 3, g'(x) = 2
        
        auto result = a * b;       // (c * g)(x) = 12, (c * g)'(x) = c * g' = 4 * 2 = 8
        REQUIRE(result.value == Approx(12.0));
        REQUIRE(result.derivative == Approx(8.0));
    }

    SECTION("Division - Dual / Dual (Quotient Rule)") {
        Dual<double> a(6.0, 2.0);  // f(x) = 6, f'(x) = 2
        Dual<double> b(3.0, 1.0);  // g(x) = 3, g'(x) = 1
        
        auto result = a / b;       // (f/g)(x) = 2, (f/g)'(x) = (f'g - fg')/g² = (2*3 - 6*1)/9 = 0/9 = 0
        REQUIRE(result.value == Approx(2.0));
        REQUIRE(result.derivative == Approx(0.0));
    }

    SECTION("Division - Dual / Scalar") {
        Dual<double> a(8.0, 4.0);  // f(x) = 8, f'(x) = 4
        double b = 2.0;
        
        auto result = a / b;       // (f/c)(x) = 4, (f/c)'(x) = f'/c = 4/2 = 2
        REQUIRE(result.value == Approx(4.0));
        REQUIRE(result.derivative == Approx(2.0));
    }

    SECTION("Division - Scalar / Dual") {
        double a = 12.0;
        Dual<double> b(3.0, 1.0);  // g(x) = 3, g'(x) = 1
        
        auto result = a / b;       // (c/g)(x) = 4, (c/g)'(x) = -c*g'/g² = -12*1/9 = -4/3
        REQUIRE(result.value == Approx(4.0));
        REQUIRE(result.derivative == Approx(-4.0/3.0));
    }

    SECTION("Unary operators") {
        Dual<double> a(3.0, 2.0);
        
        auto neg = -a;
        REQUIRE(neg.value == Approx(-3.0));
        REQUIRE(neg.derivative == Approx(-2.0));
        
        auto pos = +a;
        REQUIRE(pos.value == Approx(3.0));
        REQUIRE(pos.derivative == Approx(2.0));
    }
}

TEST_CASE("Test Dual Number Compound Assignment Operations", "[autodiff]") {
    SECTION("Compound assignment +=") {
        Dual<double> a(2.0, 3.0);
        Dual<double> b(1.0, 1.0);
        
        a += b;
        REQUIRE(a.value == Approx(3.0));
        REQUIRE(a.derivative == Approx(4.0));
    }

    SECTION("Compound assignment -= ") {
        Dual<double> a(5.0, 3.0);
        Dual<double> b(2.0, 1.0);
        
        a -= b;
        REQUIRE(a.value == Approx(3.0));
        REQUIRE(a.derivative == Approx(2.0));
    }

    SECTION("Compound assignment *= (Product Rule)") {
        Dual<double> a(2.0, 3.0);  // f = 2, f' = 3
        Dual<double> b(4.0, 1.0);  // g = 4, g' = 1
        
        a *= b;  // a becomes f*g = 8, derivative = f'*g + f*g' = 3*4 + 2*1 = 14
        REQUIRE(a.value == Approx(8.0));
        REQUIRE(a.derivative == Approx(14.0));
    }

    SECTION("Compound assignment /= (Quotient Rule)") {
        Dual<double> a(8.0, 2.0);  // f = 8, f' = 2
        Dual<double> b(2.0, 1.0);  // g = 2, g' = 1
        
        a /= b;  // a becomes f/g = 4, derivative = (f'*g - f*g')/g² = (2*2 - 8*1)/4 = -1
        REQUIRE(a.value == Approx(4.0));
        REQUIRE(a.derivative == Approx(-1.0));
    }

    SECTION("Compound assignment with scalars") {
        Dual<double> a(3.0, 2.0);
        
        a += 5.0;
        REQUIRE(a.value == Approx(8.0));
        REQUIRE(a.derivative == Approx(2.0));
        
        a -= 3.0;
        REQUIRE(a.value == Approx(5.0));
        REQUIRE(a.derivative == Approx(2.0));
        
        a *= 2.0;
        REQUIRE(a.value == Approx(10.0));
        REQUIRE(a.derivative == Approx(4.0));
        
        a /= 2.0;
        REQUIRE(a.value == Approx(5.0));
        REQUIRE(a.derivative == Approx(2.0));
    }
}

TEST_CASE("Test Dual Number Trigonometric Functions", "[autodiff]") {
    SECTION("sin function - d/dx sin(x) = cos(x)") {
        double x_val = 1.0;
        Dual<double> x(x_val, 1.0);  // x with derivative 1
        
        auto result = sin(x);
        REQUIRE(result.value == Approx(std::sin(x_val)));
        REQUIRE(result.derivative == Approx(std::cos(x_val)));
    }

    SECTION("cos function - d/dx cos(x) = -sin(x)") {
        double x_val = 0.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = cos(x);
        REQUIRE(result.value == Approx(std::cos(x_val)));
        REQUIRE(result.derivative == Approx(-std::sin(x_val)));
    }

    SECTION("tan function - d/dx tan(x) = sec²(x) = 1/cos²(x)") {
        double x_val = 0.3;
        Dual<double> x(x_val, 1.0);
        
        auto result = tan(x);
        double expected_derivative = 1.0 / (std::cos(x_val) * std::cos(x_val));
        REQUIRE(result.value == Approx(std::tan(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("asin function - d/dx asin(x) = 1/sqrt(1-x²)") {
        double x_val = 0.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = asin(x);
        double expected_derivative = 1.0 / std::sqrt(1.0 - x_val * x_val);
        REQUIRE(result.value == Approx(std::asin(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("acos function - d/dx acos(x) = -1/sqrt(1-x²)") {
        double x_val = 0.7;
        Dual<double> x(x_val, 1.0);
        
        auto result = acos(x);
        double expected_derivative = -1.0 / std::sqrt(1.0 - x_val * x_val);
        REQUIRE(result.value == Approx(std::acos(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("atan function - d/dx atan(x) = 1/(1+x²)") {
        double x_val = 2.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = atan(x);
        double expected_derivative = 1.0 / (1.0 + x_val * x_val);
        REQUIRE(result.value == Approx(std::atan(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }
}

TEST_CASE("Test Dual Number Hyperbolic Functions", "[autodiff]") {
    SECTION("sinh function - d/dx sinh(x) = cosh(x)") {
        double x_val = 1.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = sinh(x);
        REQUIRE(result.value == Approx(std::sinh(x_val)));
        REQUIRE(result.derivative == Approx(std::cosh(x_val)));
    }

    SECTION("cosh function - d/dx cosh(x) = sinh(x)") {
        double x_val = 0.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = cosh(x);
        REQUIRE(result.value == Approx(std::cosh(x_val)));
        REQUIRE(result.derivative == Approx(std::sinh(x_val)));
    }

    SECTION("tanh function - d/dx tanh(x) = sech²(x) = 1/cosh²(x)") {
        double x_val = 0.8;
        Dual<double> x(x_val, 1.0);
        
        auto result = tanh(x);
        double expected_derivative = 1.0 / (std::cosh(x_val) * std::cosh(x_val));
        REQUIRE(result.value == Approx(std::tanh(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("asinh function - d/dx asinh(x) = 1/sqrt(x²+1)") {
        double x_val = 1.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = asinh(x);
        double expected_derivative = 1.0 / std::sqrt(x_val * x_val + 1.0);
        REQUIRE(result.value == Approx(std::asinh(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("acosh function - d/dx acosh(x) = 1/sqrt(x²-1)") {
        double x_val = 2.0;  // x > 1 for acosh to be defined
        Dual<double> x(x_val, 1.0);
        
        auto result = acosh(x);
        double expected_derivative = 1.0 / std::sqrt(x_val * x_val - 1.0);
        REQUIRE(result.value == Approx(std::acosh(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("atanh function - d/dx atanh(x) = 1/(1-x²)") {
        double x_val = 0.5;  // |x| < 1 for atanh to be defined
        Dual<double> x(x_val, 1.0);
        
        auto result = atanh(x);
        double expected_derivative = 1.0 / (1.0 - x_val * x_val);
        REQUIRE(result.value == Approx(std::atanh(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }
}

TEST_CASE("Test Dual Number Exponential and Logarithmic Functions", "[autodiff]") {
    SECTION("exp function - d/dx exp(x) = exp(x)") {
        double x_val = 1.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = exp(x);
        REQUIRE(result.value == Approx(std::exp(x_val)));
        REQUIRE(result.derivative == Approx(std::exp(x_val)));
    }

    SECTION("exp2 function - d/dx 2^x = 2^x * ln(2)") {
        double x_val = 2.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = exp2(x);
        double expected_derivative = std::exp2(x_val) * std::log(2.0);
        REQUIRE(result.value == Approx(std::exp2(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("exp10 function - d/dx 10^x = 10^x * ln(10)") {
        double x_val = 1.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = exp10(x);
        double expected_derivative = std::pow(10.0, x_val) * std::log(10.0);
        REQUIRE(result.value == Approx(std::pow(10.0, x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("log function - d/dx log(x) = 1/x") {
        double x_val = 2.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = log(x);
        REQUIRE(result.value == Approx(std::log(x_val)));
        REQUIRE(result.derivative == Approx(1.0 / x_val));
    }

    SECTION("log10 function - d/dx log₁₀(x) = 1/(x*ln(10))") {
        double x_val = 10.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = log10(x);
        double expected_derivative = 1.0 / (x_val * std::log(10.0));
        REQUIRE(result.value == Approx(std::log10(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("log2 function - d/dx log₂(x) = 1/(x*ln(2))") {
        double x_val = 8.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = log2(x);
        double expected_derivative = 1.0 / (x_val * std::log(2.0));
        REQUIRE(result.value == Approx(std::log2(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }
}

TEST_CASE("Test Dual Number Root and Power Functions", "[autodiff]") {
    SECTION("sqrt function - d/dx sqrt(x) = 1/(2*sqrt(x))") {
        double x_val = 4.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = sqrt(x);
        double expected_derivative = 1.0 / (2.0 * std::sqrt(x_val));
        REQUIRE(result.value == Approx(std::sqrt(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("cbrt function - d/dx cbrt(x) = 1/(3*cbrt(x²))") {
        double x_val = 8.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = cbrt(x);
        double expected_derivative = 1.0 / (3.0 * std::cbrt(x_val * x_val));
        REQUIRE(result.value == Approx(std::cbrt(x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("pow function - Dual^Scalar - d/dx x^n = n*x^(n-1)") {
        double x_val = 3.0;
        double exponent = 2.5;
        Dual<double> x(x_val, 1.0);
        
        auto result = pow(x, exponent);
        double expected_derivative = exponent * std::pow(x_val, exponent - 1.0);
        REQUIRE(result.value == Approx(std::pow(x_val, exponent)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("pow function - Scalar^Dual - d/dx a^x = a^x * ln(a)") {
        double base = 2.0;
        double x_val = 3.0;
        Dual<double> x(x_val, 1.0);
        
        auto result = pow(base, x);
        double expected_derivative = std::log(base) * std::pow(base, x_val);
        REQUIRE(result.value == Approx(std::pow(base, x_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("pow function - Dual^Dual - d/dx x^y = x^y * (y'/x * y + y * x'/x)") {
        double x_val = 2.0;
        double y_val = 3.0;
        Dual<double> x(x_val, 1.0);  // dx/dt = 1
        Dual<double> y(y_val, 2.0);  // dy/dt = 2
        
        auto result = pow(x, y);
        // d/dt (x^y) = x^y * (dy/dt * ln(x) + y * dx/dt / x)
        //            = 2^3 * (2 * ln(2) + 3 * 1 / 2)
        //            = 8 * (2 * ln(2) + 1.5)
        double expected_derivative = std::pow(x_val, y_val) * 
                                   (2.0 * std::log(x_val) + y_val * 1.0 / x_val);
        REQUIRE(result.value == Approx(std::pow(x_val, y_val)));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }
}

TEST_CASE("Test Chain Rule with Complex Expressions", "[autodiff]") {
    SECTION("Composite function - sin(x²)") {
        double x_val = 2.0;
        Dual<double> x(x_val, 1.0);
        
        auto x_squared = x * x;                    // x² with derivative 2x
        auto result = sin(x_squared);              // sin(x²) with derivative cos(x²) * 2x
        
        double expected_value = std::sin(x_val * x_val);
        double expected_derivative = std::cos(x_val * x_val) * 2.0 * x_val;
        
        REQUIRE(result.value == Approx(expected_value));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("Composite function - e^(x²+1)") {
        double x_val = 1.5;
        Dual<double> x(x_val, 1.0);
        
        auto inner = x * x + 1.0;                 // x² + 1 with derivative 2x
        auto result = exp(inner);                 // e^(x²+1) with derivative e^(x²+1) * 2x
        
        double inner_val = x_val * x_val + 1.0;
        double expected_value = std::exp(inner_val);
        double expected_derivative = std::exp(inner_val) * 2.0 * x_val;
        
        REQUIRE(result.value == Approx(expected_value));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("Composite function - log(sin(x) + 1)") {
        double x_val = 1.0;
        Dual<double> x(x_val, 1.0);
        
        auto sin_x = sin(x);                      // sin(x) with derivative cos(x)
        auto inner = sin_x + 1.0;                 // sin(x) + 1 with derivative cos(x)
        auto result = log(inner);                 // log(sin(x) + 1) with derivative cos(x)/(sin(x) + 1)
        
        double sin_val = std::sin(x_val);
        double inner_val = sin_val + 1.0;
        double expected_value = std::log(inner_val);
        double expected_derivative = std::cos(x_val) / inner_val;
        
        REQUIRE(result.value == Approx(expected_value));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }
}

TEST_CASE("Test Dual Number Edge Cases and Special Values", "[autodiff]") {
    SECTION("Zero derivative propagation") {
        Dual<double> x(5.0, 0.0);  // Derivative is zero
        
        auto result1 = x + 3.0;
        auto result2 = x * 2.0;
        auto result3 = sin(x);
        
        REQUIRE(result1.derivative == Approx(0.0));
        REQUIRE(result2.derivative == Approx(0.0));
        REQUIRE(result3.derivative == Approx(0.0));
    }

    SECTION("Mixed type operations") {
        Dual<float> df(2.0f, 1.0f);
        Dual<double> dd(3.0, 2.0);
        
        // This tests type conversion in operations
        auto result = df + dd;
        REQUIRE(result.value == Approx(5.0));
        REQUIRE(result.derivative == Approx(3.0));
    }

    SECTION("Large derivative values") {
        Dual<double> x(1.0, 1e6);  // Large derivative
        
        auto result = x * x;  // Should handle large derivative correctly
        REQUIRE(result.value == Approx(1.0));
        REQUIRE(result.derivative == Approx(2e6));
    }

    SECTION("Very small values") {
        Dual<double> x(1e-10, 1.0);
        
        auto result = sqrt(x);
        double expected_derivative = 1.0 / (2.0 * std::sqrt(1e-10));
        
        REQUIRE(result.value == Approx(std::sqrt(1e-10)));
        REQUIRE(result.derivative == Approx(expected_derivative).margin(1e-5));
    }
}

TEST_CASE("Test Multivariate Chain Rule", "[autodiff]") {
    SECTION("Function of two variables - f(u,v) = u*v where u=x², v=sin(x)") {
        double x_val = 1.0;
        Dual<double> x(x_val, 1.0);
        
        auto u = x * x;         // u = x², du/dx = 2x
        auto v = sin(x);        // v = sin(x), dv/dx = cos(x)
        auto result = u * v;    // f = u*v, df/dx = u'*v + u*v' = 2x*sin(x) + x²*cos(x)
        
        double expected_value = x_val * x_val * std::sin(x_val);
        double expected_derivative = 2.0 * x_val * std::sin(x_val) + 
                                   x_val * x_val * std::cos(x_val);
        
        REQUIRE(result.value == Approx(expected_value));
        REQUIRE(result.derivative == Approx(expected_derivative));
    }

    SECTION("Nested composition - f(g(h(x))) where f=exp, g=sin, h=x²") {
        double x_val = 0.5;
        Dual<double> x(x_val, 1.0);
        
        auto h = x * x;         // h(x) = x², h'(x) = 2x
        auto g = sin(h);        // g(h) = sin(h), g'(h) = cos(h)
        auto f = exp(g);        // f(g) = exp(g), f'(g) = exp(g)
        
        // Chain rule: df/dx = f'(g) * g'(h) * h'(x) = exp(sin(x²)) * cos(x²) * 2x
        double h_val = x_val * x_val;
        double g_val = std::sin(h_val);
        double expected_value = std::exp(g_val);
        double expected_derivative = std::exp(g_val) * std::cos(h_val) * 2.0 * x_val;
        
        REQUIRE(f.value == Approx(expected_value));
        REQUIRE(f.derivative == Approx(expected_derivative));
    }
}