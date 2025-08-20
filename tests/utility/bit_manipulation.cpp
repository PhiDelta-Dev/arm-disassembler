#include <catch2/catch_test_macros.hpp>

import unsigned_integer;
import bit_manipulation;

// NOLINTBEGIN(*-magic-numbers)

TEST_CASE("", "[operator+(BitIndex, BitSize)]") {}

TEST_CASE("", "[make_unshifted_bit_mask]")
{
	STATIC_REQUIRE(make_unshifted_bit_mask<2>(5_bs) == 0x001F_u16);
	STATIC_REQUIRE(make_unshifted_bit_mask<2>(0_bs) == 0x0000_u16);

	STATIC_REQUIRE(make_unshifted_bit_mask<1>(8_bs) == 0xFF_u8);
	STATIC_REQUIRE(make_unshifted_bit_mask<1>(9_bs) == 0xFF_u8);

	STATIC_REQUIRE(make_unshifted_bit_mask<2>(16_bs) == 0xFFFF_u16);
	STATIC_REQUIRE(make_unshifted_bit_mask<2>(17_bs) == 0xFFFF_u16);

	STATIC_REQUIRE(make_unshifted_bit_mask<4>(32_bs) == 0xFFFFFFFF_u32);
	STATIC_REQUIRE(make_unshifted_bit_mask<4>(33_bs) == 0xFFFFFFFF_u32);

	STATIC_REQUIRE(make_unshifted_bit_mask<8>(64_bs) == 0xFFFFFFFF'FFFFFFFF_u64);
	STATIC_REQUIRE(make_unshifted_bit_mask<8>(65_bs) == 0xFFFFFFFF'FFFFFFFF_u64);
}

TEST_CASE("", "[BitRange::end]")
{
	STATIC_REQUIRE(BitRange(4_bi, 8_bs).end() == 12_bi);
	STATIC_REQUIRE(BitRange(0_bi, 8_bs).end() == 8_bi);
	STATIC_REQUIRE(BitRange(8_bi, 0_bs).end() == 8_bi);
}

TEST_CASE("", "[BitRange::make_mask]")
{
	STATIC_REQUIRE(BitRange(4_bi, 8_bs).make_mask<2>() == 0x0FF0_u16);
	STATIC_REQUIRE(BitRange(0_bi, 8_bs).make_mask<2>() == 0x00FF_u16);

	STATIC_REQUIRE(BitRange(4_bi, 16_bs).make_mask<2>() == 0xFFF0_u16);
	STATIC_REQUIRE(BitRange(4_bi, 17_bs).make_mask<2>() == 0xFFF0_u16);

	STATIC_REQUIRE(BitRange(0_bi, 0_bs).make_mask<2>() == 0x0000_u16);
	STATIC_REQUIRE(BitRange(8_bi, 0_bs).make_mask<2>() == 0x0000_u16);

	STATIC_REQUIRE(BitRange(16_bi, 8_bs).make_mask<2>() == 0x0000_u16);
	STATIC_REQUIRE(BitRange(17_bi, 8_bs).make_mask<2>() == 0x0000_u16);
}

TEST_CASE("", "[get_bits]")
{
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 8_bs}) == 0x0029_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {4_bi, 8_bs}) == 0x0072_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {8_bi, 8_bs}) == 0x0017_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 16_bs}) == 0x1729_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 17_bs}) == 0x1729_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {16_bi, 8_bs}) == 0x0000_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {17_bi, 8_bs}) == 0x0000_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {8_bi, 0_bs}) == 0x0000_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 0_bs}) == 0x0000_u16);
}

TEST_CASE("", "[get_bits]")
{
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 8_bs}) == 0x0029_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {4_bi, 8_bs}) == 0x0072_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {8_bi, 8_bs}) == 0x0017_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 16_bs}) == 0x1729_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 17_bs}) == 0x1729_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {16_bi, 8_bs}) == 0x0000_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {17_bi, 8_bs}) == 0x0000_u16);

	STATIC_REQUIRE(get_bits(0x1729_u16, {8_bi, 0_bs}) == 0x0000_u16);
	STATIC_REQUIRE(get_bits(0x1729_u16, {0_bi, 0_bs}) == 0x0000_u16);
}

TEST_CASE("", "[get_bit]")
{
	STATIC_REQUIRE(get_bit(0x1729_u16, 0_bi));
	STATIC_REQUIRE(!get_bit(0x1729_u16, 4_bi));
	STATIC_REQUIRE(!get_bit(0x1729_u16, 16_bi));
	STATIC_REQUIRE(!get_bit(0x1729_u16, 17_bi));
}

TEST_CASE("", "[set_bits]")
{
	{
		auto bits{0x1729_u16};
		set_bits(bits, 0xAB_u16, {4_bi, 8_bs});
		REQUIRE(bits == 0x1AB9_u16);
	}
	{
		auto bits{0x1729_u16};
		set_bits(bits, 0xAB_u16, {0_bi, 16_bs});
		REQUIRE(bits == 0x00AB_u16);
	}
	{
		auto bits{0x1729_u16};
		set_bits(bits, 0xAB_u16, {0_bi, 0_bs});
		REQUIRE(bits == 0x1729_u16);
	}
	{
		auto bits{0x1729_u16};
		set_bits(bits, 0xAB_u16, {16_bi, 0_bs});
		REQUIRE(bits == 0x1729_u16);
	}
	{
		auto bits{0x1729_u16};
		set_bits(bits, 0xAB_u16, {16_bi, 16_bs});
		REQUIRE(bits == 0x1729_u16);
	}
}

TEST_CASE("", "[sign_extend]")
{
	STATIC_REQUIRE(sign_extend(0x1729_u16, 12_bs) == 0x0729_u16);
	STATIC_REQUIRE(sign_extend(0x1729_u16, 13_bs) == 0xF729_u16);
	STATIC_REQUIRE(sign_extend(0x1729_u16, 8_bs) == 0x0029_u16);

	STATIC_REQUIRE(sign_extend(0x1729_u16, 16_bs) == 0x1729_u16);
	STATIC_REQUIRE(sign_extend(0x1729_u16, 17_bs) == 0x1729_u16);

	STATIC_REQUIRE(sign_extend(0x1729_u16, 0_bs) == 0x0000_u16);
}

// NOLINTEND(*-magic-numbers)