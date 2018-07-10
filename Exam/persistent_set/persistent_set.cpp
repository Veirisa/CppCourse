#include "persistent_set.h"

persistent_set::persistent_set() : root(std::make_shared<persistent_node>()) {}

persistent_set::persistent_set(persistent_set const & other) : root(other.root) {}

persistent_set &persistent_set::operator=(persistent_set const &rhs) {
	this->root = rhs.root;
	return *this;
}

persistent_set::~persistent_set() {}

persistent_set::persistent_node::persistent_node() : ending(true), left_child(nullptr), right_child(nullptr), value(0) {}

persistent_set::persistent_node::persistent_node(value_type value) : ending(false), left_child(nullptr), right_child(nullptr), value(value) {}

persistent_set::iterator::iterator(std::vector<node_ptr> found, persistent_set const *p) : descent_sequence(found), parent_set(p) {}

const persistent_set::value_type &persistent_set::iterator::operator*() const {
	return descent_sequence.back()->value;
}

std::vector<persistent_set::node_ptr> persistent_set::find_node(node_ptr cur, value_type const &value) {
	std::vector<node_ptr> d;
	while (cur != nullptr && (cur->ending || (value != cur->value))) {
		d.push_back(cur);
		if (cur->ending || (value <= cur->value)) {
			cur = cur->left_child;
		}
		else {
			cur = cur->right_child;
		}
	}
	d.push_back(cur);
	return d;
}

persistent_set::iterator persistent_set::find(value_type value) {
	std::vector<node_ptr> found_descent = find_node(root, value);
	if (found_descent.back() == nullptr) {
		found_descent.clear();
		found_descent.push_back(root);
	}
	return persistent_set::iterator(found_descent, this);
}

std::pair<persistent_set::iterator, bool> persistent_set::insert(value_type value) {
	std::vector<node_ptr> found_descent = find_node(root, value);
	if (found_descent.back() == nullptr) {
		found_descent.pop_back();
		node_ptr newnode(std::make_shared<persistent_node>(value));
		root = ascend_and_rebuild(found_descent, newnode, false, value);
		return{
			find(value), true
		};
	}
	else {
		return{
			iterator(found_descent, this), false
		};
	}
}

persistent_set::iterator persistent_set::end() const {
	std::vector<node_ptr> d;
	d.push_back(root);
	return iterator(d, this);
}

persistent_set::iterator persistent_set::begin() const {
	std::vector<node_ptr> d;
	node_ptr cur = root;
	while (cur != nullptr) {
		d.push_back(cur);
		cur = cur->left_child;
	}
	return iterator(d, this);
}

void persistent_set::erase(iterator iter) {
	node_ptr to_upd = iter.descent_sequence.back();
	node_ptr old = to_upd;
	if (to_upd->left_child == nullptr && to_upd->right_child == nullptr) {
		to_upd = nullptr;
	}
	else if (to_upd->left_child == nullptr) {
		to_upd = std::make_shared<persistent_node>(*to_upd->right_child);
	}
	else if (to_upd->right_child == nullptr) {
		to_upd = std::make_shared<persistent_node>(*to_upd->left_child);
	}
	else {
		persistent_set subset;
		node_ptr new_ending(std::make_shared<persistent_node>());
		new_ending->left_child = to_upd->right_child;
		subset.root = new_ending;
		value_type repl = *(subset.begin());
		subset.erase(subset.begin());
		to_upd = node_ptr(new persistent_node(repl));
		to_upd->right_child = subset.root->left_child;
		to_upd->left_child = old->left_child;
	}
	iter.descent_sequence.pop_back();
	root = ascend_and_rebuild(iter.descent_sequence, to_upd, true, old->value);
}

persistent_set::node_ptr persistent_set::ascend_and_rebuild(std::vector<node_ptr> const &desc_seq, node_ptr new_node, bool del,
	value_type const &which) const {
	node_ptr new_tree = new_node;
	for (size_t j = desc_seq.size(); j > 0; --j) {
		node_ptr newptr(std::make_shared<persistent_node>(*desc_seq[j - 1]));
		if (new_tree != nullptr) {
			if (newptr->ending || (new_tree->value <= newptr->value)) {
				newptr->left_child = new_tree;
			}
			else {
				newptr->right_child = new_tree;
			}
		}
		if (del) {
			if (newptr->left_child != nullptr && newptr->left_child->value == which)
				newptr->left_child = nullptr;
			else if (newptr->right_child != nullptr && newptr->right_child->value == which)
				newptr->right_child = nullptr;
			del = false;
		}
		new_tree = newptr;
	}
	return new_tree;
}

bool operator==(persistent_set::iterator a, persistent_set::iterator b) {
	bool iea = a.descent_sequence.back()->ending;
	bool ieb = b.descent_sequence.back()->ending;
	return (iea && ieb) || (!iea && !ieb) && (*a) == (*b);
}

bool operator!=(persistent_set::iterator a, persistent_set::iterator b) {
	return !(a == b);
}

persistent_set::iterator &persistent_set::iterator::operator++() {
	node_ptr cur = descent_sequence.back();
	if (cur->right_child != nullptr) {
		cur = cur->right_child;
		while (cur->left_child != nullptr) {
			descent_sequence.push_back(cur);
			cur = cur->left_child;
		}
		descent_sequence.push_back(cur);
		return *this;
	}
	else {
		value_type val = cur->value;
		descent_sequence.pop_back();
		while (!(descent_sequence.back()->ending || (val <= descent_sequence.back()->value))) {
			descent_sequence.pop_back();
		}
		return *this;
	}
}

persistent_set::iterator persistent_set::iterator::operator++(int) {
	iterator cpy(*this);
	++(*this);
	return cpy;
}

persistent_set::iterator &persistent_set::iterator::operator--() {
	node_ptr cur = descent_sequence.back();
	if (cur->left_child != nullptr) {
		cur = cur->left_child;
		while (cur->right_child != nullptr) {
			descent_sequence.push_back(cur);
			cur = cur->right_child;
		}
		descent_sequence.push_back(cur);
	}
	else {
		descent_sequence.pop_back();
		while (!(descent_sequence.back()->ending || (cur->value >= descent_sequence.back()->value))) {
			descent_sequence.pop_back();
		}
	}
	return *this;
}

persistent_set::iterator persistent_set::iterator::operator--(int) {
	iterator cpy(*this);
	--(*this);
	return cpy;
}
