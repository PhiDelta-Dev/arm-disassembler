#include <catch2/catch_test_macros.hpp>

import strong_type;

// NOLINTBEGIN(*-magic-numbers)

TEST_CASE("The StrongType template satisfies the constraints of the Strong concept", "[StrongType]")
{
	STATIC_REQUIRE_FALSE(Strong<int>);
	STATIC_REQUIRE(Strong<StrongType<int, struct Name>>);

	STATIC_REQUIRE_FALSE(Strong<double>);
	STATIC_REQUIRE(Strong<StrongType<double, struct Name>>);
}

TEST_CASE("Implicit conversions are performed when constructing a StrongType",
		  "[StrongType::StrongType]")
{
	{
		constexpr static StrongType<unsigned, struct StrongName> strong(-1);
		STATIC_REQUIRE(strong.get() == ~0U);
	}
	{
		constexpr static StrongType<float, struct StrongName> strong(2);
		STATIC_REQUIRE(strong.get() == 2.0F);
	}
}


// NOLINTEND(*-magic-numbers)