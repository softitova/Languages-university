#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <map>
#include <unordered_map>
#include <list>

struct timeout {
    
public:
    struct socket{
        struct writer;
    };
    struct timeout_wrapper {
        
        typedef std::shared_ptr<socket::writer> writer_ptr;
        time_t timeout;
        time_t deadline;
        writer_ptr writer_;
        int fd;
        
        timeout_wrapper(time_t timeout_, time_t deadline_, int fd_, writer_ptr new_writer_)
        : timeout(timeout_ ), deadline(deadline_), fd(fd_), writer_(new_writer_) {}
    };
    
    typedef std::list<timeout_wrapper>::iterator q_it;

private:
    
    typedef std::multimap<time_t, std::shared_ptr<std::list<timeout_wrapper>>>::iterator mm_it;
    typedef std::shared_ptr<std::list<timeout_wrapper>> list_ptr;

    std::unordered_map<time_t, mm_it> multimap_iterators;
    std::multimap<time_t, list_ptr> deadline_map;
    std::unordered_map<time_t, list_ptr> tl_connection_map;

    q_it update_helper(list_ptr, q_it, time_t);
    
public:
    
    timeout();
    
    q_it add(int, timeout_wrapper);
    q_it update(q_it, time_t);
    
    q_it get_min();
    timespec get_min_time();
    int get_min_fd();
    
    void remove(q_it);

    ~timeout();
};


#endif