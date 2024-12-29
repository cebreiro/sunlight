#pragma once

namespace sunlight
{
    class StatValue
	{
    public:
        using value_type = double;

	public:
		StatValue() = default;
		explicit(false) StatValue(value_type value);

		auto operator+(const StatValue& other) const -> StatValue;
		auto operator-(const StatValue& other) const -> StatValue;
		auto operator*(const StatValue& other) const -> StatValue;
		auto operator/(const StatValue& other) const -> StatValue;

		void operator+=(const StatValue& other);
		void operator-=(const StatValue& other);
		void operator*=(const StatValue& other);
		void operator/=(const StatValue& other);

		bool operator==(const StatValue& other) const;
		bool operator!=(const StatValue& other) const;
		bool operator<(const StatValue& other) const;
		bool operator<=(const StatValue& other) const;
		bool operator>(const StatValue& other) const;
		bool operator>=(const StatValue& other) const;

		template <typename T> requires std::is_arithmetic_v<T>
		auto As() const -> T
		{
			return static_cast<T>(_value);
		}

        static auto Zero() -> StatValue;

	private:
        value_type _value = {};
	};
}
