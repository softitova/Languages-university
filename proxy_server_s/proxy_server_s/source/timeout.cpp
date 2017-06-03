//#include "timeout.h"
//#include <ctime>
//#include <limits>
//#include <iostream>
//
//timeout::timeout() {}
//
//timeout::timeout_iterator timeout::add(time_t timeout, timeout_wrapper wrapper, std::function<void(struct kevent)> handler)
//{
//    auto it = tl_connection_map.find(timeout);
//    if (it == tl_connection_map.end())
//    {
//        
//        list_ptr sh_ptr = std::make_shared<std::list<timeout_wrapper>>();
//        it = tl_connection_map.insert({timeout, sh_ptr}).first;
//        deadline_map.insert({time(NULL) + timeout, it->second});
//    }
//    it->second->push_back(wrapper);
//    handlers.insert({--it->second->end() ,handler}); //  добавили хэндлер на удаление
//    return it->second->end();
//}
//
//timeout::timeout_iterator timeout::update_helper(list_ptr q, timeout_iterator it,  time_t deadline)
//{
//    timeout_wrapper new_wrapper = timeout_wrapper(it->timeout, deadline, it->fd, it->fd_);
//    q->erase(it);
//    q->push_back(new_wrapper);
//    return --q->end();
//}
//
//timeout::timeout_iterator timeout::update(timeout::timeout_iterator it, time_t new_deadline)
//{
//    mm_it mm;
//    auto t = multimap_iterators.find(it->timeout);
//    if(t == multimap_iterators.end())
//    {
//        it->deadline = new_deadline;
//        return add((int)it->timeout, *it, handlers.find(it).first);
//    }
//    mm = t->second;
//    list_ptr q = mm->second;
//    if(q->begin() == it)
//    {
//        timeout_iterator r = update_helper(q, it, new_deadline);
//        deadline_map.erase(mm);
//        multimap_iterators.erase(it->timeout);
//        multimap_iterators.insert({it->timeout, deadline_map.insert({q->begin()->deadline, q})});
//        return r;
//    }
//    return update_helper(q, it, new_deadline);
//}
//
//timeout::timeout_iterator timeout::get_min()
//{
//
//    return  deadline_map.begin()->second->begin();
//}
//
//timespec timeout::get_min_time()
//{
//    if(deadline_map.empty())
//    {
//        return {5,0};
//    }
//    return  {get_min()->deadline - time(NULL), 0} ;
//}
//
//int timeout::get_min_fd()
//{
//    if(deadline_map.empty())
//    {
//        return -1;
//    }
//    return get_min()->fd;
//}
//
//
//void timeout::remove(timeout::timeout_iterator it)
//{
//    std::function<void(int)> f = handlers.at(it);
//    f(it->fd);
//    it = update(it, std::numeric_limits<time_t>::max());
//    list_ptr q = tl_connection_map.find(it->timeout)->second;
//    if(q->size() != 1)
//    {
//        q->erase(it);
//        return;
//    }
//    tl_connection_map.erase(it->timeout);
//    deadline_map.erase(multimap_iterators.find(it->timeout)->second);
//    multimap_iterators.erase(it->timeout);
//}
//
//
//
