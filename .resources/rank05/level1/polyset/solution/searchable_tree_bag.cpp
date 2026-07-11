#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag() {}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& other) : tree_bag(other) {}
searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag& other)
{
	if (this != &other)
		tree_bag::operator=(other);
	return *this;
}
searchable_tree_bag::~searchable_tree_bag() {}

bool searchable_tree_bag::search_node(node* current, int value) const
{
	if (!current)
		return false;
	if (current->value == value)
		return true;
	if (value < current->value)
		return search_node(current->l, value);
	return search_node(current->r, value);
}

bool searchable_tree_bag::has(int value) const { return search_node(tree, value); }
