#include "bind.h"
#include <iostream>
#include <functional>

int f(int a) {
    return a / 10;
}

int g(unsigned int a, unsigned  int b, int c, int d) {
    return a + b * 3 + c * 5 + d * 7;
}

int h(int& a, int b, int c, int d) {
    a = 0;
    return b * 3 + c * 5 + d * 7;
}

int main() {
    bool ans;

    auto b0 = bind(f, _1);
    auto check0 = std::bind(f, std::placeholders::_1);
    ans = (b0(10) == check0(10));
    std::cout << ans << "\n";

    auto b1 = bind(g, 1, 10, 100, 1000);
    auto check1 = std::bind(g, 1, 10, 100, 1000);
    ans = (b1() == check1());
    std::cout << ans << "\n";

    auto b2 = bind(g, 1, 10, _1, _2);
    auto check2 = std::bind(g, 1, 10, std::placeholders::_1, std::placeholders::_2);
    ans = (b2(100, 1000) == check2(100, 1000));
    std::cout << ans << "\n";

    auto b3 = bind(g, b0, 10, _1, _2);
    auto check3 = std::bind(g, check0,  10, std::placeholders::_1, std::placeholders::_2);
    ans = (b3(100, 1000) == check3(100, 1000));
    std::cout << ans << "\n";

    auto b4 = bind(b2, 100, 1000);
    auto check4 = std::bind(check2, 100, 1000);
    ans = (b4(100, 1000) == check4(100, 1000));
    std::cout << ans << "\n";

    int x1 = 1;
    auto b5 = bind(h, x1, 10, 100, 1000);
    auto check5 = std::bind(h, x1, 10, 100, 1000);
    ans = (b5(100, 1000) == check5(100, 1000)) && (x1 == 1);
    std::cout << ans << "\n";
}