#ifndef BIGINT
#define BIGINT

#include <sstream>
#include <iostream>
#include <string>

class bigint
{
	private:
		std::string digits;
	public:
		bigint();
		bigint(unsigned int value);
		bigint(const bigint& other);

		std::string getStr() const;

		bigint& operator=(const bigint& other);

		bigint operator+(const bigint& other) const;
		bigint& operator+=(const bigint& other);

		bigint& operator++();
		bigint operator++(int);

		bigint operator<<(unsigned int shift) const;
		bigint operator>>(unsigned int shift) const;
		bigint& operator<<=(unsigned int shift);
		bigint& operator>>=(unsigned int shift);

		bigint operator<<(const bigint& other) const;
		bigint operator>>(const bigint& other) const;
		bigint& operator<<=(const bigint& other);
		bigint& operator>>=(const bigint& other);

		bool operator==(const bigint& other) const;
		bool operator!=(const bigint& other) const;
		bool operator<(const bigint& other) const;
		bool operator>(const bigint& other) const;
		bool operator<=(const bigint& other) const;
		bool operator>=(const bigint& other) const;
};

std::ostream& operator<<(std::ostream& output, const bigint& value);

#endif
