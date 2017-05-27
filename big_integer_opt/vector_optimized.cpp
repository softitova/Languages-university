#include "vector_optimized.h"
#include <iostream>
#include <cassert>
#include <bits/stdc++.h>

opt_vector::opt_vector() : small_number(0), opt_size(0), is_big(false) {}

opt_vector::opt_vector(const opt_vector& other) : opt_size(other.opt_size), 
												is_big(other.is_big)
												 {
    if(!other.is_big) {
        small_number = other.small_number;
    } else {
        other.big_number->link_counter++;
        big_number = other.big_number;
    }
}

opt_vector::opt_vector(size_t sz, unsigned val) : opt_size(sz) {
    if(sz>1) {
        is_big = true;
        big_number = new my_vector();
        big_number->data.push_back(val);
        big_number->data.resize(sz);
        big_number->link_counter = 1;
    } else {
    	is_big = false;
        small_number = val;
    }
}
void opt_vector::safe_delete() {
    if(big_number->link_counter > 1) {
        big_number->link_counter--;
    } else {
        delete big_number;
    }
}

void opt_vector::make_alone(){
    if(is_big && big_number->link_counter>1) {
        my_vector* new_vector = new my_vector();
        big_number->link_counter--;
        new_vector->data = big_number->data;
        new_vector->link_counter = 1;
        std::swap(new_vector, big_number);
    }
}

opt_vector::~opt_vector() {
    if(is_big && big_number -> link_counter > 0) {
        safe_delete();
    }
}

void swap(opt_vector& a, opt_vector& b) {
    swap(a.is_big, b.is_big);
    swap(a.opt_size, b.opt_size);
    swap(a.big_number, b.big_number);
}

opt_vector &opt_vector::operator=(opt_vector const &other) {
    opt_vector r(other);
    swap(*this, r);
    return *this;
}

unsigned& opt_vector::operator[](size_t index) {
    assert(index<opt_size);
    make_alone();
    return (is_big) ? big_number->data[index] : small_number;
}

unsigned const& opt_vector::operator[](size_t index) const {
    assert(index<opt_size);
    return (is_big) ? big_number->data[index] : small_number;
}

size_t opt_vector::size() const {
    return opt_size;
}

unsigned &opt_vector::back() {
    make_alone();
    return (is_big) ? big_number->data.back() : small_number;
}

unsigned const &opt_vector::back() const  {
    return (is_big) ? big_number->data.back() : small_number;
}

void opt_vector::resize(size_t new_size) {
    if (is_big) {
        make_alone();
        big_number -> data.resize(new_size);
    } else if(new_size > 1){
        unsigned temp = small_number;
        big_number = new my_vector();
        big_number -> data.resize(new_size, 0);
        big_number -> link_counter = 1;
        if (opt_size != 0)
            big_number -> data[0] = temp;
        is_big = true;
    } else if (opt_size == 0) {
        small_number = 0;
    }
    opt_size = new_size;
}


void opt_vector::push_back(unsigned a) {
    if (is_big) {
        make_alone();
        big_number -> data.push_back(a);
    } else if (opt_size == 1){
        unsigned temp = small_number;
        big_number = new my_vector();
        big_number -> data.emplace_back(temp);
        big_number -> data.emplace_back(a);
        big_number->link_counter = 1;
        is_big = true;
    } else {
        small_number = a;
    }
    opt_size++;
}

void opt_vector::pop_back() {
    if (is_big) {
	    make_alone();
        big_number -> data.pop_back();
    } else if (opt_size == 0) {
        throw "Zero pop back";
    }
    opt_size--;
}


void opt_vector::clear() {
    if (is_big) {
        make_alone();
        big_number -> data.clear();
    }
    opt_size = 0;
}

void opt_vector::push_front(unsigned x) {
    if (!is_big) {
        make_alone();
	    push_back(x);
        all_reverse();
    } else {
        big_number->data.insert(big_number->data.begin(), x);
        opt_size++;
    }
}

void opt_vector::all_reverse() {
    if (is_big) {
        make_alone();
	    std::reverse(big_number->data.begin(), big_number->data.end());
    }
}

bool opt_vector::empty() {
    return opt_size == 0;
}

// int main () {
// srand(time(0));
//     opt_vector a;
//     vector<unsigned> x;
//     a.resize(4);
//     cout << a.is_big << "\n";
//     cout << a.big_number->link_counter << "\n";
//     // for (int i = 1; i <= 200; i++) {

//     //     int op = rand() % 6;
//     //     // cout << op;
//     //     if (op == 1) {
//     //         unsigned y = rand();
//     //         a.push_back(y);
//     //         x.push_back(y);
//     //     } else if (op == 0) {
//     //         unsigned y = rand();
//     //         a.push_front(y);
//     //         x.insert(x.begin(), y);
//     //     } else if (op == 2) {
//     //         int y = rand() % 20 ;
//     //         a.resize(y);
//     //         x.resize(y);
//     //     } else if (op == 4) {
//     //         a.clear();
//     //         x.clear();
//     //     } else if (op == 3) {
//     //         a.all_reverse();
//     //         reverse(x.begin(), x.end());
//     //     } else {
//     //     	opt_vector qwe = a;
//     //     	qwe.push_back(1);
//     //     } 
//     //     if (a.size() != x.size()) {
//     //         cout << "Bad\n";
//     //         return 0;
//     //     }
//     //     for (int i = 0; i < a.size(); i++)
//     //         if (a[i] != x[i]) {
//     //             cout << "Bad\n";
//     //             return 0;
//     //         }
//     // }
//     // cout << "\n";
// }
