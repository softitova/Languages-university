//#ifndef TIMEOUT_H
//#define TIMEOUT_H
//
//#include <map>
//#include <unordered_map>
//#include <list>
//
//
//struct timeout {
//    
//public:
//    struct file_descriptor;
//    
//    struct timeout_wrapper {
//        
//        typedef std::shared_ptr<file_descriptor> fd_ptr;
//        time_t timeout;
//        time_t deadline;
//        fd_ptr fd_;
//        int fd;
//        
//        timeout_wrapper(time_t timeout_, time_t deadline_, int fd_, fd_ptr fd_new)
//        : timeout(timeout_ ), deadline(deadline_), fd(fd_), fd_(fd_new) {}
//    };
//    
//    typedef std::list<timeout_wrapper>::iterator timeout_iterator;
//    
//private:
//    
//    typedef std::multimap<time_t, std::shared_ptr<std::list<timeout_wrapper>>>::iterator mm_it;
//    typedef std::shared_ptr<std::list<timeout_wrapper>> list_ptr;
//    
//    std::unordered_map<time_t, mm_it> multimap_iterators;
//    std::multimap<time_t, list_ptr> deadline_map;
//    std::unordered_map<time_t, list_ptr> tl_connection_map;
//    std::unordered_map<timeout_iterator, std::function<void(struct kevent)>> handlers;
//    
//    timeout_iterator update_helper(list_ptr, timeout_iterator, time_t);
//    
//public:
//    
//    timeout();
//
//    
//    timeout_iterator add(time_t, timeout_wrapper, std::function<void(struct kevent)>);
//    timeout_iterator update(timeout_iterator, time_t);
//    
//    timeout_iterator get_min();
//    timespec get_min_time();
//    int get_min_fd();
//    
//    void remove(timeout_iterator);
//    
//  
//};
//
//
//#endif