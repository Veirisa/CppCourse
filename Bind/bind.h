#ifndef BIND_H
#define BIND_H
#include <tuple>
#include <cstdint>

template <uint32_t N>
struct placeholder{};

placeholder<0> _1;
placeholder<1> _2;
placeholder<2> _3;
placeholder<3> _4;
placeholder<4> _5;
placeholder<5> _6;
placeholder<6> _7;
placeholder<7> _8;
placeholder<8> _9;
placeholder<9> _10;

template <typename F, typename ...Args>
class bind_object {
    std::tuple<typename std::decay<Args>::type...> args;
    typename std::decay<F>::type f;

    bind_object(F&& f, Args&&... args) : f(std::forward<F>(f)), args(std::forward<Args>(args)...) {};

    template <uint32_t ...elements>
    struct sequence{};

    template <uint32_t first_el, uint32_t ...next_el>
    struct do_sequence {
        typedef typename do_sequence<first_el - 1, first_el - 1, next_el...>::type type;
    };

    template <uint32_t ...next_el>
    struct do_sequence<0, next_el...> {
        typedef sequence<next_el...> type;
    };

    template <typename This_arg , typename ...Add_args>
    auto&& arguments(This_arg& this_arg, Add_args&... add_Args) {
        return this_arg;
    }

    template <uint32_t N, typename ...Add_args>
    auto&& arguments(placeholder<N> ph, Add_args&... add_args) {
        return std::get<N>(std::forward_as_tuple(add_args...));
    }

    template <typename Func, typename ...Func_args, typename ...Add_args>
    auto arguments(bind_object<Func, Func_args...>& inner_bind, Add_args&... add_args) {
        return inner_bind(add_args...);
    }

    template <uint32_t ...N, typename ...Add_args>
    auto do_f(const sequence<N...>& s, Add_args&&... add_args) {
        return f(arguments(std::get<N>(args), add_args...)...);
    }

public:

    template <typename ...Add_args>
    auto operator()(Add_args&&...add_args) {
        return do_f(typename do_sequence<std::tuple_size<std::tuple<Args...>>::value>::type(), std::forward<Add_args>(add_args)...);
    }

    template <typename F_fr, typename ...Args_fr>
    friend bind_object<F_fr, Args_fr...> bind(F_fr&& f, Args_fr&&... args);
};

template <typename F, typename ...Args>
bind_object<F, Args...> bind(F&& f, Args&&... args) {
    return bind_object<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
}

#endif
