#ifndef MY_VECTOR_H
#define MY_VECTOR_H
#include <cstdint>
#include <vector>
#include <memory>

struct my_vector
{
	my_vector(size_t size = 0, uint32_t el = 0);
	my_vector(my_vector const& other);
	~my_vector();

	size_t size() const;
	my_vector& operator=(my_vector const& other);
	uint32_t& operator[](size_t i);
	const uint32_t operator[](size_t i) const;
	uint32_t& back();
	const uint32_t back() const;
	void resize(size_t new_size);
	void push_back(uint32_t x);
	void pop_back();

private:

	size_t elements;
	union {
		uint32_t digit;
		std::shared_ptr<std::vector<uint32_t>>* vec;
	};
	bool is_small() const;
	void copy();
};

#endif //MY_VECTOR_H

