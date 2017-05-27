#ifndef MY_BIND
#define MY_BIND
#include <cstdlib>
#include <tuple>

    template<size_t N>
    struct place_holder {};
    
    template<>
    struct place_holder<0>;
    
    template<size_t... N>
    struct indexes {};
    
    template <size_t First, size_t... Indeces>
    struct indexes_container
    {
        typedef typename indexes_container<First - 1, First - 1, Indeces...>::type type;
    };
    
    template<size_t... Indeces>
    struct indexes_container<0, Indeces...>
    {
        typedef indexes<Indeces...> type;
    };
    
    template<typename Func_t, typename... Args>
    struct my_bind
    {
    
        my_bind(Func_t &&f, Args&&... args): cur_func(std::forward<Func_t>(f)), my_tuple(std::forward<Args>(args)...) {}
    
    public:
        
        template <typename... Params>
        auto operator()(Params&&... params) const
        {
            return call(typename indexes_container<std::tuple_size<container>::value>::type(),std::forward<Params>(params)...);
        }
        
    private:
        
        typedef std::tuple<Args...> container;
        Func_t cur_func;
        container my_tuple;
        
        template <size_t PH_t, typename... Params>
        auto&& get(place_holder<PH_t>, Params&... params) const
        {
            return std::get<PH_t - 1>(std::forward_as_tuple(params...));
        }
        
        template <typename Simple_t, typename... Params>
        auto&& get(Simple_t&& current_expr, Params&... params) const
        {
            return current_expr;
        }
        
        template <typename Func_t_cur, typename... Args_t, typename... Params>
        auto get(const my_bind<Func_t_cur, Args_t...> &m_b, Params&... params) const
        {
            return m_b(params...);
        }
        
        template<size_t... Indeces, typename... Params>
        auto call(const indexes<Indeces...> &q, Params&&... params) const
        {
            return cur_func(get(std::get<Indeces>(my_tuple), params...)...);
        }
    };

    place_holder<1> _1;
    place_holder<2> _2;
    place_holder<3> _3;
    place_holder<4> _4;
    place_holder<5> _5;

    template<typename Func_t, typename... Args>
    my_bind<Func_t, Args...> bind(Func_t &&f, Args&&... args)
    {
        return my_bind<Func_t, Args...>(std::forward<Func_t>(f), std::forward<Args>(args)...);
    }
#endif