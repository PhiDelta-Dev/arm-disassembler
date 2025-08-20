export module packed_struct;

import std;

import unsigned_integer;
import bit_manipulation;

export template <typename TypeParameter, std::size_t offset, std::size_t size>
	requires(size <= sizeof_bits<TypeParameter>)
struct PackedMember
{
	using Type = TypeParameter;
	constexpr static auto range{BitRange(BitIndex(offset), BitSize(size))};
};

template <typename Type>
concept PackedMemberType =
	requires {
		typename Type::Type;
		{ Type::range };
	} && std::is_empty_v<Type> &&
	std::same_as<std::remove_cvref_t<decltype(Type::range)>, BitRange> &&
	(Type::range.size().get() <= sizeof_bits<typename Type::Type>);

export template <StrongUnsigned UnderlyingType, PackedMemberType... Members> class PackedStruct
{
public:
	using Underlying = UnderlyingType;
	constexpr static auto member_count{sizeof...(Members)};

	/*
		Clang 20 doesn't like pack indexing here
	*/
	static_assert(member_count >= 1);
	template <std::size_t index>
	using Member = std::tuple_element_t<index, std::tuple<Members...>>; // Members...[index];

	constexpr explicit PackedStruct(const Underlying t_underlying = {}) : m_underlying(t_underlying)
	{
	}

	constexpr explicit PackedStruct(const Members::Type... t_members)
	{
		[&]<std::size_t... indices>([[maybe_unused]] std::index_sequence<indices...>)
		{
			(set<indices>(t_members...[indices]), ...);
		}(std::index_sequence_for<decltype(t_members)...>());
	}

	template <std::size_t index> constexpr auto set(const Member<index>::Type t_value) noexcept
	{
		using MemberType = Member<index>::Type;
		constexpr static auto member_range = Member<index>::range;

		constexpr static auto member_occupies_full_size{member_range.size().get() >=
														sizeof_bits<Underlying>};
		constexpr static auto ub_checked{member_occupies_full_size ? UbChecked::Checked
																   : UbChecked::Unchecked};

		using MemberUnsignedType = Unsigned<sizeof(MemberType)>::Underlying;
		const auto raw_bits{std::bit_cast<MemberUnsignedType>(t_value)};
		const Underlying extended_or_truncated_bits(raw_bits);
		set_bits<ub_checked>(m_underlying, extended_or_truncated_bits, member_range);
	}

	template <std::size_t index> [[nodiscard]] constexpr auto get() const noexcept
	{
		using MemberType = Member<index>::Type;
		constexpr static auto member_range = Member<index>::range;

		constexpr static auto member_occupies_full_size{member_range.size().get() >=
														sizeof_bits<Underlying>};
		constexpr static auto ub_checked{member_occupies_full_size ? UbChecked::Checked
																   : UbChecked::Unchecked};

		using MemberUnsignedType = Unsigned<sizeof(MemberType)>::Underlying;
		const auto raw_bits{get_bits<ub_checked>(m_underlying, member_range).get()};
		const auto extended_or_truncated_bits{static_cast<MemberUnsignedType>(raw_bits)};
		return std::bit_cast<MemberType>(extended_or_truncated_bits);
	}

	[[nodiscard]] constexpr auto to_underlying() const noexcept { return m_underlying; }

private:
	template <PackedMemberType Member>
	constexpr static auto check_member_size{
		(Member::range.begin().get() < sizeof_bits<Underlying>) &&
		(Member::range.end().get() <= sizeof_bits<Underlying>)};
	static_assert((check_member_size<Members> && ...));

	Underlying m_underlying{};
};

export template <typename Type>
concept Packed = requires(const Type::Underlying t_underlying) {
	typename Type::Underlying;
	{ Type::member_count };
	{ Type() };
	{ Type(t_underlying) };
};

// Overloads necessary for member access through structured bindings
export template <std::size_t index>
[[nodiscard]] constexpr auto get(const Packed auto t_packed) noexcept
{
	return t_packed.template get<index>();
}
export template <Packed Type>
struct std::tuple_size<Type> : std::integral_constant<std::size_t, Type::member_count>
{
};
export template <std::size_t index, Packed Type> struct std::tuple_element<index, Type>
{
	using type = typename Type::template Member<index>::Type;
};