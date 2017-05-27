#ifndef ECHO_H
#define ECHO_H

#include "kqueue_.h"
#include <map>



struct echo_server
{
public:
    echo_server();
    echo_server(std::set<int>);
    echo_server(echo_server const&) = delete;
    echo_server& operator=(echo_server const&) = delete;
    
    void start();
    void soft_stop();
    void stop();
    
    void handle_signal(int, std::function<void(struct kevent&)>);
    
    ~echo_server();

private:
    const time_t delta = 5;
    int cur_kq;
    bool work, stopped;
    kqueue_ queue;
    std::set<int> server_fd;
};

#endif