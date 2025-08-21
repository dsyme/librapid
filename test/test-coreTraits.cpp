#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <librapid/librapid.hpp>

using namespace librapid;

TEST_CASE("LibRapidType enum values are distinct", "[core][traits][sameType]") {
	// Test that all enum values are unique
	REQUIRE(detail::LibRapidType::Scalar != detail::LibRapidType::Dual);
	REQUIRE(detail::LibRapidType::Scalar != detail::LibRapidType::Vector);
	REQUIRE(detail::LibRapidType::Scalar != detail::LibRapidType::ArrayContainer);
	REQUIRE(detail::LibRapidType::Scalar != detail::LibRapidType::ArrayFunction);
	REQUIRE(detail::LibRapidType::Scalar != detail::LibRapidType::GeneralArrayView);
	
	REQUIRE(detail::LibRapidType::Dual != detail::LibRapidType::Vector);
	REQUIRE(detail::LibRapidType::Dual != detail::LibRapidType::ArrayContainer);
	REQUIRE(detail::LibRapidType::Dual != detail::LibRapidType::ArrayFunction);
	REQUIRE(detail::LibRapidType::Dual != detail::LibRapidType::GeneralArrayView);
	
	REQUIRE(detail::LibRapidType::Vector != detail::LibRapidType::ArrayContainer);
	REQUIRE(detail::LibRapidType::Vector != detail::LibRapidType::ArrayFunction);
	REQUIRE(detail::LibRapidType::Vector != detail::LibRapidType::GeneralArrayView);
	
	REQUIRE(detail::LibRapidType::ArrayContainer != detail::LibRapidType::ArrayFunction);
	REQUIRE(detail::LibRapidType::ArrayContainer != detail::LibRapidType::GeneralArrayView);
	
	REQUIRE(detail::LibRapidType::ArrayFunction != detail::LibRapidType::GeneralArrayView);
}

TEST_CASE("sameType function - identical types", "[core][traits][sameType]") {
	// Test that sameType returns true for identical types
	REQUIRE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::Scalar));
	REQUIRE(detail::sameType(detail::LibRapidType::Dual, detail::LibRapidType::Dual));
	REQUIRE(detail::sameType(detail::LibRapidType::Vector, detail::LibRapidType::Vector));
	REQUIRE(detail::sameType(detail::LibRapidType::ArrayContainer, detail::LibRapidType::ArrayContainer));
	REQUIRE(detail::sameType(detail::LibRapidType::ArrayFunction, detail::LibRapidType::ArrayFunction));
	REQUIRE(detail::sameType(detail::LibRapidType::GeneralArrayView, detail::LibRapidType::GeneralArrayView));
}

TEST_CASE("sameType function - different types", "[core][traits][sameType]") {
	// Test that sameType returns false for different types
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::Dual));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::Vector));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::ArrayContainer));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::ArrayFunction));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Scalar, detail::LibRapidType::GeneralArrayView));
	
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Dual, detail::LibRapidType::Vector));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Dual, detail::LibRapidType::ArrayContainer));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Dual, detail::LibRapidType::ArrayFunction));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Dual, detail::LibRapidType::GeneralArrayView));
	
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Vector, detail::LibRapidType::ArrayContainer));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Vector, detail::LibRapidType::ArrayFunction));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::Vector, detail::LibRapidType::GeneralArrayView));
	
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::ArrayContainer, detail::LibRapidType::ArrayFunction));
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::ArrayContainer, detail::LibRapidType::GeneralArrayView));
	
	REQUIRE_FALSE(detail::sameType(detail::LibRapidType::ArrayFunction, detail::LibRapidType::GeneralArrayView));
}

TEST_CASE("sameType function - reflexivity", "[core][traits][sameType]") {
	// Test reflexive property: sameType(a, a) == true for all a
	auto allTypes = {
		detail::LibRapidType::Scalar,
		detail::LibRapidType::Dual,
		detail::LibRapidType::Vector,
		detail::LibRapidType::ArrayContainer,
		detail::LibRapidType::ArrayFunction,
		detail::LibRapidType::GeneralArrayView
	};
	
	for (const auto& type : allTypes) {
		REQUIRE(detail::sameType(type, type));
	}
}

TEST_CASE("sameType function - symmetry", "[core][traits][sameType]") {
	// Test symmetric property: sameType(a, b) == sameType(b, a)
	auto allTypes = {
		detail::LibRapidType::Scalar,
		detail::LibRapidType::Dual,
		detail::LibRapidType::Vector,
		detail::LibRapidType::ArrayContainer,
		detail::LibRapidType::ArrayFunction,
		detail::LibRapidType::GeneralArrayView
	};
	
	for (const auto& type1 : allTypes) {
		for (const auto& type2 : allTypes) {
			REQUIRE(detail::sameType(type1, type2) == detail::sameType(type2, type1));
		}
	}
}

TEST_CASE("sameType function - transitivity", "[core][traits][sameType]") {
	// Test transitive property: if sameType(a, b) && sameType(b, c) then sameType(a, c)
	// Since sameType only returns true for identical types, this is automatically satisfied
	auto allTypes = {
		detail::LibRapidType::Scalar,
		detail::LibRapidType::Dual,
		detail::LibRapidType::Vector,
		detail::LibRapidType::ArrayContainer,
		detail::LibRapidType::ArrayFunction,
		detail::LibRapidType::GeneralArrayView
	};
	
	for (const auto& type1 : allTypes) {
		for (const auto& type2 : allTypes) {
			for (const auto& type3 : allTypes) {
				if (detail::sameType(type1, type2) && detail::sameType(type2, type3)) {
					REQUIRE(detail::sameType(type1, type3));
				}
			}
		}
	}
}

TEST_CASE("LibRapidType compilation check", "[core][traits][compilation]") {
	// Test that the enum can be used in constexpr contexts
	constexpr auto scalarType = detail::LibRapidType::Scalar;
	constexpr auto dualType = detail::LibRapidType::Dual;
	constexpr bool areEqual = detail::sameType(scalarType, scalarType);
	constexpr bool areDifferent = detail::sameType(scalarType, dualType);
	
	STATIC_REQUIRE(areEqual == true);
	STATIC_REQUIRE(areDifferent == false);
	
	// Verify at runtime as well
	REQUIRE(areEqual);
	REQUIRE_FALSE(areDifferent);
}