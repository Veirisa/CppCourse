#include "any.h"
#include <iostream>
#include <vector>

struct str {
    int a;
    int b;
    int c;
    double d;

    str() : a(10), b(20), c(30), d(40.0) {};
    str(const str& other) : a(other.a), b(other.b), c(other.c), d(other.d) {};
    str(str&& other) : a(std::move(other.a)), b(std::move(other.b)), c(std::move(other.c)), d(std::move(other.d)) {};

};

struct other_str : str {};

int main() {

    int x = 10;
    const int cx = 10;
    str y;
    const str cy;

    //constuctors

    any a0;

    any a_T_small0 (x);
    any a_T_small1(cx);
    any a_T_small2(10);

    any a_T_big0(y);
    any a_T_big1(cy);
    any a_T_big2(std::move(y));

    //copy constructors

    any a_copy(a0);

    const any a_copy_small0(a_T_small0);
    any a_copy_small1(a_copy_small0);
    any a_copy_small2(std::move(a_copy_small1));

    const any a_copy_big0(a_T_big0);
    any a_copy_big1(a_copy_big0);
    any a_copy_big2(std::move(a_copy_big1));

    //operator=

    a_T_small0 = a_T_small1;
    a_T_small2 = a_T_big2;
    a_copy_big2 = a_copy_small2;
    a_T_big1 = a_T_big0;

    //good casts

    std::cout << "good casts (small): ";

    std::cout << *any_cast<int>(&a_T_small0) << " ";
    std::cout << *any_cast<int>(&a_copy_small0) << " ";
    std::cout << any_cast<int>(a_T_small0) << " ";
    std::cout << any_cast<int>(a_copy_small0) << " ";
    std::cout << any_cast<int>(std::move(a_T_small0)) << " ";

    std::cout << "\ngood casts (big): ";

    std::cout << any_cast<str>(&a_T_big0)->a << " ";
    std::cout << any_cast<str>(&a_copy_big0)->a << " ";
    std::cout << any_cast<str>(a_T_big0).a << " ";
    std::cout << any_cast<str>(a_copy_big0).a << " ";
    std::cout << any_cast<str>(std::move(a_T_big0)).a << " ";

    //bad casts

    std::cout << "\nbad casts: ";

    std::cout << (any_cast<double>(&a_T_small0) == nullptr) << " ";
    std::cout << (any_cast<other_str>(&a_copy_big0) == nullptr) << " ";
    try {
        any_cast<other_str>(a_T_big0);
    } catch(std::bad_cast& e) {
        std::cout << "1 ";
    }
    try {
        any_cast<double>(a_copy_small0);
    } catch(std::bad_cast& e) {
        std::cout << "1 ";
    }
    try {
        any_cast<int>(std::move(a0));
    } catch(std::bad_cast& e) {
        std::cout << "1 ";
    }
    return 0;
}
