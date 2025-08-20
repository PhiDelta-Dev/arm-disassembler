export module instruction_formatting;

import std;

import unsigned_integer;
import types;
import shift_operand;
import instruction;

// Condition
template <> struct std::formatter<dzl::Condition> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::Condition t_condition,
										std::format_context& t_context) const
	{
		constexpr static std::array names{"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
										  "hi", "ls", "ge", "lt", "gt", "le", ""};

		const auto index{static_cast<std::size_t>(t_condition)};
		const auto formatted{std::format("{}", names.at(index))};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Register
template <> struct std::formatter<dzl::Register> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::Register t_register,
										std::format_context& t_context) const
	{
		constexpr static std::array names{"r0",	 "r1", "r2", "r3", "r4",   "r5",
										  "r6",	 "r7", "r8", "r9", "r10",  "r11",
										  "r12", "sp", "lr", "pc", "cpsr", "spsr"};

		const auto index{static_cast<std::size_t>(t_register)};
		const auto formatted{std::format("{}", names.at(index))};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// ShiftType
template <> struct std::formatter<dzl::ShiftType> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ShiftType t_shift_type,
										std::format_context& t_context) const
	{
		constexpr static std::array names{"lsl", "lsr", "asr", "ror", "rrx"};

		const auto index{static_cast<std::size_t>(t_shift_type)};
		const auto formatted{std::format("{}", names.at(index))};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// ShiftOperand
template <> struct std::formatter<dzl::ShiftOperand> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ShiftOperand t_shift_operand,
										std::format_context& t_context) const
	{
		switch (t_shift_operand.get_type())
		{
		case dzl::ShiftOperandType::Immediate:
		{
			const auto [type, value]{t_shift_operand.get<dzl::ImmediateOperand>()};

			const auto formatted{std::format("#{:#x}", value.get())};
			return std::formatter<std::string>::format(formatted, t_context);
		}
		case dzl::ShiftOperandType::RotatedImmediate:
		{
			const auto [type, source, amount]{t_shift_operand.get<dzl::RotatedImmediateOperand>()};

			const dzl::Word extended_source(source.get());
			const auto rotate_result{rotate_right(extended_source, amount)};

			const auto formatted{std::format("#{:#x}", rotate_result.get())};
			return std::formatter<std::string>::format(formatted, t_context);
		}
		case dzl::ShiftOperandType::ImmediateShiftedRegister:
		{
			const auto [type, source, shift_type,
						amount]{t_shift_operand.get<dzl::ImmediateShiftedRegisterOperand>()};

			// Rotate right extended
			if (shift_type == dzl::ShiftType::RotateRightExtended)
			{
				const auto formatted{
					std::format("{}, {}", source, dzl::ShiftType::RotateRightExtended)};
				return std::formatter<std::string>::format(formatted, t_context);
			}

			// Unshifted register
			if (amount == 0_sh)
			{
				const auto formatted{std::format("{}", source)};
				return std::formatter<std::string>::format(formatted, t_context);
			}

			const auto formatted{std::format("{}, {} #{}", source, shift_type, amount.get())};
			return std::formatter<std::string>::format(formatted, t_context);
		}
		case dzl::ShiftOperandType::RegisterShiftedRegister:
		{
			const auto [type, source, shift_type,
						amount]{t_shift_operand.get<dzl::RegisterShiftedRegisterOperand>()};

			const auto formatted{std::format("{}, {} {}", source, shift_type, amount)};
			return std::formatter<std::string>::format(formatted, t_context);
		}
		default:
			std::unreachable();
		}
	}
};

