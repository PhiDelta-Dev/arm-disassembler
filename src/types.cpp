export module types;

import std;

import strong_type;
import unsigned_integer;
import bit_manipulation;
import packed_struct;

namespace dzl
{
export using Byte = Unsigned<1>;
export using Halfword = Unsigned<2>;
export using Word = Unsigned<4>;

export enum struct Register : Unsigned<1>::Underlying{R0 = 0,	 R1 = 1,	R2 = 2,	  R3 = 3,	//
													  R4 = 4,	 R5 = 5,	R6 = 6,	  R7 = 7,	//
													  R8 = 8,	 R9 = 9,	R10 = 10, R11 = 11, //
													  R12 = 12,	 R13 = 13,	R14 = 14, R15 = 15, //
													  Cpsr = 16, Spsr = 17,						//
													  Sp = R13,	 Lr = R14,	Pc = R15};

export enum struct Condition
	: Unsigned<1>::Underlying{Eq, Ne, Cs, Cc, Mi, Pl, Vs, Vc, Hi, Ls, Ge, Lt, Gt, Le, Al};

export using Address = StrongType<Unsigned<4>::Underlying, struct AddressTag>;
export using AddressOffset = StrongType<Unsigned<4>::Underlying, struct AddressOffsetTag>;

export [[nodiscard]] constexpr auto operator-(const Address t_first,
											  const Address t_second) noexcept
{
	return AddressOffset(t_first.get() - t_second.get());
}

export [[nodiscard]] constexpr auto operator+(const Address t_address,
											  const AddressOffset t_offset) noexcept
{
	return Address(t_address.get() + t_offset.get());
}
export [[nodiscard]] constexpr auto operator-(const Address t_address,
											  const AddressOffset t_offset) noexcept
{
	return Address(t_address.get() - t_offset.get());
}

export [[nodiscard]] constexpr auto operator-(const AddressOffset t_offset) noexcept
{
	return AddressOffset(-t_offset.get());
}

export [[nodiscard]] constexpr auto is_word_aligned(const Address t_address) noexcept
{
	return (t_address.get() % 4U) == 0U;
}

export enum struct ShiftType : Unsigned<1>::Underlying{LogicalLeft, LogicalRight, ArithmeticRight,
													   RotateRight, RotateRightExtended};

} // namespace dzl

/*
	Literals
*/
export [[nodiscard]] consteval auto operator""_add(const unsigned long long t_value) noexcept
{
	return dzl::Address(t_value);
}
export [[nodiscard]] consteval auto operator""_off(const unsigned long long t_value) noexcept
{
	return dzl::AddressOffset(t_value);
}