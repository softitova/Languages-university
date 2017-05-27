#ifndef RESOLVER_H
#define RESOLVER_H

#include "task_wrapper.h"
struct resolver
{
    resolver();
    resolver(size_t);
    ~resolver();
    
    void set_fd(int) noexcept;
    int get_fd() const noexcept;
    
    void push(std::shared_ptr<http_request>);
    std::shared_ptr<http_request> pop();
    
    void cancel(http_request*);
    void stop();

    
private:
    
    bool works;
    file_descriptor pipe_fd;
    std::mutex global_mutex;
    std::queue<std::shared_ptr<http_request>> answers;
    
    
    int next_free = 0;
    size_t thread_limit = 10;
    std::vector<std::thread> resolvers;
    std::vector<std::unique_ptr<task_wrapper>> task_wrappers;
    std::unordered_map<int, int> client_index_map;
    
    
};


#endif