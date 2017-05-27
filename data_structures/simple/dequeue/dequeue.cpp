#include <deque>
#include <string>
#include "dec.h"
#include <cassert>
#include <iostream>

dec::iterator::iterator() {}
dec::iterator::iterator (int index, dec &a) : a(a.a), head(a.head), pos(index), _capacity(&a._capacity) {}

dec::iterator::iterator (const iterator& other) : a(other.a), head(other.head), pos(other.pos), _capacity(other._capacity) {}

dec::iterator::~iterator () {}

void dec::iterator::swap(iterator &b)
{
    std::swap(a, b.a);
    std::swap(head, b.head);
    std::swap(pos, b.pos);
    std::swap(_capacity, b._capacity);
}

dec::iterator dec::iterator::operator=(iterator other)
{
    this->swap(other);
    return *this;
}

bool dec::iterator::operator==(const iterator &b)
{
    return a == b.a && head == b.head && pos == b.pos && _capacity == b._capacity;
}

bool dec::iterator::operator!=(const iterator &b)
{
  return !(*this == b);
}

dec::iterator& dec::iterator::operator+=(const int b)
{
    pos = (pos + b % *_capacity + *_capacity) % *_capacity;
    return *this;
}

dec::iterator operator+(dec::iterator a, const int b)
{
    return a += b;
}

dec::iterator dec::iterator::operator++(int)
{
    iterator temp = *this;
    ++*this;
    return temp;
}

dec::iterator& dec::iterator::operator++()
{
    return *this += 1;
}

dec::iterator& dec::iterator::operator-=(const int b)
{
    pos = (*_capacity - b % *_capacity + pos) % *_capacity;
    return *this;
}

dec::iterator operator-(dec::iterator a, const int b)
{
    return a -= b;
}

int operator-(dec::iterator a, dec::iterator b)
{
     return (int)a.pos - (int)b.pos;
}

dec::iterator dec::iterator::operator--(int)
{
    iterator x = *this;
    --*this;
    return x;
}

dec::iterator& dec::iterator::operator--()
{
    return *this -= 1;
}

std::string& dec::iterator::operator*()// const - return const string, other the same
{
//    std::cout << (head - a) << ' ' << pos << "\n";
//    std::cout << (head - a) << ' ' << pos << "\n";
//    std::cout << (a - head + pos) % *_capacity << "\n";
    return *((head - a + pos) % *_capacity + a);
}

bool dec::iterator::operator>(iterator b)
{
        return pos > b.pos;
}

bool dec::iterator::operator>=(iterator b)
{
        return pos >= b.pos;
}

bool dec::iterator::operator<(iterator b)
{
        return pos < b.pos;
}

bool dec::iterator::operator<=(iterator b)
{
        return pos <= b.pos;
}


dec::const_iterator::const_iterator() {}

dec::const_iterator::const_iterator (int index, const dec &a) : a(a.a), head(a.head), pos(index), _capacity(&a._capacity) {}

dec::const_iterator::const_iterator (const const_iterator& other) : a(other.a), head(other.head), pos(other.pos), _capacity(other._capacity) {}

dec::const_iterator::~const_iterator () {}

void dec::const_iterator::swap(const_iterator &b)
{
    std::swap(a, b.a);
    std::swap(head, b.head);
    std::swap(pos, b.pos);
    std::swap(_capacity, b._capacity);
}

dec::const_iterator dec::const_iterator::operator=(const_iterator other)
{
    this->swap(other);
    return *this;
}

bool dec::const_iterator::operator==(const const_iterator &b)
{
    return a == b.a && head == b.head && pos == b.pos && _capacity == b._capacity;
}

bool dec::const_iterator::operator!=(const const_iterator &b)
{
  return !(*this == b);
}

dec::const_iterator& dec::const_iterator::operator+=(const int b)
{
     pos = (pos + b % *_capacity + *_capacity) % *_capacity;
    return *this;
}

dec::const_iterator operator+(dec::const_iterator a, const int b)
{
    return a += b;
}

dec::const_iterator dec::const_iterator::operator++(int)
{
    const_iterator temp = *this;
    ++*this;
    return temp;
}

dec::const_iterator& dec::const_iterator::operator++()
{
    return *this += 1;
}

dec::const_iterator& dec::const_iterator::operator-=(const int b)
{
    pos = (*_capacity - b % *_capacity + pos) % *_capacity;
    return *this;
}

dec::const_iterator operator-(dec::const_iterator a, const int b)
{
    return a -= b;
}

int operator-(dec::const_iterator a, dec::const_iterator b)
{
     return a.pos - b.pos;
}

dec::const_iterator dec::const_iterator::operator--(int)
{
    const_iterator x = *this;
    --*this;
    return x;
}

dec::const_iterator& dec::const_iterator::operator--()
{
    return *this -= 1;
}

const std::string& dec::const_iterator::operator*()// const - return const string, other the same
{
    return *((a - head + pos) % *_capacity + a);
}

