export module arm_instruction;

import std;

import strong_type;
import unsigned_integer;
import bit_manipulation;
import packed_struct;

import types;
import shift_operand;
import instruction;

// NOLINTBEGIN(*-magic-numbers)

namespace dzl::fmt::arm
{
struct FormatMask
{
	Word checked_bits, required_bits;
};
constexpr std::array format_masks{
	// BranchAndExchange
	FormatMask{		   .checked_bits = 0b0000'111111111111111111111111'0000_u32,
			   .required_bits = 0b0000'000100101111111111110001'0000_u32			},
	// SingleDataSwap
	FormatMask{	   .checked_bits = 0b0000'11111'0'11'0000'0000'11111111'0000_u32,
			   .required_bits = 0b0000'00010'0'00'0000'0000'00001001'0000_u32	 },
	// Multiply
	FormatMask{	   .checked_bits = 0b0000'111111'00'0000'0000'0000'1111'0000_u32,
			   .required_bits = 0b0000'000000'00'0000'0000'0000'1001'0000_u32	 },
	// HalfwordDataTransferRegisterOffset
	FormatMask{ .checked_bits = 0b0000'111'00'1'00'0000'0000'11111'00'1'0000_u32,
			   .required_bits = 0b0000'000'00'0'00'0000'0000'00001'00'1'0000_u32 },
	// MultiplyLong
	FormatMask{	   .checked_bits = 0b0000'11111'000'0000'0000'0000'1111'0000_u32,
			   .required_bits = 0b0000'00001'000'0000'0000'0000'1001'0000_u32	 },
	// HalfwordDataTransferImmediateOffset
	FormatMask{.checked_bits = 0b0000'111'00'1'00'0000'0000'0000'1'00'1'0000_u32,
			   .required_bits = 0b0000'000'00'1'00'0000'0000'0000'1'00'1'0000_u32},
	// CoprocessorDataOperation
	FormatMask{   .checked_bits = 0b0000'1111'0000'0000'0000'0000'000'1'0000_u32,
			   .required_bits = 0b0000'1110'0000'0000'0000'0000'000'0'0000_u32	  },
	// CoprocessorRegisterTransfer
	FormatMask{   .checked_bits = 0b0000'1111'0000'0000'0000'0000'000'1'0000_u32,
			   .required_bits = 0b0000'1110'0000'0000'0000'0000'000'1'0000_u32	  },
	// Undefined
	FormatMask{	   .checked_bits = 0b0000'111'00000'0000'0000'0000000'1'0000_u32,
			   .required_bits = 0b0000'011'00000'0000'0000'0000000'1'0000_u32	 },
	// SoftwareInterrupt
	FormatMask{		   .checked_bits = 0b0000'1111'000000000000000000000000_u32,
			   .required_bits = 0b0000'1111'000000000000000000000000_u32			},
	// BlockDataTransfer
	FormatMask{	   .checked_bits = 0b0000'111'00000'0000'0000000000000000_u32,
			   .required_bits = 0b0000'100'00000'0000'0000000000000000_u32		  },
	// Branch
	FormatMask{		   .checked_bits = 0b0000'111'0'000000000000000000000000_u32,
			   .required_bits = 0b0000'101'0'000000000000000000000000_u32		 },
	// CoprocessorDataTransfer
	FormatMask{	   .checked_bits = 0b0000'111'00000'0000'0000'0000'00000000_u32,
			   .required_bits = 0b0000'110'00000'0000'0000'0000'00000000_u32		},
	// DataProcessingPsrTransfer
	FormatMask{	   .checked_bits = 0b0000'11'0'0000'0'0000'0000'000000000000_u32,
			   .required_bits = 0b0000'00'0'0000'0'0000'0000'000000000000_u32	 },
	// SingleDataTransfer
	FormatMask{	   .checked_bits = 0b0000'11'000000'0000'0000'000000000000_u32,
			   .required_bits = 0b0000'01'000000'0000'0000'000000000000_u32	   }
};

// Require that the masks are sorted
static_assert(std::ranges::is_sorted(format_masks,
									 [](const FormatMask t_first, const FormatMask t_second)
									 {
										 return std::popcount(t_first.checked_bits.get()) >
												std::popcount(t_second.checked_bits.get());
									 }));

enum struct Format : Unsigned<1>::Underlying{BranchAndExchange,
											 SingleDataSwap,
											 Multiply,
											 HalfwordDataTransferRegsiterOffset,
											 MultiplyLong,
											 HalfwordDataTransferImmedaiteOffset,
											 CoprocessorDataOperation,
											 CoprocessorRegisterTransfer,
											 Undefined,
											 SoftwareInterrupt,
											 BlockDataTransfer,
											 Branch,
											 CoprocessorDataTransfer,
											 DataProcessingPsrTransfer,
											 SingleDataTransfer};

[[nodiscard]] constexpr auto decode_branch_and_exchange(const Word t_raw_instruction) noexcept
{
	using BranchAndExchange = PackedStruct<Word,						  //
										   PackedMember<Register, 0, 4>,  // Destination
										   PackedMember<Condition, 28, 4> // Condition
										   >;
	const auto [destination, condition]{BranchAndExchange(t_raw_instruction)};

	const ins::BranchAndExchange instruction(ins::Operation::BranchAndExchange, condition,
											 destination);
	return ins::Instruction(instruction);
}

using BranchRawOffset = Unsigned<4>;
[[nodiscard]] constexpr auto decode_branch_offset(const BranchRawOffset t_raw_offset) noexcept
{
	const auto unshifted(t_raw_offset << 2_sh);
	const auto sign_exteded{sign_extend<UbChecked::Unchecked>(unshifted, 26_bs)};
	return AddressOffset(sign_exteded.get());
}

[[nodiscard]] constexpr auto decode_branch(const Word t_raw_instruction) noexcept
{
	using Branch = PackedStruct<Word,								  //
								PackedMember<BranchRawOffset, 0, 24>, // Raw offset
								PackedMember<bool, 24, 1>,			  // Link
								PackedMember<Condition, 28, 4>		  // Condition
								>;
	const auto [raw_offset, link, condition]{Branch(t_raw_instruction)};

	const auto offset{decode_branch_offset(raw_offset)};

	const ins::Branch instruction(ins::Operation::Branch, condition, link, offset);
	return ins::Instruction(instruction);
}

// Shift operand types
using RawRotateAmount = Unsigned<1>;
using RawRotatedImmediateOperand =
	PackedStruct<Unsigned<2>,						 //
				 PackedMember<Byte, 0, 8>,			 // Rotated value
				 PackedMember<RawRotateAmount, 8, 4> // Raw rotate amount
				 >;

using RawShiftedRegisterOperand =
	PackedStruct<Unsigned<2>,				   //
				 PackedMember<Register, 0, 4>, // Source
				 PackedMember<bool, 4, 1>,	   // Shifted by register-specified amount
				 PackedMember<ShiftType, 5, 2> // Shift type
				 >;
using RawRegisterShiftedRegisterOperand = PackedStruct<Unsigned<2>,					//
													   PackedMember<Register, 8, 4> // Amount
													   >;
using RawImmediateShiftedRegisterOperand = PackedStruct<Unsigned<2>,					   //
														PackedMember<BitShiftAmount, 7, 5> // Amount
														>;

export [[nodiscard]] constexpr auto
decode_data_processing_second_operand(const Word t_raw_instruction) noexcept
{
	using DataProcessingPsrTransferRawSecond =
		PackedStruct<Word,					   //
					 PackedMember<bool, 25, 1> // Is rotated immediate
					 >;
	const auto [is_immediate]{DataProcessingPsrTransferRawSecond(t_raw_instruction)};
	const Unsigned<2> second(t_raw_instruction.get());

	// Rotated immediate
	if (is_immediate)
	{
		const auto [rotated_value, raw_rotate_amount]{RawRotatedImmediateOperand(second)};

		const BitShiftAmount rotate_amount((raw_rotate_amount << 1_sh).get());

		return ShiftOperand(rotated_value, rotate_amount);
	}

	const auto [source, register_amount, shift_type]{RawShiftedRegisterOperand(second)};

	// Register shifted by register-specified amount
	if (register_amount)
	{
		const auto [amount]{RawRegisterShiftedRegisterOperand(second)};

		return ShiftOperand(source, shift_type, amount);
	}

	// Register shifted by immediate amount
	const auto [amount]{RawImmediateShiftedRegisterOperand(second)};

	// Special encodings
	if (amount == 0_sh)
	{
		if (shift_type == ShiftType::LogicalRight || shift_type == ShiftType::ArithmeticRight)
		{
			return ShiftOperand(source, shift_type, 32_sh);
		}

		if (shift_type == ShiftType::RotateRight)
		{
			return ShiftOperand(source, ShiftType::RotateRightExtended, 0_sh);
		}
	}

	return ShiftOperand(source, shift_type, amount);
}

[[nodiscard]] constexpr auto decode_data_processing_psr_transfer(const Word t_raw_instruction)
{
	using DataProcessingPsrTransfer =
		PackedStruct<Word,											 //
					 PackedMember<Register, 12, 4>,					 // Destination
					 PackedMember<Register, 16, 4>,					 // First
					 PackedMember<bool, 20, 1>,						 // Set condition codes
					 PackedMember<ins::DataProcessingOpCode, 21, 4>, // Op code
					 PackedMember<Condition, 28, 4>					 // Condition
					 >;

	const auto [destination, first, set_condition_codes, op_code,
				condition]{DataProcessingPsrTransfer(t_raw_instruction)};

	if (!set_condition_codes &&
		(op_code == ins::DataProcessingOpCode::Tst || op_code == ins::DataProcessingOpCode::Cmp))
	{
		// Move from Psr
		const auto source_psr{op_code == ins::DataProcessingOpCode::Cmp ? Register::Spsr
																		: Register::Cpsr};

		const ins::MoveFromPsr instruction(ins::Operation::MoveFromPsr, condition, destination,
										   source_psr);
		return ins::Instruction(instruction);
	}

	const auto second{decode_data_processing_second_operand(t_raw_instruction)};

	if (!set_condition_codes &&
		(op_code == ins::DataProcessingOpCode::Teq || op_code == ins::DataProcessingOpCode::Cmn))
	{
		// Move to Psr
		const auto destination_psr{op_code == ins::DataProcessingOpCode::Cmn ? Register::Spsr
																			 : Register::Cpsr};
		const auto flags_only{first == Register::R8};

		const ins::MoveToPsr instruction(ins::Operation::MoveFromPsr, condition, destination_psr,
										 second, flags_only);
		return ins::Instruction(instruction);
	}

	const ins::DataProcessing instruction(ins::Operation::DataProcessing, condition, op_code,
										  set_condition_codes, destination, first, second);
	return ins::Instruction(instruction);
}

[[nodiscard]] constexpr auto get_format(const Word t_raw_instruction)
{
	for (std::size_t i_mask{}; i_mask < format_masks.size(); ++i_mask)
	{
		const auto mask{format_masks[i_mask]};
		if ((t_raw_instruction & mask.checked_bits) == mask.required_bits)
		{
			return static_cast<Format>(i_mask);
		}
	}

	throw std::runtime_error("Invalid format");
}

export [[nodiscard]] constexpr auto decode(const Word t_raw_instruction)
{
	switch (get_format(t_raw_instruction))
	{
	case Format::BranchAndExchange:
		return decode_branch_and_exchange(t_raw_instruction);
	case Format::SingleDataSwap:
	case Format::Multiply:
	case Format::HalfwordDataTransferRegsiterOffset:
	case Format::MultiplyLong:
	case Format::HalfwordDataTransferImmedaiteOffset:
	case Format::CoprocessorDataOperation:
	case Format::CoprocessorRegisterTransfer:
	case Format::Undefined:
	case Format::SoftwareInterrupt:
	case Format::BlockDataTransfer:
		throw std::runtime_error("Format not implemented");
	case Format::Branch:
		return decode_branch(t_raw_instruction);
	case Format::CoprocessorDataTransfer:
		throw std::runtime_error("Format not implemented");
	case Format::DataProcessingPsrTransfer:
		return decode_data_processing_psr_transfer(t_raw_instruction);
	case Format::SingleDataTransfer:
		throw std::runtime_error("Format not implemented");
	default:
		std::unreachable();
	}
}

} // namespace dzl::fmt::arm

// NOLINTEND(*-magic-numbers)