// Branch and exchange
template <> struct std::formatter<dzl::ins::BranchAndExchange> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::BranchAndExchange t_instruction,
										std::format_context& t_context) const
	{
		const auto [operation, condition, destination]{t_instruction};

		const auto formatted{std::format("bx{} {}",	 //
										 condition,	 // Condition
										 destination // Destination
										 )};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Branch
template <> struct std::formatter<dzl::ins::Branch> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::Branch t_instruction,
										std::format_context& t_context) const
	{
		const auto [operation, condition, link, offset]{t_instruction};

		const auto formatted{std::format("b{}{} {:#x}",					//
										 condition,						// Condition
										 link ? 'l' : '\0',				// Link
										 static_cast<int>(offset.get()) // Offset
										 )};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Data processing
template <> struct std::formatter<dzl::ins::DataProcessing> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::DataProcessing t_instruction,
										std::format_context& t_context) const
	{
		const auto [operation, condition, op_code, set_condition_codes, destination, first,
					second]{t_instruction};

		constexpr static std::array names{"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
										  "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

		if (op_code == dzl::ins::DataProcessingOpCode::Tst ||
			op_code == dzl::ins::DataProcessingOpCode::Teq ||
			op_code == dzl::ins::DataProcessingOpCode::Cmp ||
			op_code == dzl::ins::DataProcessingOpCode::Cmn)
		{
			// No destination
			const auto formatted{std::format("{}{} {}, {}",							   //
											 names[static_cast<std::size_t>(op_code)], // Op code
											 condition,								   // Condition
											 first,									   // First
											 second									   // Second
											 )};

			return std::formatter<std::string>::format(formatted, t_context);
		}

		if (op_code == dzl::ins::DataProcessingOpCode::Mov ||
			op_code == dzl::ins::DataProcessingOpCode::Mvn)
		{
			// No first operand
			const auto formatted{
				std::format("{}{}{} {}, {}",						  //
							names[static_cast<std::size_t>(op_code)], // Op code
							condition,								  // Condition
							set_condition_codes ? 's' : '\0',		  // Set condition codes
							destination,							  // Destination
							second									  // Second
							)};

			return std::formatter<std::string>::format(formatted, t_context);
		}

		const auto formatted{std::format("{}{}{} {}, {}, {}",					   //
										 names[static_cast<std::size_t>(op_code)], // Op code
										 condition,								   // Condition
										 set_condition_codes ? 's' : '\0', // Set condition codes
										 destination,					   // Destination
										 first,							   // First
										 second							   // Second
										 )};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Move from PSR
template <> struct std::formatter<dzl::ins::MoveFromPsr> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::MoveFromPsr t_instruction,
										std::format_context& t_context) const
	{
		const auto [operation, condition, destination, first]{t_instruction};

		const auto formatted{std::format("mrs{} {}, {}", //
										 condition,		 // Condition
										 destination,	 // Destination
										 first			 // Source
										 )};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Move to PSR
template <> struct std::formatter<dzl::ins::MoveToPsr> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::MoveToPsr t_instruction,
										std::format_context& t_context) const
	{
		const auto [operation, condition, destination, source, flags_only]{t_instruction};

		const auto formatted{std::format("msr{} {}{}, {}",		   //
										 condition,				   // Condition
										 destination,			   // Destination
										 flags_only ? "_flg" : "", // Flags suffix
										 source					   // Source
										 )};

		return std::formatter<std::string>::format(formatted, t_context);
	}
};

// Instruction
template <> struct std::formatter<dzl::ins::Instruction> : std::formatter<std::string>
{
	[[nodiscard]] constexpr auto format(const dzl::ins::Instruction t_instruction,
										std::format_context& t_context) const
	{
		switch (t_instruction.get_operation())
		{
		case dzl::ins::Operation::BranchAndExchange:
			return std::formatter<std::string>::format(
				std::format("{}", t_instruction.get<dzl::ins::BranchAndExchange>()), t_context);
		case dzl::ins::Operation::Branch:
			return std::formatter<std::string>::format(
				std::format("{}", t_instruction.get<dzl::ins::Branch>()), t_context);
		case dzl::ins::Operation::DataProcessing:
			return std::formatter<std::string>::format(
				std::format("{}", t_instruction.get<dzl::ins::DataProcessing>()), t_context);
		case dzl::ins::Operation::MoveFromPsr:
			return std::formatter<std::string>::format(
				std::format("{}", t_instruction.get<dzl::ins::MoveFromPsr>()), t_context);
		case dzl::ins::Operation::MoveToPsr:
			return std::formatter<std::string>::format(
				std::format("{}", t_instruction.get<dzl::ins::MoveToPsr>()), t_context);
		case dzl::ins::Operation::Multiply:
		case dzl::ins::Operation::Load:
		case dzl::ins::Operation::Store:
		case dzl::ins::Operation::LoadMultiple:
		case dzl::ins::Operation::StoreMultiple:
		case dzl::ins::Operation::Swap:
		case dzl::ins::Operation::SoftwareInterrupt:
		case dzl::ins::Operation::CoprocessorDataOperation:
		case dzl::ins::Operation::CoprocessorLoad:
		case dzl::ins::Operation::CoprocessorStore:
		case dzl::ins::Operation::LoadCoprocessorRegister:
		case dzl::ins::Operation::StoreCoprocessorRegsiter:
		case dzl::ins::Operation::Undefined:
			throw std::runtime_error("PLEASE DEFINE THIS");
		default:
			std::unreachable();
		}
	}
};