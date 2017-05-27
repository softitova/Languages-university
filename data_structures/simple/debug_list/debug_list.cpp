#include <iostream>
#include <list>
#include <cstdlib>
#include <cstdio>
#include <memory.h>
#include <cassert>
#include "list.h"


list::node::node (const std::string &key) : key(key), prev(this), next(this) {}
list::node::node (const std::string &key, node* prev, node* next) : key(key), prev(prev), next(next) {}
list::node::node (const std::string &key, node const &other) : node(key, other.prev, other.next) {}
list::node::node (const node& other) : node(other.key, other.prev, other.next) {}
list::node::~node() {}

bool list::node::operator==(const node& b)
{
    return key == b.key && prev == b.prev && next == b.next;
}

list::iterator::iterator (list* obj, const std::string &key = "") : cur(new node(key)), my_object(obj), flag(true)
{
    my_object->v.push_back(this);
}
list::iterator::iterator (node& other, list* obj) : cur(&other), my_object(obj), flag(true)
{
    my_object->v.push_back(this);
}
list::iterator::iterator (const std::string &key, const iterator &other) : cur(new node(key, *other.cur)), my_object(other.my_object), flag(true)
{
    my_object->v.push_back(this);
}
list::iterator::iterator (const iterator& other) : cur(other.cur), my_object(other.my_object), flag(true)
{
    my_object->v.push_back(this);
}
list::iterator::iterator (const std::string &key, const iterator &prev, const iterator &next, list *obj) : cur(new node(key, prev.cur, next.cur)), my_object(obj), flag(true)
{
    my_object->v.push_back(this);
}
list::iterator::~iterator ()
{
    for (size_t it = 0; it < my_object->v.size(); it++)
    {
        if (my_object->v[it] == this)
        {
            my_object->v.erase(my_object->v.begin() + it);
            it--;
        }
    }
}


void list::iterator::swap(iterator &b)
{
    std::swap(cur, b.cur);
    std::swap(my_object, b.my_object);
    std::swap(flag, b.flag);
}

list::iterator list::iterator::operator=(iterator other)
{
    this->swap(other);
    return *this;
}


bool list::iterator::operator==(const iterator &b)
{
    return cur == b.cur && my_object == b.my_object && flag == b.flag;
}

bool list::iterator::operator!=(const iterator &b)
{
    return cur != b.cur || my_object != b.my_object || flag != b.flag;
}

list::iterator& list::iterator::operator++()
{
    assert(flag && *this != my_object->end() && "Operator ++");
    cur = cur->next;
    return *this;
}
list::iterator list::iterator::operator++(int)
{
    iterator cp = *this;
    ++*this;
    return cp;
}
list::iterator& list::iterator::operator--()
{
    assert(flag && *this != my_object->begin() && "Operator --");
    cur = cur->prev;
    return *this;
}
list::iterator list::iterator::operator--(int)
{
    iterator cp = *this;
    --*this;
    return cp;
}
std::string& list::iterator::operator*()
{
    assert(flag);
    return cur->key;
}


list::const_iterator::const_iterator (const list* obj, const std::string &key) : cur(new node(key)), my_object(obj), flag(true)
{
    my_object->cv.push_back(this);
}
list::const_iterator::const_iterator (node& other, const list* obj) : cur(&other), my_object(obj), flag(true)
{
    my_object->cv.push_back(this);
}
list::const_iterator::const_iterator (const std::string &key, const const_iterator &other) : cur(new node(key, *other.cur)), my_object(other.my_object), flag(true)
{
    my_object->cv.push_back(this);
}
list::const_iterator::const_iterator (const const_iterator& other) : cur(other.cur), my_object(other.my_object), flag(true)
{
    my_object->cv.push_back(this);
}
list::const_iterator::const_iterator (const std::string &key, const const_iterator &prev, const const_iterator &next, const list *obj) : cur(new node(key, prev.cur, next.cur)), my_object(obj), flag(true)
{
    my_object->cv.push_back(this);
}
list::const_iterator::~const_iterator ()
{
    for (size_t it = 0; it < my_object->cv.size(); it++)
    {
        if (my_object->cv[it] == this)
        {
            my_object->cv.erase(my_object->cv.begin() + it);
            it--;
        }
    }
}


void list::const_iterator::swap(const_iterator &b)
{
    std::swap(cur, b.cur);
}

list::const_iterator list::const_iterator::operator=(const_iterator other)
{
    this->cur = other.cur;
    return *this;
}

bool list::const_iterator::operator==(const const_iterator &b)
{
    return cur == b.cur;
}

bool list::const_iterator::operator!=(const const_iterator &b)
{
    return cur != b.cur;
}

