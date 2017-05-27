#include <string>
#include <iostream>


// optional can be used as the return type of a factory that may fail
int maint() {
    return 0;
}


// 201







//function();
//(1)	(начиная с C++11)
//function( std::nullptr_t );
//(2)	(начиная с C++11)
//function( const function& f );
//(3)	(начиная с C++11)
//function( function&& f );
//(4)	(начиная с C++11)
//template< class F >
//function( F f );








//#include <iostream>
//
//template<typename Return, typename... Params>
//struct function{};
//
//template<typename Return, typename... Params>
//struct function<Return(Params...)>
//{
//    public:
//    
//    template<typename F>
//    function(F func) //func is copied here, it's ok, because function should live, when local obj is done
//    {
//        ptr = new F(std::move(func)); //func is moved 'cause local copy
//        deleter = function::template_deleter<F>; //function explicitly needed because static field
//        caller = function::template_caller<F>;
//    }
//    
//    ~function()
//    {
//        deleter(ptr);
//    }
//    
//    Return operator()(Params&&... params)
//    {
//        return caller(ptr, std::forward<Params>(params)...);
//    }
//    
//    private:
//    void *ptr;
//    void(*deleter)(void *); //function, that deletes *ptr in time
//    Return(*caller)(void *,Params&&...);
//    
//    template<typename F>
//    static Return template_caller(void *p, Params&&... params)
//    {
//        F* ptr_to_obj = static_cast<F*>(p);
//        return (*ptr_to_obj)(std::forward<Params>(params)...);
//    }
//    
//    template<typename F>
//    static void template_deleter(void *p)
//    {
//        delete static_cast<F*>(p);
//    }
//};
//
//int f(float fl)
//{
//    return static_cast<int>(fl * 2);
//}
//
//struct my_type
//{
//    my_type(){}
//    my_type(const my_type& rhs){}
//    
//    double operator()(int a)
//    {
//        return 3.1415;
//    }
//};
//
//int main()
//{
//    function<int(float)> m_f(f);
//    std::cout << m_f(2.3) << std::endl;
//    
//    my_type m_t;
//    function<double(int)> m_ff(std::move(m_t)); //(1)
//    std::cout << m_ff(3) << std::endl;
//    
//    function<void(std::string)> m_fff([](std::string str){std::cout << str << std::endl;});
//    m_fff("abc");
//}
//
//
//
//
//
////
////
////#include <iostream>
////
////using namespace std;
////class cl {
////    int i;
////public:
////    int get_i();
////    void put_i(int j);
////};
////inline int cl::get_i()
////{
////    return i;
////}
////inline void cl::put_i(int j)
////{
////    i = j;
////}
////int main()
////{
////    cl s;
////    
////    cout << s.get_i();
////    return 0;
////
////}
////void func();
////struct A {
////    A(){
////        func(this);
////    }
////    virtual void f() = 0;
////
////};
////void func( A *aa) {
////    aa->f();
////}
////
////int main() {
////    A *sm = new A();
////    cout << sm.*base;
////}
////
////
////
////class A;
////void func(A* pA);
////class A
////{
////public:
////    A()
////    {func(this);};
////    virtual void pureVirtual()=0;
////};
////
////class B : public A
////{
////public:
////    virtual void pureVirtual(){};
////};
////
////void func(A* pA)
////{
////    pA->pureVirtual();
////}
////
////int main() {
////    A * bb = new B;
////    bb->pureVirtual();
////    
////    return 0;
////}
////class CBase
////{
////private:
////    int privateBase;
////protected:
////    int protBase;
////public:
////    int pubBase;
////};
////
////class CDerived : public CBase
////{
////    //унаследованные данные класса
////    //недоступно:
////    // int privateBase;
////    //protected:
////    // int protBase;
////    //public:
////    // int pubBase;
////
////public:
////    void updateDerived()
////    {
////        //privateBase=0; //нельзя доступиться к
////        //private данным CBase
////        protBase=0;
////        pubBase=0;
////    }
////};
////
////class CDerived1 : public CDerived
////{
////public:
////    void updateDerived1()
////    {
////        //privateBase=1; //нельзя доступиться к
////        //private данным CBase
////        protBase=1;
////        pubBase=1;
////    }
////};
////
////int main() {
////    CDerived1 g;
////    g.updateDerived1();
////}
////
////
////struct BaseExc {
////    BaseExc(int a) {msg = a;};
////    virtual ~BaseExc() {};
////    int msg;
////    virtual void nameOut() {cout << "BaseExc " << msg << endl;}
////};
////
////struct DerivedExc : public BaseExc {
////    DerivedExc(int a) : BaseExc(a) {};
////    virtual ~DerivedExc() {};
////    void nameOut() {cout << "DerivedExc " << msg << endl;}
////};
////
////struct OneMoreExc : public BaseExc {
////    OneMoreExc(int a) : BaseExc(a) {};
////    virtual ~OneMoreExc() {};
////    void nameOut() {cout << "OneMoreExc " << msg << endl;};
////};
////
////int main() {
////    try {
////        try {
////            throw DerivedExc(5);
////        } catch (BaseExc &a) {
////            a.nameOut();
////            throw a;
////        }
////    }
////    catch (BaseExc &a)
////    {
////        a.nameOut();
////    }
////}
////int main() {
////    try {
////        throw 5;
////           } catch(int a) {
////               std::cout << a<<"\n";
////    }
////}
////
////
////int main() {
////    try {
////        try {
////            throw DerivedExc(5);
////        } catch (BaseExc &a) {
////            a.nameOut();
////            throw ;
////        }
////    }
////    catch (BaseExc &a)
////    {
////        a.nameOut();
////    }
////}
////class A { public: virtual void Foo() {cout << "Heh" << endl;} };
////class B : public virtual A {public : void Foo() {cout << "HehB" << endl;}};
////class C : public A {};
////class D : public B, public C {};
////
////int main() {
////    A * a = new B();
////    B * b = new B();
////    a -> Foo();
////    b -> Foo();
////    a = b;
////    a -> Foo();
////}
////
////struct Base {
////    Base() {cout << "Base" << endl;}
////    virtual ~Base() {cout << "~Base" << endl;}
////
////    virtual void foo() {cout << "Base -> foo" << endl;}
////    virtual void bar() {cout << "Base -> bar" << endl;}
////    void baz() {cout << "Base -> baz" << endl;}
////    //virtual void func() = 0;
////};
////
////struct Derived : public Base {
////    Derived() {cout << "Derived" << endl;}
////    virtual ~Derived() {cout << "~Derived" << endl;}
////
////    virtual void foo() {cout << "Derived -> foo" << endl;}
////    void bar() {cout << "Derived -> bar" << endl;}
////    void baz() {cout << "Derived -> baz" << endl;}
////    void func() {cout << "Derived -> func" << endl;}
////
////};
////
////struct Derived2 : public Derived {
////    Derived2() {cout << "Derived2" << endl;}
////    virtual ~Derived2() {cout << "~Derived2" << endl;}
////
////    virtual void foo() {cout << "Derived2 -> foo" << endl;}
////    void bar() {cout << "Derived2 -> bar" << endl;}
////    void baz() {cout << "Derived2 -> baz" << endl;}
////    void func() {cout << "Derived2 -> func" << endl;}
////
////};
////
////int main() {
////    Base * a = new Derived();
////    a -> foo();
////    a -> bar();
////    a -> baz();
////    //a -> func();
////
////    delete a;
////    cout << endl;
////
////    a = new Derived2();
////    a -> foo();
////    a -> bar();
////    a -> baz();
////    //a -> func();
////
////    delete a;
////    cout << endl;
////
////    Derived * b = new Derived2();
////    b -> foo();
////    b -> bar();
////    b -> baz();
////    //b -> func();
////
////    delete b;
////
////    return 0;
////}
////
////------------------------------------------------------------------------------------------------------------
////
////------------------------------------------------------------------------------------------------------------
////
////
////#include <iostream>
////
////using namespace std;
////
////struct A {
////    A() {cout << "inA" << endl;}
////    virtual ~A() {cout << "outA" << endl;}
////    virtual void foo() {cout << "A" << endl;};
////};
////
////struct B : virtual public A {
////    B() {cout << "inB" << endl;}
////    virtual ~B() {cout << "outB" << endl;}
////    //void foo() {cout << "B" << endl;}
////};
////
////struct C : virtual public A {
////    C() {cout << "inC" << endl;}
////    virtual ~C() {cout << "outC" << endl;}
////    void foo() {cout << "C" << endl;}
////};
////
////struct D : public B , public C {
////    D() {cout << "inD" << endl;}
////    virtual ~D() {cout << "outD" << endl;}
//////    void foo() {cout << "D" << endl;}
////};
////
////
////int main() {
////    A *d = new D();
////
////    d -> foo();
////
////    delete d;
////}