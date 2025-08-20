export module instruction;

import std;

import unsigned_integer;
import bit_manipulation;
import packed_struct;

import types;
import shift_operand;

namespace dzl::ins
{
export enum struct Operation : Unsigned<1>::Underlying{BranchAndExchange,
													   Branch,

													   DataProcessing,
													   MoveFromPsr,
													   MoveToPsr,

													   Multiply,

													   Load,
													   Store,
													   LoadMultiple,
													   StoreMultiple,
													   Swap,

													   SoftwareInterrupt,

													   CoprocessorDataOperation,
													   CoprocessorLoad,
													   CoprocessorStore,
													   LoadCoprocessorRegister,
													   StoreCoprocessorRegsiter,

													   Undefined};

// NOLINTBEGIN(*-magic-numbers)
using InstructionBits = Unsigned<8>;

export using BranchAndExchange = PackedStruct<InstructionBits,				 //
											  PackedMember<Operation, 0, 8>, // Operation
											  PackedMember<Condition, 8, 4>, // Condition
																			 //
											  PackedMember<Register, 32, 4>	 // Destination
											  >;

export using Branch = PackedStruct<InstructionBits,					   //
								   PackedMember<Operation, 0, 8>,	   // Operation
								   PackedMember<Condition, 8, 4>,	   // Condition
																	   //
								   PackedMember<bool, 16, 1>,		   // Link
								   PackedMember<AddressOffset, 32, 32> // Offset
								   >;

export enum struct DataProcessingOpCode
	: Unsigned<1>::Underlying{And, Eor, Sub, Rsb, Add, Adc, Sbc, Rsc,
							  Tst, Teq, Cmp, Cmn, Orr, Mov, Bic, Mvn};

export using DataProcessing = PackedStruct<InstructionBits,							  //
										   PackedMember<Operation, 0, 8>,			  // Operation
										   PackedMember<Condition, 8, 4>,			  // Condition
																					  //
										   PackedMember<DataProcessingOpCode, 16, 4>, // OpCode
										   PackedMember<bool, 20, 1>,		  // SetConditionCodes
										   PackedMember<Register, 24, 4>,	  // Destination
										   PackedMember<Register, 28, 4>,	  // First
										   PackedMember<ShiftOperand, 32, 16> // Second
										   >;

export using MoveFromPsr = PackedStruct<InstructionBits,			   //
										PackedMember<Operation, 0, 8>, // Operation
										PackedMember<Condition, 8, 4>, // Condition
																	   //
										PackedMember<Register, 16, 5>, // Destination
										PackedMember<Register, 24, 5>  // Source
										>;

export using MoveToPsr = PackedStruct<InstructionBits,					  //
									  PackedMember<Operation, 0, 8>,	  // Operation
									  PackedMember<Condition, 8, 4>,	  // Condition
																		  //
									  PackedMember<Register, 16, 5>,	  // Destination
									  PackedMember<ShiftOperand, 32, 16>, // Source
									  PackedMember<bool, 48, 1>			  // Flags only
									  >;

export using Multiply =
	PackedStruct<InstructionBits,				//
				 PackedMember<Operation, 0, 8>, // Operation
				 PackedMember<Condition, 8, 4>, // Condition
												//
				 PackedMember<Register, 16, 4>, // Destination (high bytes)
				 PackedMember<Register, 24, 4>, // Accumulator / destination low bytes
				 PackedMember<Register, 32, 4>, // First
				 PackedMember<Register, 48, 4>, // Second
				 PackedMember<bool, 56, 1>,		// Set condition codes
				 PackedMember<bool, 57, 1>,		// Accumulate
				 PackedMember<bool, 58, 1>,		// Long
				 PackedMember<bool, 59, 1>		// Unsigned
				 >;
// NOLINTEND(*-magic-numbers)

template <typename Type>
concept InstructionType =					 //
	std::same_as<Type, BranchAndExchange> || //
	std::same_as<Type, Branch> ||			 //
	std::same_as<Type, DataProcessing> ||	 //
	std::same_as<Type, MoveFromPsr> ||		 //
	std::same_as<Type, MoveToPsr> ||		 //
	std::same_as<Type, Multiply>;			 //

export class Instruction
{
public:
	constexpr explicit Instruction(const InstructionBits t_underlying) : m_underlying(t_underlying)
	{
	}

	constexpr explicit Instruction(const InstructionType auto t_instruction)
		: m_underlying(t_instruction.to_underlying())
	{
	}

	[[nodiscard]] constexpr auto get_operation() const noexcept
	{
		const auto operation_bits{get_bits(m_underlying, {0_bi, 8_bs})};
		const auto operation{static_cast<Operation>(operation_bits.get())};

		// May need to check elsewhere
		/*
		if (operation > Operation::Undefined)
		{
			throw std::runtime_error("Invalid operation");
		}
		*/

		return operation;
	};

	[[nodiscard]] constexpr auto get_condition() const noexcept
	{
		const auto condition_bits{get_bits(m_underlying, {8_bi, 4_bs})};
		const auto operation{static_cast<Condition>(condition_bits.get())};

		// May need to check elsewhere
		/*
		if (operation > Condition::Al)
		{
			throw std::runtime_error("Invalid condition");
		}
		*/

		return operation;
	};

	template <InstructionType Type> [[nodiscard]] constexpr auto get() const noexcept
	{
		return Type(m_underlying);
	}

private:
	InstructionBits m_underlying;
};

} // namespace dzl::ins
