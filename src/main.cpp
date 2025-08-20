import std;

import unsigned_integer;
import bit_manipulation;
import packed_struct;

import types;
import shift_operand;
import instruction;
import arm_instruction;
import instruction_formatting;

auto main(const int t_argument_count, const char** t_arguments) -> int
{
	if (t_argument_count <= 1)
	{
		// return 1;
	}

	const std::span<const char* const> arguments(t_arguments,
												 static_cast<std::size_t>(t_argument_count));

	constexpr static dzl::ins::Instruction instruction{
		dzl::ins::MoveToPsr(
			dzl::ins::Operation::MoveToPsr, dzl::Condition::Al, dzl::Register::Spsr, dzl::ShiftOperand(0xD5_u8, 0_sh), false)
			.to_underlying()};

	std::println("{}", instruction);
}