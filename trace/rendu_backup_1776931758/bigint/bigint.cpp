#include "bigint.hpp"

bigint::bigint() { str = "0"; }

bigint::bigint(unsigned int num) {
	std::stringstream ss;
	ss << num;
	str = ss.str();
}

bigint::bigint(const bigint& src) { *this = src; }

bigint& bigint::operator=(const bigint& src) {
	if (this != &src)
		str = src.str;
	return *this;
}

std::string bigint::getStr() const { return str; }

static std::string rev(const std::string& s) {
	std::string r;
	for (size_t i = s.length(); i > 0; i--)
		r.push_back(s[i - 1]);
	return r;
}

static unsigned int toUint(const std::string& s) {
	std::stringstream ss(s);
	unsigned int r;
	ss >> r;
	return r;
}

bigint bigint::operator+(const bigint& o) const {
	std::string s1 = rev(str), s2 = rev(o.getStr());
	while (s1.length() < s2.length()) s1.push_back('0');
	while (s2.length() < s1.length()) s2.push_back('0');
	std::string res;
	int carry = 0;
	for (size_t i = 0; i < s1.length(); i++) {
		int sum = (s1[i] - '0') + (s2[i] - '0') + carry;
		carry = sum / 10;
		res.push_back(sum % 10 + '0');
	}
	if (carry)
		res.push_back(carry + '0');
	bigint tmp;
	tmp.str = rev(res);
	return tmp;
}

bigint& bigint::operator+=(const bigint& o) { *this = *this + o; return *this; }
bigint& bigint::operator++() { *this = *this + bigint(1); return *this; }
bigint bigint::operator++(int) { bigint t = *this; ++(*this); return t; }

bigint bigint::operator<<(unsigned int n) const {
	bigint t = *this;
	t.str.insert(t.str.end(), n, '0');
	return t;
}

bigint bigint::operator>>(unsigned int n) const {
	bigint t = *this;
	if (n >= t.str.length()) t.str = "0";
	else t.str.erase(t.str.length() - n, n);
	return t;
}

bigint& bigint::operator<<=(unsigned int n) { *this = *this << n; return *this; }
bigint& bigint::operator>>=(unsigned int n) { *this = *this >> n; return *this; }

bigint bigint::operator<<(const bigint& o) const { return *this << toUint(o.str); }
bigint bigint::operator>>(const bigint& o) const { return *this >> toUint(o.str); }
bigint& bigint::operator<<=(const bigint& o) { *this = *this << toUint(o.str); return *this; }
bigint& bigint::operator>>=(const bigint& o) { *this = *this >> toUint(o.str); return *this; }

bool bigint::operator==(const bigint& o) const { return str == o.str; }
bool bigint::operator!=(const bigint& o) const { return !(*this == o); }
bool bigint::operator<(const bigint& o) const {
	if (str.length() != o.str.length()) return str.length() < o.str.length();
	return str < o.str;
}
bool bigint::operator>(const bigint& o) const { return !(*this < o) && !(*this == o); }
bool bigint::operator<=(const bigint& o) const { return *this < o || *this == o; }
bool bigint::operator>=(const bigint& o) const { return *this > o || *this == o; }

std::ostream& operator<<(std::ostream& out, const bigint& obj) {
	out << obj.getStr();
	return out;
}