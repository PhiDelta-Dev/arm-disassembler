#include <catch2/catch_test_macros.hpp>

import strong_type;
import unsigned_integer;

// NOLINTBEGIN(*-magic-numbers)

TEST_CASE("The StrongUnsigned concept behaves as intended", "[StrongUnsigned]")
{
	STATIC_REQUIRE(StrongUnsigned<StrongType<std::uint8_t, struct Tag>>);
	STATIC_REQUIRE(StrongUnsigned<StrongType<std::uint16_t, struct Tag>>);
	STATIC_REQUIRE(StrongUnsigned<StrongType<std::uint32_t, struct Tag>>);
	STATIC_REQUIRE(StrongUnsigned<StrongType<std::uint64_t, struct Tag>>);
}

TEST_CASE("The Unsigned strong types behave as intended ", "[Unsigned]")
{
	STATIC_REQUIRE(std::same_as<Unsigned<1>::Underlying, std::uint8_t>);
	STATIC_REQUIRE(std::same_as<Unsigned<2>::Underlying, std::uint16_t>);
	STATIC_REQUIRE(std::same_as<Unsigned<4>::Underlying, std::uint32_t>);
	STATIC_REQUIRE(std::same_as<Unsigned<8>::Underlying, std::uint64_t>);
}

TEST_CASE("The comparison operators behave as intended", "[operator==, operator<=>]")
{
	STATIC_REQUIRE(0x1729_u16 == 0x1729_u16);
	STATIC_REQUIRE(0x1729_u16 <= 0x1729_u16);
	STATIC_REQUIRE(0x1729_u16 >= 0x1729_u16);

	STATIC_REQUIRE(0x1729_u16 < 0x3141_u16);
	STATIC_REQUIRE(0x3141_u16 > 0x1729_u16);

	STATIC_REQUIRE(0_u16 < 0xFFFF_u16);
	STATIC_REQUIRE(0xFFFF_u16 > 0_u16);
}

TEST_CASE("The bit shift operators behave as intended",
		  "[operator<<, operator>>, arithmetic_shift_right, rotate_right]")
{
	STATIC_REQUIRE((0x1729_u16 << 4_sh) == 0x7290_u16);
	STATIC_REQUIRE((0x1729_u16 << 0_sh) == 0x1729_u16);
	STATIC_REQUIRE((0x1729_u16 << 16_sh) == 0x0000_u16);

	STATIC_REQUIRE((0x1729_u16 >> 4_sh) == 0x0172_u16);
	STATIC_REQUIRE((0x1729_u16 >> 0_sh) == 0x1729_u16);

	STATIC_REQUIRE(arithmetic_shift_right(0x1729_u16, 4_sh) == 0x0172_u16);
	STATIC_REQUIRE(arithmetic_shift_right(0x1729_u16, 0_sh) == 0x1729_u16);
	STATIC_REQUIRE(arithmetic_shift_right(0x8000_u16, 4_sh) == 0xF800_u16);
	STATIC_REQUIRE(arithmetic_shift_right(0x8000_u16, 0_sh) == 0x8000_u16);

	STATIC_REQUIRE(rotate_right(0x1729_u16, 4_sh) == 0x9172_u16);
	STATIC_REQUIRE(rotate_right(0x1729_u16, 0_sh) == 0x1729_u16);
}

TEST_CASE("The logical operators behave as intended",
		  "[operator~, operator&, operator|, operator^]")
{
	STATIC_REQUIRE((~0x0FF0_u16) == 0xF00F_u16);
	STATIC_REQUIRE((~0x0000_u16) == 0xFFFF_u16);
	STATIC_REQUIRE((~0xFFFF_u16) == 0x0000_u16);

	STATIC_REQUIRE((0x1729_u16 & 0x0FF0_u16) == 0x0720_u16);
	STATIC_REQUIRE((0x1729_u16 & 0x0000_u16) == 0x0000_u16);
	STATIC_REQUIRE((0x1729_u16 & 0xFFFF_u16) == 0x1729_u16);

	STATIC_REQUIRE((0x1009_u16 | 0x0720_u16) == 0x1729_u16);
	STATIC_REQUIRE((0x1729_u16 | 0x0000_u16) == 0x1729_u16);
	STATIC_REQUIRE((0x1729_u16 | 0xFFFF_u16) == 0xFFFF_u16);

	STATIC_REQUIRE((0x1009_u16 ^ 0x0720_u16) == 0x1729_u16);
	STATIC_REQUIRE((0x1729_u16 ^ 0x0000_u16) == 0x1729_u16);
	STATIC_REQUIRE((0xF00F_u16 ^ 0xFFFF_u16) == 0x0FF0_u16);
}

TEST_CASE("The arithmetic operators behave as intended",
		  "[operator+, operator-, operator*]")
{
	STATIC_REQUIRE((0x1700_u16 + 0x0029_u16) == 0x1729_u16);
	STATIC_REQUIRE((0x1729_u16 + 0x0000_u16) == 0x1729_u16);
	STATIC_REQUIRE((0xFFFF_u16 + 0x0001_u16) == 0x0000_u16);
	STATIC_REQUIRE((0xFFFF_u16 + 0xFFFF_u16) == 0xFFFE_u16);

	STATIC_REQUIRE((0x1729_u16 - 0x1700_u16) == 0x0029_u16);
	STATIC_REQUIRE((0x1729_u16 - 0x1729_u16) == 0x0000_u16);
	STATIC_REQUIRE((0x0000_u16 - 0x0001_u16) == 0xFFFF_u16);
	STATIC_REQUIRE((0xFFFE_u16 - 0xFFFF_u16) == 0xFFFF_u16);
}

// NOLINTEND(*-magic-numbers)