#pragma once

#include "searchable_bag.hpp"

class set
{
	private:
		searchable_bag& bag;
	public:
		set() = delete;
		set(const set&) = delete;
		set& operator=(const set&) = delete;
		set(searchable_bag& bag);
		~set();
		bool has(int value) const;
		void insert(int value);
		void insert(int* values, int count);
		void print() const;
		void clear();
		const searchable_bag& get_bag();
};
