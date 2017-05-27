#ifndef BIND_H
#define BIND_H
#include <cstdlib>
#include <tuple>
#include "val.h"
#include "arg.h"

template <typename Ph>
struct holder {
    typedef val<Ph> type;
};

template<size_t N>
struct holder<arg<N>> {
    typedef arg<N> type;
};

template<typename Func_t,typename ... Params>
struct my_bind;
template<typename Func_t,typename ... Params>
struct holder <my_bind<Func_t, Params ...>> {
    typedef my_bind<Func_t, Params ...> type;
};

template <size_t ... N>
struct indexes{};

template <size_t F_arg, size_t ... Indexes>
struct indexes_container {
    typedef typename indexes_container<F_arg - 1, F_arg - 1, Indexes ... >::type type;
};

template <size_t ... Indexes>
struct indexes_container<0, Indexes ... > {
    typedef indexes<Indexes ...> type;
};

template<typename Func_t,typename ... Params>

struct my_bind {
    my_bind(Func_t f, Params&& ... params) :
    f(std::move(f)), p(std::forward<Params>(params) ... ) {}
    template< typename ... Args>
    auto operator()(Args const& ... args) const {
        return call(typename indexes_container<std::tuple_size<container>::value>::type(),args...);
    }
    
private:
    Func_t f;
    typedef std::tuple<typename holder<Params>::type ...> container;
    container p;
    template<size_t ... Indexes, typename ... Args>
    auto call(const indexes<Indexes ... >, Args const& ... args) const {
        return f(std::get<Indexes>(p)(args...)...);
    }
    
};



template <typename Func_t, typename ... Params>
my_bind<Func_t,  Params ...> bind(Func_t f, Params ... params)
{
    return my_bind<Func_t,  Params ...>(std::move(f), std::forward<Params>(params)...);
}
int sum(int a, int b, int c)
{
    return a + b + c;
}

constexpr arg<0> _1;
constexpr arg<1> _2;
constexpr arg<2> _3;

#endif