list::const_iterator& list::const_iterator::operator++()
{
    cur = cur->next;
    return *this;
}
list::const_iterator list::const_iterator::operator++(int)
{
    const_iterator cp = *this;
    ++*this;
    return cp;
}
list::const_iterator& list::const_iterator::operator--()
{
    cur = cur->prev;
    return *this;
}
list::const_iterator list::const_iterator::operator--(int)
{
    const_iterator cp = *this;
    --*this;
    return cp;
}
const std::string& list::const_iterator::operator*()
{
    return cur->key;
}

list::list() : head(this), tail(head) {
}

list::list(list const &my_list) : list()
{
    list other;
    for (auto x : my_list)
        other.push_back(x);
    this->swap(other);
}

list::~list()
{
    clear();
}

list& list::operator=(const list & s)
{
    list(s).swap(*this);
    return *this;
}


void list::swap(list &b)
{
    head.swap(b.head);
    tail.swap(b.tail);
}

list::iterator list::begin()
{
    return head;
}

list::const_iterator list::begin() const
{
    return list::const_iterator(*head.cur, this);
}

list::iterator list::end()
{
    return tail;
}

list::const_iterator list::end() const
{
    return list::const_iterator(*tail.cur, this);
}

list::iterator list::insert(iterator pr, std::string const &s)
{
    assert(pr.flag && pr.my_object == this);
    if (empty())
    {
        head.cur = new node(s, *tail.cur);
        tail.cur->prev = tail.cur->next = head.cur;
        return head;
    }
    else if (pr == begin())
    {
        iterator temp(s, iterator(this), pr, this);
        pr.cur->prev = temp.cur;
        head.cur = temp.cur;
        return temp;
    }
    else
    {
        iterator pos = pr--;
        iterator temp(s, pr, pos, this);
        pos.cur->prev = pr.cur->next = temp.cur;
        return temp;
    }
}

list::iterator list::erase(iterator pos)
{
    assert(pos.flag && pos.my_object == this && pos != end());
    for (auto &x : v)
    {
        if (x->cur == pos.cur)
        {
            x->flag = false;
        }
    }
    iterator x = pos;
    x++;
    assert(!empty());
    if (*head.cur->next == *tail)
    {
        delete head.cur;
        tail.cur->next = tail.cur->prev = head.cur = tail.cur;
    }
    else
    {
        pos.cur->prev->next = pos.cur->next;
        pos.cur->next->prev = pos.cur->prev;
        if (head == pos)
        {
            head.cur = pos.cur->next;
        }
        delete pos.cur;
    }
    return x;
}

void list::push_front(std::string const & s)
{
    insert(head, s);
}

void list::pop_front()
{
    assert(!empty());
    erase(head);
}

void list::push_back(std::string const & s)
{
    insert(tail, s);
}

void list::pop_back()
{
    assert(!empty());
    erase(--end());
}

std::string list::front()
{
    assert(!empty());
    return *begin();
}
std::string const & list::front() const
{
    assert(!empty());
    return *begin();
}

std::string list::back()
{
    assert(!empty());
    return *(--end());
}
std::string const& list::back() const
{
    assert(!empty());
    return *(--end());
}

bool list::empty() const
{
    std::string x = *begin();
    return (begin() == end());
}

void list::splice(iterator pos, list &other, iterator first, iterator last)
{
    assert(pos.flag && first.flag && last.flag);
    assert(pos.my_object == this && first.my_object == &other && last.my_object == &other);
    bool order = false;
    for (auto x = other.begin(); x != other.end(); x++)
    {
        if (x == first)
        {
            order = true;
        }
        if (x == last)
        {
            assert(order);
            order = false;
        }
    }

    iterator x = first;
    iterator y = last;
    x--; y++;
    x.cur->next = y.cur;
    y.cur->prev = x.cur;

    x = pos++;
    x.cur->next = first.cur;
    pos.cur->prev = last.cur;
    last.cur->next = pos.cur;
    first.cur->prev = x.cur;
}

void list::clear()
{
    while (!empty())
    {
        pop_back();
    }
}

int main()
{
    {
//        std::list<std::string> a;
        list b;
//        a.insert(a.begin(), "1");
//        a.insert(a.begin(), "2");
        b.insert(b.begin(), "");
//        b.insert(b.begin(), "2");
//        for (auto x : b)
//            std::cout << x << ' ';
        assert(!b.empty());
    }
//    return 0;
    {
        std::list<std::string> a;
        list b;
        for (int i = 0; i < 100; i++)
        {
            std::string d;
            d += i + 48;
            int x = rand() % (a.size() + 1);
            a.insert(a.end(), d);
            b.insert(b.end(), d);
        }
        auto aa = a.begin();
        auto bb = b.begin();
        while (aa != a.end() && bb != b.end())
        {
            assert(*aa == *bb);
            aa++;
            bb++;
        }
        assert(aa == a.end() && bb == b.end());
        a.clear();
        b.clear();
        assert(b.empty());
    }
    {
        //std::list<std::string> a;
        list b;
        b.insert(b.begin(), "122");
        iterator aa = b.begin();
        aa--;
    }
}
