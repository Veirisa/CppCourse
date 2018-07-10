#include <iostream>
#include <limits>

struct signedNumber
{};

template <bool T>
struct checkedForm
{
	typedef signedNumber form;
};

struct unsignedNumber
{};

template <>
struct checkedForm<false>
{
	typedef unsignedNumber form;
};

template <typename T>
bool checkedMult(T a, T b)
{
	if (a != 0 && b != 0 && (((a * b) / b) != a) || ((a * b / a) != b)) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedAdd(T a, T b, signedNumber)
{
	if (((b > 0) && (a > std::numeric_limits<T>::max() - b)) || ((b < 0) && (a < std::numeric_limits<T>::min() - b))) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedSub(T a, T b, signedNumber)
{
	if (((b > 0) && (a < std::numeric_limits<T>::min() + b)) || ((b < 0) && (a > std::numeric_limits<T>::max() + b))) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedDiv(T a, T b, signedNumber)
{
	if ((b == 0) || ((a == std::numeric_limits<T>::min()) && (b == -1))) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedNegate(T a, signedNumber)
{
	if (a == std::numeric_limits<T>::min()) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedAdd(T a, T b, unsignedNumber)
{
	if (a > std::numeric_limits<T>::max() - b) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedSub(T a, T b, unsignedNumber)
{
	if (a < std::numeric_limits<T>::min() + b) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedDiv(T a, T b, unsignedNumber)
{
	if (b == 0) {
		return false;
	}
	return true;
}

template <typename T>
bool checkedNegate(T a, unsignedNumber)
{
	if (a != 0) {
		return false;
	}
	return true;
}

template <typename T>
struct checked
{
	T number;

	checked<T>()
	{
		number = 0;
	}

	checked<T>(T a)
	{
		number = a;
	}

	~checked<T>()
	{}

	checked<T> operator=(checked<T> const a)
	{
		number = a.number;
		return *this;
	}

	checked<T> operator*=(checked<T> const a)
	{
		if (checkedMult(number, a.number)) {
			number *= a.number;
		}
		else {
			std::overflow_error e("overflow multiplication");
			throw e;
		}
		return *this;
	}

	checked<T> operator+=(checked<T> const a)
	{
		if (checkedAdd(number, a.number, typename checkedForm<std::numeric_limits<T>::is_signed>::form())) {
			number += a.number;
		}
		else {
			std::overflow_error e("overflow addition");
			throw e;
		}
		return *this;
	}

	checked<T> operator-=(checked<T> const a)
	{
		if (checkedSub(number, a.number, typename checkedForm<std::numeric_limits<T>::is_signed>::form())) {
			number -= a.number;
		}
		else {
			std::overflow_error e("overflow subtracting");
			throw e;
		}
		return *this;
	}

	checked<T> operator/=(checked<T> const a)
	{
		if (checkedDiv(number, a.number, typename checkedForm<std::numeric_limits<T>::is_signed>::form())) {
			number /= a.number;
		}
		else {
			std::overflow_error e("overflow division");
			throw e;
		}
		return *this;
	}

	checked<T> operator-()
	{
		if (checkedNegate(number, typename checkedForm<std::numeric_limits<T>::is_signed>::form())) {
			number = -number;
		}
		else {
			std::overflow_error e("overflow negation");
			throw e;
		}
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& s, const checked<T> a)
	{
		return s << a.number;
	}
};

template <typename T>
checked<T> operator*(checked<T> a, checked<T> b)
{
	return a *= b;
}

template <typename T>
checked<T> operator+(checked<T> a, checked<T> b)
{
	return a += b;
}

template <typename T>
checked<T> operator-(checked<T> a, checked<T> b)
{
	return a -= b;
}

template <typename T>
checked<T> operator/(checked<T> a, checked<T> b)
{
	return a /= b;
}

int main() {
	return 0;
}