bool dec::const_iterator::operator>(const_iterator b)
{
        return pos > b.pos;
}

bool dec::const_iterator::operator>=(const_iterator b)
{
        return pos >= b.pos;
}

bool dec::const_iterator::operator<(const_iterator b)
{
        return pos < b.pos;
}

bool dec::const_iterator::operator<=(const_iterator b)
{
        return pos <= b.pos;
}


dec::dec() : dec(10) {}

dec::dec(size_t capacity) :
_capacity(capacity), _size(0),
 a(((std::string*) operator new(_capacity * sizeof(std::string)))), head(a) {}

void swap(dec &lhs, dec &rhs)
{
    std::swap(lhs.a, rhs.a);
    std::swap(lhs._capacity, rhs._capacity);
    std::swap(lhs._size, rhs._size);
    std::swap(lhs.head, rhs.head);
}

dec::dec(dec const & other) : dec(other.capacity())
{
    for (auto x : other)
    {
        push_back(x);
    }
}

dec::~dec()
{
    while (!empty())
        pop_back();
    head = a;
    operator delete (a);
}

dec& dec::operator=(dec other)
{
    swap(*this, other);
    return *this;
}

void emplace_capacity(dec &x)
{
    dec buf(x._capacity * 2);
    for (size_t pos = 0; pos < x._size; pos++)
    {
        buf.push_back(x[pos]);
    }
    swap(buf, x);
}

std::string const& dec::back() const
{
    return *--end();
}

std::string const& dec::front() const
{
    return *begin();
}

std::string & dec::back()
{
    return *--end();
}

std::string & dec::front()
{
    return *begin();
}

dec::iterator dec::begin()
{
     return iterator(0, *this);
}

dec::iterator dec::end()
{
    return iterator(_size, *this);
}


dec::const_iterator dec::begin() const
{
     return const_iterator(0, *this);
}

dec::const_iterator dec::end() const
{
    return const_iterator(_size, *this);
}


bool dec::empty() const
{
    return _size == 0;
}

size_t dec::size() const
{
     return _size;
}

size_t dec::capacity() const
{
    return _capacity;
}

std::string const& dec::operator[](size_t pos) const
{
    return a[(head - a + pos) % _capacity];
}

std::string& dec::operator[](size_t pos)
{
    return a[(head - a + pos) % _capacity];
}


dec::iterator dec::erase(iterator first, iterator last)
{
    int p = first - begin();
    int f = last - begin();
    int l = f - p;
    if (p >= _size - f)
    {
        for (int i = p; i + l < _size; i++)
        {
            swap((*this)[i], (*this)[i + l]);
        }
        while (l)
        {
            pop_back();
            l--;
        }
    }
    else
    {
        for (int i = f - 1; i >= l; i--)
        {
            swap((*this)[i], (*this)[i - l]);
        }
        while (l)
        {
            pop_front();
            l--;
        }
    }
    return begin() + p;
}

dec::iterator dec::erase(iterator pos)
{
    if (pos == end())
        return end();
    int x = pos - begin();
    assert(!empty());
    if (pos - begin() >= end() - pos)
    {
//    std::cout << "Here\n";
        while (pos != end())
        {
            iterator cur = pos++;
            if (pos == end())
            {
//                std::cout << "Op\n";
                (*cur).std::string::~string();
            }
            else
            {
                swap(*cur, *pos);
            }
        }
    }
    else
    {
//        std::cout << "This\n";
        while (pos != begin())
        {
            iterator cur = pos--;
            swap(*cur, *pos);
        }
        (*pos).std::string::~string();
        head = a + (head - a + 1) % _capacity;
//        std::cout << head << ' ' << a << "\n";
    }
    _size--;
    return begin() + x;
}

dec::iterator dec::insert(iterator position, size_t l, std::string const & x)
{
    int p = position - begin();
    int f = p + l;
    if (p >= _size - f)
    {
        for (int i = 0; i < f - p; i++)
        {
            push_back(x);
        }
        for (size_t i = _size - 1; i - l >= p; i--)
        {
            swap((*this)[i], (*this)[i - l]);
        }
    }
    else
    {
        for (int i = 0; i < f - p; i++)
        {
            push_front(x);
        }
        for (size_t i = 0; i + l < f; i++)
        {
            swap((*this)[i], (*this)[i + l]);
        }
    }
    return begin() + f;
}

dec::iterator dec::insert(iterator position, std::string const & x)
{
    if (_size == _capacity - 1)
    {
        int d = position - begin();
        emplace_capacity(*this);
        position = begin() + d;
    }
    if (position - begin() >= end() - position)
    {
        new (a + (head - a + _size) % _capacity) std::string(x);
        for (auto x = end(); x > position;)
        {
            auto prev = x--;
            swap(*prev, *x);
        }
    }
    else
    {
        head--;
        if (head < a)
            head = a + _capacity - 1;
        new (head) std::string(x);
        for (auto x = begin(); x < position;)
        {
            auto prev = x++;
            swap(*prev, *x);
        }
    }
    _size++;
    return position;
}

