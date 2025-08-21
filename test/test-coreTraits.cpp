#include <librapid/librapid.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test core traits functionality", "[coreTraits]") {
    SECTION("typeName functionality") {
        // Test typeName for basic types
        auto intName = librapid::typetraits::typeName<int>();
        auto doubleName = librapid::typetraits::typeName<double>();
        auto floatName = librapid::typetraits::typeName<float>();
        
        REQUIRE(!intName.empty());
        REQUIRE(!doubleName.empty());
        REQUIRE(!floatName.empty());
        
        // Type names should be different
        REQUIRE(intName != doubleName);
        REQUIRE(doubleName != floatName);
        REQUIRE(intName != floatName);
        
        // Test with const types
        auto constIntName = librapid::typetraits::typeName<const int>();
        auto constDoubleName = librapid::typetraits::typeName<const double>();
        
        REQUIRE(!constIntName.empty());
        REQUIRE(!constDoubleName.empty());
    }
    
    SECTION("TypeInfo basic properties") {
        // Test TypeInfo for int
        using IntInfo = librapid::typetraits::TypeInfo<int>;
        REQUIRE(IntInfo::supportsArithmetic == true);
        REQUIRE(IntInfo::supportsLogical == true);
        REQUIRE(IntInfo::supportsBinary == true);
        REQUIRE(IntInfo::canAlign == true);
        REQUIRE(IntInfo::canMemcpy == true);
        REQUIRE(IntInfo::packetWidth >= 1);
        
        // Test TypeInfo for float
        using FloatInfo = librapid::typetraits::TypeInfo<float>;
        REQUIRE(FloatInfo::supportsArithmetic == true);
        REQUIRE(FloatInfo::supportsLogical == true);
        REQUIRE(FloatInfo::supportsBinary == false);  // Floats don't support binary ops
        REQUIRE(FloatInfo::allowVectorisation == true);
        REQUIRE(FloatInfo::canAlign == true);
        REQUIRE(FloatInfo::canMemcpy == true);
        
        // Test TypeInfo for double
        using DoubleInfo = librapid::typetraits::TypeInfo<double>;
        REQUIRE(DoubleInfo::supportsArithmetic == true);
        REQUIRE(DoubleInfo::supportsLogical == true);
        REQUIRE(DoubleInfo::supportsBinary == false);  // Doubles don't support binary ops
        REQUIRE(DoubleInfo::allowVectorisation == true);
        REQUIRE(DoubleInfo::canAlign == true);
        REQUIRE(DoubleInfo::canMemcpy == true);
        
        // Test TypeInfo for bool
        using BoolInfo = librapid::typetraits::TypeInfo<bool>;
        REQUIRE(BoolInfo::supportsArithmetic == false);
        REQUIRE(BoolInfo::supportsLogical == false);
        REQUIRE(BoolInfo::supportsBinary == true);
        REQUIRE(BoolInfo::allowVectorisation == false);
        REQUIRE(BoolInfo::canAlign == true);
        REQUIRE(BoolInfo::canMemcpy == true);
    }
    
    SECTION("TypeInfo with different integer types") {
        // Test various integer types
        using Int8Info = librapid::typetraits::TypeInfo<int8_t>;
        using Int16Info = librapid::typetraits::TypeInfo<int16_t>;
        using Int32Info = librapid::typetraits::TypeInfo<int32_t>;
        using Int64Info = librapid::typetraits::TypeInfo<int64_t>;
        
        REQUIRE(Int8Info::supportsArithmetic == true);
        REQUIRE(Int16Info::supportsArithmetic == true);
        REQUIRE(Int32Info::supportsArithmetic == true);
        REQUIRE(Int64Info::supportsArithmetic == true);
        
        REQUIRE(Int8Info::allowVectorisation == true);
        REQUIRE(Int16Info::allowVectorisation == true);
        REQUIRE(Int32Info::allowVectorisation == true);
        REQUIRE(Int64Info::allowVectorisation == false);  // 64-bit integers typically don't vectorize as well
        
        // Test unsigned variants
        using UInt8Info = librapid::typetraits::TypeInfo<uint8_t>;
        using UInt16Info = librapid::typetraits::TypeInfo<uint16_t>;
        using UInt32Info = librapid::typetraits::TypeInfo<uint32_t>;
        using UInt64Info = librapid::typetraits::TypeInfo<uint64_t>;
        
        REQUIRE(UInt8Info::supportsArithmetic == true);
        REQUIRE(UInt16Info::supportsArithmetic == true);
        REQUIRE(UInt32Info::supportsArithmetic == true);
        REQUIRE(UInt64Info::supportsArithmetic == true);
    }
    
    SECTION("TypeInfo reference and pointer types") {
        // Test that reference types inherit from base type
        using IntRefInfo = librapid::typetraits::TypeInfo<int&>;
        using IntInfo = librapid::typetraits::TypeInfo<int>;
        
        REQUIRE(IntRefInfo::supportsArithmetic == IntInfo::supportsArithmetic);
        REQUIRE(IntRefInfo::supportsLogical == IntInfo::supportsLogical);
        REQUIRE(IntRefInfo::supportsBinary == IntInfo::supportsBinary);
        
        // Test const types
        using ConstIntInfo = librapid::typetraits::TypeInfo<const int>;
        REQUIRE(ConstIntInfo::supportsArithmetic == IntInfo::supportsArithmetic);
        
        // Test pointer types
        using IntPtrInfo = librapid::typetraits::TypeInfo<int*>;
        REQUIRE(IntPtrInfo::supportsArithmetic == IntInfo::supportsArithmetic);
    }
    
    SECTION("LibRapidType enum") {
        // Test that sameType function works
        using namespace librapid::detail;
        
        REQUIRE(sameType(LibRapidType::Scalar, LibRapidType::Scalar) == true);
        REQUIRE(sameType(LibRapidType::Scalar, LibRapidType::Vector) == false);
        REQUIRE(sameType(LibRapidType::Vector, LibRapidType::ArrayContainer) == false);
        REQUIRE(sameType(LibRapidType::ArrayContainer, LibRapidType::ArrayContainer) == true);
    }
    
    SECTION("NumericInfo functionality") {
        // Test NumericInfo for double
        using DoubleNumInfo = librapid::typetraits::NumericInfo<double>;
        
        // These should compile without errors and return reasonable values
        REQUIRE_NOTHROW(DoubleNumInfo::min());
        REQUIRE_NOTHROW(DoubleNumInfo::max());
        REQUIRE_NOTHROW(DoubleNumInfo::epsilon());
        
        // Sanity checks for numeric limits
        REQUIRE(DoubleNumInfo::min() > 0.0);  // min() returns smallest positive normalized value
        REQUIRE(DoubleNumInfo::max() > 0.0);
        REQUIRE(DoubleNumInfo::epsilon() > 0.0);
        REQUIRE(DoubleNumInfo::epsilon() < 1.0);
    }
}