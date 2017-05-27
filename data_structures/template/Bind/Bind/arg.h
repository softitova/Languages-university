#ifndef ARGS_H
#define ARGS_H

template <size_t Number>
struct arg {
    constexpr arg(){}
    template <typename Zero_arg, typename ... Other>
    Zero_arg operator()(Zero_arg const& zero_arg, Other const& ... other) const {
        constexpr const arg<Number - 1> next;
        return next(other...);
    }
};


template<>
struct arg<0> {
    constexpr arg(){}
    template <typename Zero_arg, typename ... Other>
    Zero_arg operator()(Zero_arg const& zero_arg, Other const& ... other) const {
        return zero_arg;
    }
};

#endif
