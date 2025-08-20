export module strong_type;

import std;

template <typename Type>
concept SimpleType = std::semiregular<Type> && std::same_as<Type, std::remove_cvref_t<Type>>;

export template <SimpleType UnderlyingType, typename TagParameter> class StrongType
{
public:
	using Underlying = UnderlyingType;
	using Tag = TagParameter;

	constexpr StrongType() = default;
	constexpr explicit StrongType(const Underlying t_value) : m_value(t_value) {}

	[[nodiscard]] constexpr auto get() const noexcept { return m_value; }
	[[nodiscard]] constexpr auto set(const Underlying t_value) noexcept { m_value = t_value; }

private:
	Underlying m_value{};
};

export template <typename Type>
concept Strong =
	requires(const Type t_const_value, Type& t_mutable_reference, const Type::Underlying t_value) {
		typename Type::Underlying;
		typename Type::Tag;

		{ Type() };
		{ Type(t_value) };

		{ t_const_value.get() } noexcept -> std::same_as<typename Type::Underlying>;
		{ t_mutable_reference.set(t_value) } noexcept -> std::same_as<void>;
	} && std::same_as<StrongType<typename Type::Underlying, typename Type::Tag>, Type>;