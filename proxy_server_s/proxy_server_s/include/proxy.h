#ifndef PROXY_H
#define PROXY_H

#include "kqueue_.h"
#include "sockets.h"
#include "http.h"
#include "resolver_new.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <mutex>
#include <fcntl.h>
#include <queue>
#include <iostream>
#include <signal.h>

struct proxy_server {
    
    proxy_server(int);
    ~proxy_server();
        
    void run();
    void hard_stop();
    void soft_stop();
    
private:
    
    void connect_client(struct kevent&);

    void read_from_client(struct kevent&);
    void read_header_from_server(struct kevent&);
    void read_from_server(struct kevent&);
    
    void write_to_client(struct kevent&);
    void write_to_server(struct kevent&);
    
    void on_host_resolved(struct kevent&);
    
    void disconnect_client(struct kevent&);
    void disconnect_server(struct kevent&);
    
    void update_timer(int, time_t);
    void finalize();
    bool check_event(struct kevent&, std::function<void(struct kevent&)>);
    void handle_signal(int, std::function<void(struct kevent&)>);
    
    void make_nonblocking(int);
    sockaddr_in create_addr(int);
    int create_listening_socket(int);
    
    bool works, want_to_stop;
    kqueue_ queue;
    resolver resolver;
    file_descriptor proxy_fd, pipe_fd;
    
    std::map<int, http_request*> requests;
    std::map<int, http_response> responses;
    
    std::map<uintptr_t, server*> servers;
    std::map<uintptr_t, std::unique_ptr<client>> clients;
   
};

#endif
