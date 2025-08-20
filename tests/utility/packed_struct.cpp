#include <catch2/catch_test_macros.hpp>

import unsigned_integer;
import packed_struct;

// NOLINTBEGIN(*-magic-numbers)

TEST_CASE("", "[operator+(BitIndex, BitSize)]")
{
	using MyType = PackedStruct<Unsigned<8>, PackedMember<bool, 0, 1>,
								PackedMember<BitShiftAmount, 1, 8>, PackedMember<float, 16, 32>>;

	constexpr static MyType structure(false, 20_sh, 20.0F);
	const auto [first, second, third]{structure};
	REQUIRE(first == false);
	REQUIRE(second.get() == (20_sh).get());
	REQUIRE(third == 20.0F);
}

// NOLINTEND(*-magic-numbers)