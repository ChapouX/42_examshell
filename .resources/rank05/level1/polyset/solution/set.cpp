#include "set.hpp"

set::set(searchable_bag& bag) : bag(bag) {}
set::~set() {}
bool set::has(int value) const { return bag.has(value); }
void set::insert(int value) { if (!has(value)) bag.insert(value); }
void set::insert(int* values, int count) { for (int i = 0; i < count; i++) insert(values[i]); }
void set::print() const { bag.print(); }
void set::clear() { bag.clear(); }
const searchable_bag& set::get_bag() { return bag; }
