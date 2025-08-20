export module bit_manipulation;

import std;

import strong_type;
import unsigned_integer;

export using BitIndex = StrongType<std::size_t, struct BitIndexTag>;
export using BitSize = StrongType<std::size_t, struct BitSizeTag>;
export [[nodiscard]] constexpr auto operator+(const BitIndex t_offset, const BitSize t_size) noexcept
{
	return BitIndex(static_cast<BitIndex::Underlying>(t_offset.get() + t_size.get()));
}
export [[nodiscard]] consteval auto operator""_bi(const unsigned long long t_value) noexcept
{
	return BitIndex(t_value);
}
export [[nodiscard]] consteval auto operator""_bs(const unsigned long long t_value) noexcept
{
	return BitSize(t_value);
}

export enum struct UbChecked : bool { Unchecked, Checked };

export template <typename Type> constexpr std::size_t sizeof_bits{8UZ * sizeof(Type)};

// If unchecked, t_width must be less than 8 * byte_width
export template <std::size_t byte_width, UbChecked is_checked = UbChecked::Checked>
[[nodiscard]] constexpr auto make_unshifted_bit_mask(const BitSize t_width) noexcept
{
	using ResultType = Unsigned<byte_width>;

	if constexpr (is_checked == UbChecked::Checked)
	{
		if (t_width.get() >= sizeof_bits<ResultType>)
		{
			// Defined behaviour: set all bits
			constexpr static auto all_set{~ResultType(0)};
			return all_set;
		}
	}

	constexpr static ResultType one{1};
	const BitShiftAmount amount(t_width.get());
	return (one << amount) - one;
}

export class BitRange
{
public:
	constexpr BitRange(const BitIndex t_begin, const BitSize t_size)
		: m_begin(t_begin), m_size(t_size)
	{
	}

	// If unchecked, m_begin and m_size must both be less than 8 * byte_width
	template <std::size_t byte_width, UbChecked is_checked = UbChecked::Checked>
	[[nodiscard]] constexpr auto make_mask() const noexcept
	{
		if constexpr (is_checked == UbChecked::Checked)
		{
			using ResultType = Unsigned<byte_width>;
			if (m_begin.get() >= sizeof_bits<ResultType>)
			{
				// Defined behaviour: return zero
				constexpr static ResultType zero{0};
				return zero;
			}
		}

		const auto unshifted{make_unshifted_bit_mask<byte_width, is_checked>(m_size)};
		const BitShiftAmount amount(m_begin.get());
		return unshifted << amount;
	}

	[[nodiscard]] constexpr auto begin() const noexcept { return m_begin; }
	[[nodiscard]] constexpr auto size() const noexcept { return m_size; }
	[[nodiscard]] constexpr auto end() const noexcept { return m_begin + m_size; }

private:
	BitIndex m_begin;
	BitSize m_size;
};

// If unchecked, t_range.m_begin and t_range.m_size must both be less than sizeof_bits<Type>
export template <UbChecked is_checked = UbChecked::Checked>
[[nodiscard]] constexpr auto get_bits(const StrongUnsigned auto t_value,
									  const BitRange t_range) noexcept
{
	if constexpr (is_checked == UbChecked::Checked)
	{
		using ValueType = std::remove_cvref_t<decltype(t_value)>;

		if (t_range.begin().get() >= sizeof_bits<ValueType>)
		{
			// Defined behaviour: return zero
			constexpr static ValueType zero{0};
			return zero;
		}
	}

	constexpr static auto byte_width{sizeof(t_value)};
	const auto mask{make_unshifted_bit_mask<byte_width, is_checked>(t_range.size())};

	const BitShiftAmount shift_amount(t_range.begin().get());
	const auto shifted{t_value >> shift_amount};
	return shifted & mask;
}

// If unchecked, t_index must be less than sizeof_bits<Type>
export template <UbChecked is_checked = UbChecked::Checked>
[[nodiscard]] constexpr auto get_bit(const StrongUnsigned auto t_value,
									 const BitIndex t_index) noexcept
{
	using ValueType = std::remove_cvref_t<decltype(t_value)>;

	if constexpr (is_checked == UbChecked::Checked)
	{
		if (t_index.get() >= sizeof_bits<ValueType>)
		{
			// Defined behaviour: return false
			return false;
		}
	}

	const BitShiftAmount shift_amount(t_index.get());
	const auto shifted{t_value >> shift_amount};

	constexpr static ValueType one{1};
	constexpr static ValueType zero{0};
	return (shifted & one) != zero;
}

// If unchecked, t_range.m_begin and t_range.m_size must both be less than sizeof_bits<Type>
export template <UbChecked is_checked = UbChecked::Checked>
[[nodiscard]] constexpr auto set_bits(StrongUnsigned auto& t_destination,
									  const std::remove_cvref_t<decltype(t_destination)> t_source,
									  const BitRange t_range) noexcept
{
	using ValueType = std::remove_cvref_t<decltype(t_destination)>;

	const auto destination_mask{~(t_range.make_mask<sizeof(ValueType), is_checked>())};
	const auto masked_destination{t_destination & destination_mask};

	const auto source_mask{make_unshifted_bit_mask<sizeof(ValueType), is_checked>(t_range.size())};
	const auto masked_source{t_source & source_mask};

	const BitShiftAmount shift_amount(t_range.begin().get());
	t_destination = masked_destination | (masked_source << shift_amount);
}

// If unchecked, t_width must be greater than zero and less than sizeof_bits<Type>
export template <UbChecked is_checked = UbChecked::Checked>
[[nodiscard]] constexpr auto sign_extend(const StrongUnsigned auto t_value, const BitSize t_width) noexcept
{
	using ValueType = std::remove_cvref_t<decltype(t_value)>;

	if constexpr (is_checked == UbChecked::Checked)
	{
		if (t_width == 0_bs)
		{
			// Defined behaviour: return zero
			constexpr static ValueType zero{0};
			return zero;
		}

		if (t_width.get() >= sizeof_bits<ValueType>)
		{
			// Defined behaviour: return the unchanged value
			return t_value;
		}
	}

	constexpr static auto byte_width{sizeof(t_value)};
	const auto mask{make_unshifted_bit_mask<byte_width, is_checked>(t_width)};
	const auto masked{t_value & mask};

	constexpr static ValueType one{1};
	const BitShiftAmount shift_amount(t_width.get() - 1UZ);
	const auto most_significant_bit{one << shift_amount};
	return (masked ^ most_significant_bit) - most_significant_bit;
}