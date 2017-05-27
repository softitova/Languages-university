#include "BindNew.h"
//#include <iostream>
//
//
//
//int sum(int a, int b, int c)
//{
//    return a + b + c;
//}
//int main() {
//    auto f = [](int &a, int &b, int &c) { return a + b + c;};
//    int a = 1, b = 2, c = 3;
//        std::cout << (bind(&sum, 0, -5, 5))() << std::endl;
//        std::cout << (bind(&sum, bind(&sum, _2, 10, _1), _1, _1))(6, -15) << std::endl;
//        return 0;
//}

#include <cassert>

#if 1
//#include "bind.h"
#else
#include <functional>
using std::bind;
using namespace std::placeholders;
#endif

//#define bind my_bind

void f(int a, int b, int c)
{
    assert(a == 42);
    assert(b == 666);
    assert(c == 2016);
}

int sum(int a, int b)
{
    return a + b;
}

int add(int& lhs, int rhs)
{
    lhs += rhs;
    return lhs;
}

struct counter
{
    counter()
    : value(0)
    {}
    
    int operator()(int delta)
    {
        value += delta;
        return value;
    }
    
    int get() const
    {
        return value;
    }
    
private:
    int value;
};

struct noncopyable
{
    noncopyable() = default;
    
    noncopyable(noncopyable const&) = delete;
    noncopyable(noncopyable&&) = default;
    
    noncopyable& operator=(noncopyable const&) = delete;
    noncopyable& operator=(noncopyable&&) = default;
};

int main()
{
    bind(f, 42, 666, 2016)();
    bind(f, _3, _1, _2)(666, 2016, 42);
    bind(f, _2, 666, 2016)(123, 42);
    
    bind(f, 42, _1, bind(sum, _1, _2))(666, 1350);
    
    {
        auto bound = bind(sum, 1, _1);
        assert(bound(5) == 6);
        assert(bound(6) == 7);
    }
    
    {
        int val = 13;
        bind(add, _1, 5)(val);
        assert(val == 18);
        bind(add, _1, 5)(val);
        assert(val == 23);
    }
    
    {
        int val = 1;
        auto bound = bind(add, val, 1);
        assert(bound() == 2);
        assert(val == 1);
        assert(bound() == 3);
        assert(val == 1);
    }
    
    {
        counter c;
        auto bound = bind(c, 5);
        assert(bound() == 5);
        assert(c.get() == 0);
        assert(bound() == 10);
   //     assert(c.get() == 0);
    }
    
    bind([](noncopyable const&, int) {}, noncopyable{}, _1)(5);
    
    bind([](noncopyable const&, noncopyable const&) {}, noncopyable{}, _1)(noncopyable{});
}
