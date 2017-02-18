#include "my_vector.h"
#include <algorithm>

const size_t MANY = 2;

bool my_vector::is_small() const
{
	return elements <= 1;
}

my_vector::my_vector(size_t size, uint32_t el)
{
	elements = std::min(size, MANY);
	if (is_small()) {
		digit = el;
	}
	else {
		*vec = std::make_shared<std::vector<uint32_t>>(size, el);
	}
}

my_vector::my_vector(my_vector const& other) : elements(other.elements)
{
	if (is_small()) {
		digit = other.digit;
	}
	else {
		vec = new std::shared_ptr<std::vector<uint32_t>>(*other.vec);
	}
}

my_vector::~my_vector()
{
	if (!is_small()) {
		delete vec;
	}
}

void my_vector::copy()
{
	if (vec->use_count() > 1) {
		*this->vec = std::make_shared<std::vector<uint32_t>>(*(*this->vec));
	}
}

my_vector& my_vector::operator=(my_vector const& other)
{
	if (&other == this) {
		return *this;
	}
	if (!is_small()) {
		delete vec;
	}
	elements = other.elements;
	if (is_small()) {
		digit = other.digit;
	}
	else {
		vec = new std::shared_ptr<std::vector<uint32_t>>(*other.vec);
	}
	return *this;
}

uint32_t& my_vector::operator[](size_t i)
{
	if (is_small()) {
		return digit;
	}
	else {
		copy();
		return (*vec)->at(i);
	}
}

const uint32_t my_vector::operator[](size_t i) const
{
	if (is_small()) {
		return digit;
	}
	else {
		return (*vec)->at(i);
	}
}

uint32_t& my_vector::back()
{
	if (is_small()) {
		return digit;
	}
	else {
		copy();
		return (*vec)->back();
	}
}

const uint32_t my_vector::back() const
{
	if (is_small()) {
		return digit;
	}
	else {
		return (*vec)->back();
	}
}

void my_vector::resize(size_t new_size)
{
	if (new_size <= size()) {
		return;
	}
	if (new_size == 1) {
		elements = 1;
		digit = 0;
	}
	else {
		if (is_small()) {
			uint32_t buf = digit;
			vec = new std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>(new_size, 0));
			if (elements == 1) {
				(*vec)->at(0) = buf;
			}
			elements = MANY;
		}
		else {
			copy();
			(*vec)->resize(new_size);
		}
	}
}

size_t my_vector::size() const
{
	if (is_small()) {
		return elements;
	}
	else {
		return (*vec)->size();
	}
}

void my_vector::push_back(uint32_t x)
{
	resize(size() + 1);
	back() = x;
}

void my_vector::pop_back()
{
	if (!is_small()) {
		if ((*vec)->size() == 2) {
			--elements;
			digit = (*vec)->at(0);
		}
		else {
			copy();
			(*vec)->pop_back();
		}
	}
	else {
		--elements;
	}
}
