#ifndef VAL_H
#define VAL_H


template <typename T>
struct val {
    val(T value) : value(std::move(value)){}
    template<typename ... arg>
    T operator()(arg const& ...) const {
        return value;
    }
private:
    T value;
};

#endif