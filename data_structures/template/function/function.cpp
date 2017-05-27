#include <iostream>
#include <cassert>
#include "function.h"
int sum(int a, int b){
    return a + b;
}

int sub(int a, int b, int c){
    return a - b - c;
}

int mul(int a, int b){
    return a * b;
}

function <int(int, int)> my_func(){
    return sum;
}

function <int(int, int, int)> my_func_triple(){
    return sub;
}

int main()
{
    function <int(int, int)> f;
    f = sum;
    function <int(int, int)> ff;
    assert(!(ff));
    assert((f));
    ff = mul;
    f = ff;
    ff = sum;
    
    function <int(int, int)> fff = my_func();
    assert(fff(100, 2) == 102);
    function <int(int, int)> g;
    function <int(int, int,int)> my_sub = my_func_triple();
    assert(my_sub(10, 3, 0) == 7);
    assert((ff));
    assert((f));
    assert(f(5, 6) == 30);
    assert(ff(5, 6) == 11);
    swap(f, ff);
    assert(ff(5, 6) == 30);
    assert(f(5, 6) == 11);
    f.swap(ff);
    assert(f(5, 6) == 30);
    assert(ff(5, 6) == 11);
    return 0;
}