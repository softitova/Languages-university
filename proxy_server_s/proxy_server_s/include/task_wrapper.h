#ifndef TASK_WRAPPER_H
#define TASK_WRAPPER_H
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>
#include <unordered_map>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cassert>
#include <fcntl.h>
#include <csignal>

#include "http.h"
#include "sockets.h"


struct task_wrapper
{
    task_wrapper(file_descriptor&, std::mutex&,std::queue<std::shared_ptr<http_request>>&, bool&);
    ~task_wrapper();
    
    void add_task(std::shared_ptr<http_request>& request);
    void execute();
    
    
private:
    std::mutex m;
    std::condition_variable cv;
    std::queue<std::shared_ptr<http_request>> task_queue;
    void write_to_pipe() noexcept;
    
    std::mutex* global_mutex;
    bool* works;
    std::queue<std::shared_ptr<http_request>>* answers;
    file_descriptor* pipe_fd;
    
    
};

#endif