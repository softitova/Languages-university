#ifndef KQUEUE_H
#define KQUEUE_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <unordered_map>
#include "exception_.h"
#include "socket.h"
#include <set>
#include <functional>
#include <map>
#include <queue>
#include "timeout.h"


struct kqueue_
{
    kqueue_(kqueue_ const&) = delete;
    kqueue_& operator=(kqueue_ const&) = delete;
    
    kqueue_();

    void add_event(std::function<void(struct kevent&)>, uintptr_t, int16_t, uint16_t, uint32_t fflags = 0, intptr_t data = 0);
    void delete_event(uintptr_t, int16_t);

    void set_handlers(std::set<int>&, time_t);
    void solve();
    
    ~kqueue_();
    
private:
    
    void update(struct kevent);
    void clear_fds();
    
    void add_event(uintptr_t, int16_t, uint16_t, uint32_t, intptr_t, void*);
    void change_event(uintptr_t, int16_t, int16_t);
    
    static const uint16_t EVLIST_SIZE = 512;
    struct kevent event_list[EVLIST_SIZE];
    struct timespec tmout{5,0};
    
    timeout timer;
    socket::file_descriptor kq;
    
    std::unordered_map<ssize_t, socket::writer> working_fds;
    std::map<uintptr_t, std::function<void(struct kevent&)>> sig_handlers;
    std::map<uintptr_t, std::function<void(struct kevent&)>> handlers;
    
};

#endif