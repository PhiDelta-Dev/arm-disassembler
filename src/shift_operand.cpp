export module shift_operand;

import std;

import strong_type;
import unsigned_integer;
import bit_manipulation;
import packed_struct;

import types;

namespace dzl
{
export enum struct ShiftOperandType
	: Unsigned<1>::Underlying{Immediate, RotatedImmediate, ImmediateShiftedRegister,
							  RegisterShiftedRegister};

using ShiftOperandBits = Unsigned<2>;

//NOLINTBEGIN(*-magic-numbers)

export using ImmediateValue = StrongType<Unsigned<2>::Underlying, struct ImmediateValueTag>;
export using ImmediateOperand = PackedStruct<ShiftOperandBits,					   //
											 PackedMember<ShiftOperandType, 0, 2>, // Type
																				   //
											 PackedMember<ImmediateValue, 4, 12>   // Source
											 >;

export using RotatedImmediateOperand = PackedStruct<ShiftOperandBits,					  //
													PackedMember<ShiftOperandType, 0, 2>, // Type
																						  //
													PackedMember<Byte, 8, 8>,			  // Source
													PackedMember<BitShiftAmount, 2, 5>	  // Amount
													>;
export using ImmediateShiftedRegisterOperand =
	PackedStruct<ShiftOperandBits,					   //
				 PackedMember<ShiftOperandType, 0, 2>, // Type
													   //
				 PackedMember<Register, 2, 4>,		   // Source
				 PackedMember<ShiftType, 6, 3>,		   // Type
				 PackedMember<BitShiftAmount, 10, 6>   // Amount
				 >;

export using RegisterShiftedRegisterOperand =
	PackedStruct<ShiftOperandBits,					   //
				 PackedMember<ShiftOperandType, 0, 2>, // Type
													   //
				 PackedMember<Register, 4, 4>,		   // Source
				 PackedMember<ShiftType, 8, 2>,		   // Type
				 PackedMember<Register, 12, 4>		   // Amount
				 >;

//NOLINTEND(*-magic-numbers)

template <typename Type>
concept ShiftOperandTypeConcept =
	std::same_as<Type, ImmediateOperand> || std::same_as<Type, RotatedImmediateOperand> ||
	std::same_as<Type, ImmediateShiftedRegisterOperand> ||
	std::same_as<Type, RegisterShiftedRegisterOperand>;

export class ShiftOperand
{
public:
	constexpr explicit ShiftOperand(const ShiftOperandBits t_underlying)
		: m_underlying(t_underlying)
	{
	}

	constexpr explicit ShiftOperand(const ShiftOperandTypeConcept auto t_operand)
		: m_underlying(t_operand.to_underlying())
	{
	}

	constexpr explicit ShiftOperand(const ImmediateValue t_value)
		: m_underlying(ImmediateOperand(ShiftOperandType::Immediate, t_value).to_underlying())
	{
	}

	constexpr ShiftOperand(const Byte t_source, const BitShiftAmount t_rotate_amount)
		: m_underlying(
			  RotatedImmediateOperand(ShiftOperandType::RotatedImmediate, t_source, t_rotate_amount)
				  .to_underlying())
	{
	}

	constexpr ShiftOperand(const Register t_source, const ShiftType t_type,
						   const BitShiftAmount t_amount)
		: m_underlying(ImmediateShiftedRegisterOperand(ShiftOperandType::ImmediateShiftedRegister,
													   t_source, t_type, t_amount)
						   .to_underlying())
	{
	}

	constexpr ShiftOperand(const Register t_source, const ShiftType t_type, const Register t_amount)
		: m_underlying(RegisterShiftedRegisterOperand(ShiftOperandType::RegisterShiftedRegister,
													  t_source, t_type, t_amount)
						   .to_underlying())
	{
	}

	[[nodiscard]] constexpr auto get_type() const noexcept
	{
		const auto type_bits{get_bits(m_underlying, {0_bi, 2_bs})};
		return ShiftOperandType(type_bits.get());
	}

	template <ShiftOperandTypeConcept OperandType> [[nodiscard]] constexpr auto get() const noexcept
	{
		return OperandType(m_underlying);
	}

private:
	ShiftOperandBits m_underlying;
};

}; // namespace dzl