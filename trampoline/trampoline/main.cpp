//
//  main.cpp
//  trampoline
//
//  Created by Titova Sophia on 21.06.17.
//  Copyright © 2017 Titova Sophia. All rights reserved.
//
#include <iostream>
#include <cstdio>
#include <vector>
#include "trampoline.h"
#include <cassert>
using namespace std;

struct test_struct_1 {
    int operator()(int p1, int p2, int p3, int p4, int p5) {
        return 123;
    }
};

void test_base() {
    cout << "Test 0 started" << endl;
    test_struct_1 tmp;
    trampoline<int (int, int, int, int, int)> tr(tmp);
    auto p = tr.get();
    assert (123 == p(1, 1, 1, 1, 1));
    cout << "Test 0 PASSED" << endl << endl;
}


void test_ptr() {
    cout << "Test 1 started" << endl;
    std::function<char*(int *)> fun = [](int *){return std::make_shared<char>('a').get();};
    trampoline<char*(int*)> tr(fun);
    auto p = tr.get();
    assert ('a' == *p(nullptr));
    cout << "Test 1 PASSED" << endl << endl;
}


void test_multipal_types_less_six_args() {
    
    cout << "Test 2 started" << endl;
    
    {
        trampoline<int (int, int, int, int, int)>
        t([&] (int p0, int p1, int p2, int p3, int p4) {return p1 + p2 + p3 + p4 + p0;});
        auto p = t.get();
        assert (5 == p(1, 1, 1, 1, 1));
        cout << " int test completed"  << endl;
    }
    {
        trampoline<double (double, double, double, double, double)>
        t([&] (double p0, double p1, double p2, double p3, double p4) {return p1 + p2 + p3 + p4 + p0;});
        auto p = t.get();
        assert (5.2 == p(1.0, 1.2, 1, 1, 1));
        cout << " double test completed"  << endl;
    }
    {
        trampoline<float (float, float, float, float, float)>
        t([&] (float p0, float p1, float p2, float p3, float p4) {return p1 + p2 + p3 + p4 + p0;});
        auto p = t.get();
        float res = 5.2;
        assert (res == p(1.0, 1.2, 1, 1, 1));
        cout << " float test completed"  << endl;
    }
    {
        trampoline<float (int, double, int, float, float)>
        t([&] (int p0, double p1, int p2, float p3, float p4) {return (p1 + p2 + p3 + p4 + p0);});
        auto p = t.get();
        float res = 5.2;
        assert (res == p(1.0, 1.2, 1, 1, 1));
        cout << " int/double/float test completed"  << endl;
    }
    {
        trampoline<int (int&)>
        t([&] (int p0) {return p0;});
        auto p = t.get();
        int a = 1;
        assert (1 == p(a));
        cout << " & test completed"  << endl;
    }
    cout << "Test 2 PASSED" << endl << endl;
}

void test_multipal_types_more_five_args_simple() {
    cout << "Test 3 started" << endl;
    {
        trampoline<long long (int, int, int, int, int, int, int, int)>
        t([&] (int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7)
        {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
        cout << " int test completed"  << endl;

    }
    {
        trampoline<long long (double, double, double, double, double, double, double, double)>
        t([&] (double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7)
          {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
        cout << " double test completed"  << endl;
    }
    {
        trampoline<long long (float, float, float, float, float, float, float, float)>
        t([&] (float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
          {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
        cout << " float test completed"  << endl;
    }
    {
        trampoline<long long (double, int, float, int, int, double, double, float)>
        t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
          {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
        cout << " int/double/float test completed"  << endl;
    }
    {
        trampoline<long long (double&, int&, float&, int, int, double, double, float&)>
        t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
          {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        double a = 1;
        int b = 1;
        float c = 1, d = 1;
        assert (8 == p(a, b, c, 1, 1, 1, 1, d));
        cout << " &/int/double/float test completed"  << endl;
    }
    cout << "Test 3 PASSED" << endl << endl;
}

void test_multipal_types_more_five_args() {
    cout << "Test 4 started" << endl;
    {
        trampoline<float (double, int, float, int, int, double, double, float)>
        t1([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
          {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p1 = t1.get();
        
        trampoline<float (double, int, float, int, const int&, double&, double, float&)>
        t2([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
           {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        const int a = 1;
        double b = 3.7;
        float c = 4.1;
        auto p2 = t2.get();
        assert ((float)(p1(1, 1, 1, 1, 1, 1, 1, 1) + 103.8) == p2(1, 2, 100, -1, a, b, 1, c));
        cout << " int/double/float test completed"  << endl;
    }
    {
        trampoline<float (double, int, float, int, int, double, double, float)>
        t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
           {return p2;});
        auto p = t.get();
        assert(p(1, 2, 3, 4, 5, 6, 7, 8) == 3);
        cout<<" parms sequence test completed" << endl;
    }
    {
        trampoline<int (double, int, float, int, int, double, double, float)>
        t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
           {return p7;});
        auto p = t.get();
        p(2, 3, 4, 5, 6, 7, 8, 9);
        assert(p(1, 2, 3, 4, 5, 6, 7, 8.8) == 8);
        cout<<" return parm test completed" << endl;
    }
    {
        
    }
    cout << "Test 4 PASSED" << endl << endl;
}

int main()
{
    test_base();
    test_ptr();
    test_multipal_types_less_six_args();
    test_multipal_types_more_five_args_simple();
    test_multipal_types_more_five_args();
    return 0;
}



