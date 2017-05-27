#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>

template<typename T>
struct function;

template<typename F>
struct keep {};

template<typename R, typename... Args>
void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs);

template<typename R, typename... Args>
struct function<R(Args...)>
{
    
    function() : func(nullptr), caller(nullptr), copier(nullptr), deleter(nullptr) {}
    function(const function& other) : func(other.copier(other.func)),
    caller(other.caller),
    copier(other.copier),
    deleter(other.deleter) {}
    
    template<typename F>
    function(F f) : func(new F(std::move(f))),
    caller(my_call<F>),
    copier(my_copy<F>),
    deleter(my_delete<F>)
    {}
    
    R operator()(Args... args) {
        return caller(func, std::forward<Args>(args)...);
    }
    
    ~function() {
        if (func) {
            deleter(func);
            func = nullptr;
            caller = nullptr;
            copier = nullptr;
            deleter = nullptr;
        }
    }
    
    function& operator=(const function& other) {
        function tmp(other);
        swap(*this, tmp);
        return *this;
    }
    function& operator=(function&& other) {
        swap(*this, other);
        return *this;
    }
    
    template <class F>
    function& operator=(F&& func) {
        function tmp(std::move(func));
        ::swap(*this, tmp);
        return *this;
    }
    
    explicit operator bool() const {
        return func != nullptr;
    }
    
    void swap(function &other) {
        ::swap(*this, other);
    }
    
    friend void ::swap<>(function& a, function& b);

private:
    
    template<typename F>
    static R my_call(void *func, Args... args) {
        return  (*static_cast<F*>(func))(std::forward<Args>(args)...);
    }
    
    template<typename F>
    static void* my_copy(void *func) {
        return new F(*static_cast<F*>(func));
    }
    template<typename F>
    static void my_delete(void *func) {
        delete static_cast<F*>(func);
    }
    
    void* func;
    R (*caller)(void*, Args...);
    void* (*copier)(void*);
    void (*deleter)(void*);
};

template<typename R, typename... Args>
void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs) {
    std::swap(lhs.func, rhs.func);
    std::swap(lhs.caller, rhs.caller);
    std::swap(lhs.copier, rhs.copier);
    std::swap(lhs.deleter, rhs.deleter);
}


#endif