//#include <vector>
//#include <cstdio>
//#include <fstream>
//
//
//struct signed_numb{};
//struct unsigned_numb{};
//template <typename T>
//bool checked(T value);
//
//template <typename T>
//struct number_traits {};
//
//template <typename T>
//struct number_traits<unsigned int> {
//    typedef unsigned_numb tag;
//};
//
//template <typename T>
//struct number_traits<int> {
//    typedef signed_numb tag;
//};
//
//template <typename T>
//bool checked_impl(T, signed_numb) {
//    if(std::is_same<T,int>::value) {
//        
//    } else if(std::is_same<T,double>::value) {
//        
//        
//    } else if(std::is_same<T,long>::value) {
//        
//    } else if(std::is_same<T,float>::value) {
//        
//    } else if(std::is_same<T,short>::value) {
//        
//    }
//    return true;
//}
//
//template <typename T>
//bool checked_impl(T, unsigned_numb) {
//    if(std::is_same<T,unsigned int>::value) {
//        
//    } else if(std::is_same<T,unsigned long>::value) {
//        
//    } else if(std::is_same<T,unsigned short>::value) {
//        
//    }
//    return true;
//
//}
//
//
//
//template <typename T>
//bool checked(T value) {
//    return checked_impl(T, typename number_traits<T>::tag());
//}

//using namespace std;
//
//template <typename... Smth>
//struct function;
//
//template <typename R, typename... Arg_t>
//struct function <R(Arg_t...)>
//{
//    function():valid(false), ptr(nullptr), deleter(nullptr), caller(nullptr), copier(nullptr){}
//
//    function(const function& f):valid(true), ptr(f.copy()), deleter(f.deleter), caller(f.caller), copier(f.copier){}
//
//    function(function&& f){
//        make_clear();
//        swap(*this, f);
//    }
//
//    template <typename F>
//    function(F _func):valid(true), ptr(new F(std::move(_func))), deleter(delete_obj<F>), caller(call_obj<F>), copier(copy_obj<F>){}
//
//    function& operator=(const function& another){
//        void* tmp;
//        tmp = copier(another.ptr); //another.copy();
//        make_clear();
//        ptr = tmp;
//        valid = true;
//        caller = another.caller;
//        deleter = another.deleter;
//        copier = another.copier;
//        return *this;
//    }
//
//    function& operator=(function&& another){
//        make_clear();
//        swap(*this, another);
//        return *this;
//    }
//
//    R operator()(Arg_t&&... args) const{
//        return caller(ptr, std::forward <Arg_t> (args)...);
//    }
//
//    ~function(){
//        make_clear();
//    }
//
//    void swap_in(function& other) {
//        swap(*this, other);
//    }
//
//    explicit operator bool() const{
//        return valid;
//    }
//
//
//
//private:
//
//    template <typename T>
//    static R call_obj(void* func, Arg_t... args){
//        return  (*static_cast <T*> (func))(std::forward<Arg_t>(args)...);
//    }
//
//    template <typename T>
//    static void* copy_obj(void* func){
//        return new T(*static_cast <T*> (func));
//    }
//
//    template <typename T>
//    static void delete_obj(void* func){
//        delete static_cast<T*>(func);
//    }
//
//
//    void make_clear(){
//
//        if (valid) {
//            deleter(ptr);
//            ptr = nullptr;
//            deleter = nullptr;
//            caller = nullptr;
//            copier = nullptr;
//            valid = false;
//        }
//    }
//
//    void* ptr;
//    bool valid;
//
//    void (*deleter)(void*);
//    R (*caller)(void*, Arg_t...);
//    void* (*copier)(void*);
//
//    template <typename R_t, typename... Args_t>
//    friend void swap(function<R_t(Args_t...)>& a, function<R_t(Args_t...)>& b);
//};
//
//template <typename R, typename... Args_t>
//void swap(function<R(Args_t...)>& a, function<R(Args_t...)>& b){
//
//    std::swap(a.valid, b.valid);
//    std::swap(a.deleter, b.deleter);
//    std::swap(a.caller, b.caller);
//    std::swap(a.copier, b.copier);
//    std::swap(a.ptr, b.ptr);
//}
