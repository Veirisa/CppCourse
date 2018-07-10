#include <iostream>
#include "signals.h"

void f() {
    std::cout << "F ";
}

struct G {
    void operator()() {
        std::cout << "G ";
    }
};

struct H {
    void h() {
        std::cout << "H ";
    }
};

signals_soo<void(int)> signal_rec;

void s(int x) {
    std::cout << "S(" << x << ") ";
    if (x > 0) {
        signal_rec(x - 1);
    }
}

int main() {
    signals_soo<void()> signal;

    auto connect1 = signal.connect(&f);
    std::cout << "0: ";
    signal();

    auto connect2 = signal.connect(G());
    std::cout << "\n1: ";
    signal();

    auto connect3 = signal.connect([&signal]() { signal.connect(&f); });
    std::cout << "\n2: ";
    signal();

    H x;
    auto connect4 = signal.connect(std::bind(&H::h, &x));
    std::cout << "\n3: ";
    signal();

    connect4.disconnect();
    std::cout << "\n4: ";
    signal();

    connect3.disconnect();
    std::cout << "\n5: ";
    signal();

    connect2.disconnect();
    std::cout << "\n6: ";
    signal();

    connect1.disconnect();
    std::cout << "\n7: ";
    signal();

    signal.disconnect_all();
    std::cout << "\n8: ";
    signal();

    auto connect = signal_rec.connect(&s);
    std::cout << "\n9: ";
    signal_rec(3);
}
