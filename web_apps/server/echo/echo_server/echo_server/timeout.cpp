#include "timeout.h"
#include <ctime>
#include <limits>
#include <iostream>

timeout::timeout() {}

timeout::q_it timeout::add(int timeout, timeout_wrapper wrapper)
{
    auto it = tl_connection_map.find(timeout);
    if (it == tl_connection_map.end())
    {
        list_ptr sh_ptr = std::make_shared<std::list<timeout_wrapper>>();
        it = tl_connection_map.insert({timeout, sh_ptr}).first;
        deadline_map.insert({time(NULL) + timeout, it->second});
    }
    it->second->push_back(wrapper);
    return --it->second->end();
}

timeout::q_it timeout::update_helper(list_ptr q, q_it it,  time_t deadline)
{
    timeout_wrapper new_wrapper = timeout_wrapper(it->timeout, deadline, it->fd, it->writer_);
    q->erase(it);
    q->push_back(new_wrapper);
    return --q->end();
}

timeout::q_it timeout::update(timeout::q_it it, time_t new_deadline)
{
    mm_it mm;
    auto t = multimap_iterators.find(it->timeout);
    if(t == multimap_iterators.end())
    {
        it->deadline = new_deadline;
        return add((int)it->timeout, *it);
    }
    mm = t->second;
    list_ptr q = mm->second;
    if(q->begin() == it)
    {
        q_it r = update_helper(q, it, new_deadline);
        deadline_map.erase(mm);
        multimap_iterators.erase(it->timeout);
        multimap_iterators.insert({it->timeout, deadline_map.insert({q->begin()->deadline, q})});
        return r;
    }
    return update_helper(q, it, new_deadline);
}

timeout::q_it timeout::get_min()
{
    return  deadline_map.begin()->second->begin();
}

timespec timeout::get_min_time()
{
    if(deadline_map.empty())
    {
        return {5,0};
    }
    return  {get_min()->timeout,0} ;
}

int timeout::get_min_fd()
{
    if(deadline_map.empty()) {
        return -1;
    }
    return get_min()->fd;
}


void timeout::remove(timeout::q_it it)
{
    it = update(it, std::numeric_limits<time_t>::max());
    list_ptr q = tl_connection_map.find(it->timeout)->second;
    if(q->size() != 1)
    {
        q->erase(it);
        return;
    }
    tl_connection_map.erase(it->timeout);
    deadline_map.erase(multimap_iterators.find(it->timeout)->second);
    multimap_iterators.erase(it->timeout);
}

timeout::~timeout() {}