void dec::push_back(std::string const &value)
{
    dec::insert(end(), value);
}

void dec::pop_back()
{
    dec::erase(--end());
}

void dec::push_front(std::string const& value)
{
    dec::insert(begin(), value);
}

void dec::pop_front()
{
    dec::erase(begin());
}

void dec::clear()
{
    dec empty_dec;
    swap(*this, empty_dec);
}

int main()
{
//{
//    dec y;
//    y.push_back("1");
//    y.push_back("2");
//    y.push_back("3");
//    y.erase(y.begin(), y.end());
//    for (auto x : y)
//        std::cout << x << "\n";
//}
{
    dec y;
    y.push_back("1");
    y.push_back("2");
//    y.push_back("3");
    //    y.pop_front();
    for (auto x : y)
        std::cout << x << ' ';
    std::cout << "\n";

}
//return 0;
//return 0;
//return 0;

{
    dec y;
    y.insert(y.begin(), 10, "1");
}
//return 0;
{
    dec y;
    std::deque<std::string> x;
    for (int i = 0; i < 200; i++)
    {
        std::string s = "";
        s += ('0' + rand() % 40);
        int pos = rand() % (y.size() + 1);
        int kol = rand() % 20 + 15;
        x.insert(x.begin() + pos, kol, s);
        y.insert(y.begin() + pos, kol, s);
//        return 0;
    }
    for (int i = 0; i < 100; i++)
    {
        int pos = rand() % (y.size() + 1);
        int kol = rand() % 15 + 1 + pos;
        x.erase(x.begin() + pos, x.begin() + kol);
        y.erase(y.begin() + pos, y.begin() + kol);
    }
    assert(x.size() == y.size());
    for (int i = 0; i < x.size(); i++)
        assert(x[i] == y[i]);
}

//return 0;
{
    std::deque<std::string> x;
    dec y;
    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            std::string s = "";
            s += ('0' + rand() % 40);
            int pos = rand() % (y.size() + 1);
            x.insert(x.begin() + pos, s);
            y.insert(y.begin() + pos, s);
        }

//        for (int j = 0; j < y.size(); j++)
//            std::cout << y[j] << ' ' << x[j] << "\n";
//        std::cout << "\n";
        for (int j = 0; j < y.size(); j++)
            assert(y[j] == x[j]);
        for (int j = 0; j < 5; j++)
        {
            int pos = rand() % (y.size());
//            std::cout << y.size() << ' ' << pos << "\n";
            y.erase(y.begin() + pos);
            x.erase(x.begin() + pos);
//        for (int j = 0; j < y.size(); j++)
//            std::cout << y[j] << ' ' << x[j] << "\n";
//        std::cout << "\n";
        }
//        for (int j = 0; j < y.size(); j++)
//            assert(y[j] == x[j]);
//        return 0;
    }

}
//return 0;
{
    const int N = 20;
    dec y;
    for (int i = 0; i < N; i++)
    {
        std::string s = "";
        s += ('0' + i % 10);
//        std::cout << s << "\n";
        y.push_front(s);
    }
//    y.clear();
    for (int i = 0; i < 5; i++)
        y.pop_front();
        for (int i = 0; i < y.size(); i++)
            std::cout << y[i] << ' ';
        std::cout << "\n";
}
//return 0;
//return 0;
    {
        std::deque<std::string> x;
        dec y;
        auto xx = x.begin();
        auto yy = y.begin();
        for (int i = 0; i < 100; i++)
        {
            std::string s = "";
            s += ('0' + rand() % 10);
            yy = y.insert(yy, s);
            xx = x.insert(xx, s);
        }
        int qq = rand() % 10;
        xx = x.begin() + qq;
        yy = y.begin() + qq;
        for (int i = 0; i < 50; i++)
        {
            xx = x.erase(xx);
            yy = y.erase(yy);
        }
        assert(x.size() == y.size());
        for (int j = 0; j < x.size(); j++)
            assert(y[j] == x[j]);
    }

//return 0;
    for (int i = 0; i < 200; i++)
    {
        std::deque<std::string> x;
        dec y;
        for (int j = 0; j < 1000; j++)
        {
            std::string s = "";
            s += ('0' + rand() % 10);
            int pos = rand() % (y.size() + 1);
            int q = y.insert(y.begin() + pos, s) - y.begin();
            int z = x.insert(x.begin() + pos, s) - x.begin();
//            if (z < 0)
//            {
//                std::cout << "Ops\n";
//            }
//assert(z <= 0);
//            assert(q == z);
        }
        for (int j = 0; j < 500; j++)
        {
            int pos = rand() % (y.size());
            y.erase(y.begin() + pos);
            x.erase(x.begin() + pos);
        }
        for (int j = 0; j < 500; j++)
            assert(y[j] == x[j]);
    }
    std::cout << "All tests passed\n";
}

