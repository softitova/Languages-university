#ifndef MY_OPT_VECTOR
#define MY_OPT_VECTOR


#include <vector>

using namespace std;

struct my_vector{
    std::vector<unsigned> data;
    size_t link_counter;
};

struct opt_vector{
public:
    union {
        my_vector *big_number;
        unsigned small_number;
    };
    size_t opt_size;
    bool is_big = false;
    void safe_delete();
    void make_alone();
    friend void swap(opt_vector &a, opt_vector &b);
public:
    opt_vector();
    opt_vector(size_t, unsigned);
    opt_vector(const opt_vector& other);

    ~opt_vector();
   


    opt_vector &operator=(opt_vector const &);
    unsigned& operator[](size_t);
    unsigned const& operator[](size_t) const;
    size_t size() const;
    
    unsigned &back();
    unsigned const &back() const;
    
    void resize(size_t);
    
    void push_back(unsigned);
    void pop_back();
    void clear();

    void push_front(unsigned);
    void all_reverse();
	bool empty();    
};




#endif