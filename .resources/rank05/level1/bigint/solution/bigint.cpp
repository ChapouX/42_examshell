#include "bigint.hpp"

bigint::bigint() { digits = "0"; }

bigint::bigint(unsigned int value)
{
	std::stringstream ss;
	ss << value;
	digits = ss.str();
}

bigint::bigint(const bigint& other) { *this = other; }

bigint& bigint::operator=(const bigint& other)
{
	if (this != &other)
		digits = other.digits;
	return *this;
}

std::string bigint::getStr() const { return digits; }

static std::string reverse_string(const std::string& s)
{
	std::string result;
	for (size_t i = s.length(); i > 0; i--)
		result.push_back(s[i - 1]);
	return result;
}

static unsigned int string_to_uint(const std::string& s)
{
	std::stringstream ss(s);
	unsigned int value;
	ss >> value;
	return value;
}

bigint bigint::operator+(const bigint& other) const
{
	std::string left = reverse_string(digits), right = reverse_string(other.getStr());
	while (left.length() < right.length())
		left.push_back('0');
	while (right.length() < left.length())
		right.push_back('0');
	std::string result;
	int carry = 0;
	for (size_t i = 0; i < left.length(); i++)
	{
		int sum = (left[i] - '0') + (right[i] - '0') + carry;
		carry = sum / 10;
		result.push_back(sum % 10 + '0');
	}
	if (carry)
		result.push_back(carry + '0');
	bigint sum_value;
	sum_value.digits = reverse_string(result);
	return sum_value;
}

bigint& bigint::operator+=(const bigint& other) { *this = *this + other; return *this; }
bigint& bigint::operator++() { *this = *this + bigint(1); return *this; }
bigint bigint::operator++(int) { bigint before = *this; ++(*this); return before; }

bigint bigint::operator<<(unsigned int shift) const
{
	bigint result = *this;
	if (result.digits != "0")
		result.digits.insert(result.digits.end(), shift, '0');
	return result;
}

bigint bigint::operator>>(unsigned int shift) const
{
	bigint result = *this;
	if (shift >= result.digits.length())
		result.digits = "0";
	else
		result.digits.erase(result.digits.length() - shift, shift);
	return result;
}

bigint& bigint::operator<<=(unsigned int shift) { *this = *this << shift; return *this; }
bigint& bigint::operator>>=(unsigned int shift) { *this = *this >> shift; return *this; }

bigint bigint::operator<<(const bigint& other) const { return *this << string_to_uint(other.digits); }
bigint bigint::operator>>(const bigint& other) const { return *this >> string_to_uint(other.digits); }
bigint& bigint::operator<<=(const bigint& other) { *this = *this << string_to_uint(other.digits); return *this; }
bigint& bigint::operator>>=(const bigint& other) { *this = *this >> string_to_uint(other.digits); return *this; }

bool bigint::operator==(const bigint& other) const { return digits == other.digits; }
bool bigint::operator!=(const bigint& other) const { return !(*this == other); }
bool bigint::operator<(const bigint& other) const
{
	if (digits.length() != other.digits.length())
		return digits.length() < other.digits.length();
	return digits < other.digits;
}
/* NOT "!(*this < other)" alone: that would make (a > a) true. */
bool bigint::operator>(const bigint& other) const { return !(*this < other) && !(*this == other); }
bool bigint::operator<=(const bigint& other) const { return *this < other || *this == other; }
bool bigint::operator>=(const bigint& other) const { return *this > other || *this == other; }

std::ostream& operator<<(std::ostream& output, const bigint& value)
{
	output << value.getStr();
	return output;
}
