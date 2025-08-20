export module unsigned_integer;

import std;

import strong_type;

template <std::size_t byte_width>
	requires(byte_width == sizeof(std::uint8_t) || byte_width == sizeof(std::uint16_t) ||
			 byte_width == sizeof(std::uint32_t) || byte_width == sizeof(std::uint64_t))
using RawUnsignedFromWidth = //
	std::conditional_t<
		byte_width == sizeof(std::uint8_t), std::uint8_t,										  //
		std::conditional_t<byte_width == sizeof(std::uint16_t), std::uint16_t,					  //
						   std::conditional_t<byte_width == sizeof(std::uint32_t), std::uint32_t, //
											  std::uint64_t>>>;

export template <typename Type>
concept RawUnsigned = std::same_as<Type, std::uint8_t> || std::same_as<Type, std::uint16_t> ||
					  std::same_as<Type, std::uint32_t> || std::same_as<Type, std::uint64_t>;

export template <typename Type>
concept StrongUnsigned = Strong<Type> && RawUnsigned<typename Type::Underlying>;

/*
	Define a strong unsigned integral type with the given width (in bytes)
*/
export template <std::size_t byte_width>
using Unsigned = StrongType<RawUnsignedFromWidth<byte_width>, struct UnsignedTag>;

/*
	Comparison of strong unsigned integral types
*/
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator==(const Type t_first, const Type t_second) noexcept
{
	return t_first.get() == t_second.get();
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator<=>(const Type t_first, const Type t_second) noexcept
{
	return t_first.get() <=> t_second.get();
}

/*
	Bitwise shift operations on strong unsigned integral types
*/
export using BitShiftAmount = StrongType<Unsigned<1>::Underlying, struct BitShiftAmountTag>;

// Left shift
export template <StrongUnsigned Type>
constexpr auto operator<<=(Type& t_value, const BitShiftAmount t_amount) noexcept -> Type&
{
	const auto amount{static_cast<int>(t_amount.get())};
	const auto result{static_cast<Type::Underlying>(t_value.get() << amount)};
	t_value.set(result);
	return t_value;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator<<(Type t_value, const BitShiftAmount t_amount) noexcept
{
	t_value <<= t_amount;
	return t_value;
}

// Right shift
export template <StrongUnsigned Type>
constexpr auto operator>>=(Type& t_value, const BitShiftAmount t_amount) noexcept -> Type&
{
	const auto amount{static_cast<int>(t_amount.get())};
	const auto result{static_cast<Type::Underlying>(t_value.get() >> amount)};
	t_value.set(result);
	return t_value;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator>>(Type t_value, const BitShiftAmount t_amount) noexcept
{
	t_value >>= t_amount;
	return t_value;
}

// Arithmetic right shift
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto arithmetic_shift_right(const Type t_value,
													const BitShiftAmount t_amount) noexcept
{
	using SignedType = std::make_signed_t<typename Type::Underlying>;
	const auto signed_value{static_cast<SignedType>(t_value.get())};
	const auto amount{static_cast<int>(t_amount.get())};
	const auto raw_result{static_cast<Type::Underlying>(signed_value >> amount)};
	return Type(raw_result);
}

// Right rotation
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto rotate_right(const Type t_value,
										  const BitShiftAmount t_amount) noexcept
{
	const auto amount{static_cast<int>(t_amount.get())};
	const auto raw_result{static_cast<Type::Underlying>(std::rotr(t_value.get(), amount))};
	return Type(raw_result);
}

/*
	Logical operations
*/

// Negation
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator~(const Type t_value) noexcept
{
	return Type(static_cast<Type::Underlying>(~t_value.get()));
}

// And
export template <StrongUnsigned Type>
constexpr auto operator&=(Type& t_first, const Type t_second) noexcept -> Type&
{
	const auto result{static_cast<Type::Underlying>(t_first.get() & t_second.get())};
	t_first.set(result);
	return t_first;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator&(Type t_first, const Type t_second) noexcept
{
	t_first &= t_second;
	return t_first;
}

// Or
export template <StrongUnsigned Type>
constexpr auto operator|=(Type& t_first, const Type t_second) noexcept -> Type&
{
	const auto result{static_cast<Type::Underlying>(t_first.get() | t_second.get())};
	t_first.set(result);
	return t_first;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator|(Type t_first, const Type t_second) noexcept
{
	t_first |= t_second;
	return t_first;
}

// Xor
export template <StrongUnsigned Type>
constexpr auto operator^=(Type& t_first, const Type t_second) noexcept -> Type&
{
	const auto result{static_cast<Type::Underlying>(t_first.get() ^ t_second.get())};
	t_first.set(result);
	return t_first;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator^(Type t_first, const Type t_second) noexcept
{
	t_first ^= t_second;
	return t_first;
}

/*
	Arithmetic operations
*/

// Addition
export template <StrongUnsigned Type>
constexpr auto operator+=(Type& t_first, const Type t_second) noexcept -> Type&
{
	const auto result{static_cast<Type::Underlying>(t_first.get() + t_second.get())};
	t_first.set(result);
	return t_first;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator+(Type t_first, const Type t_second) noexcept
{
	t_first += t_second;
	return t_first;
}

// Subtraction
export template <StrongUnsigned Type>
constexpr auto operator-=(Type& t_first, const Type t_second) noexcept -> Type&
{
	const auto result{static_cast<Type::Underlying>(t_first.get() - t_second.get())};
	t_first.set(result);
	return t_first;
}
export template <StrongUnsigned Type>
[[nodiscard]] constexpr auto operator-(Type t_first, const Type t_second) noexcept
{
	t_first -= t_second;
	return t_first;
}

/*
	Literals
*/
// NOLINTBEGIN(*-magic-numbers)
export [[nodiscard]] consteval auto operator""_u8(const unsigned long long t_value) noexcept
{
	return Unsigned<1>(t_value);
}
export [[nodiscard]] consteval auto operator""_u16(const unsigned long long t_value) noexcept
{
	return Unsigned<2>(t_value);
}
export [[nodiscard]] consteval auto operator""_u32(const unsigned long long t_value) noexcept
{
	return Unsigned<4>(t_value);
}
export [[nodiscard]] consteval auto operator""_u64(const unsigned long long t_value) noexcept
{
	return Unsigned<8>(t_value);
}
// NOLINTEND(*-magic-numbers)

export [[nodiscard]] consteval auto operator""_sh(const unsigned long long t_value) noexcept
{
	return BitShiftAmount(t_value);
}