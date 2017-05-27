#ifndef KQUEUE_H
#define KQUEUE_H
#include <sys/types.h>
#include <set>
#include <functional>
#include <sys/types.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include "sockets.h"
#include "timeout.h"


struct kqueue_
{

    kqueue_();
    ~kqueue_();
    
    void add_event(uintptr_t, int16_t, uint16_t, uint32_t = 0, intptr_t = 0, void* = nullptr);
    void add_event(std::function<void(struct kevent&)>, uintptr_t, int16_t, uint16_t, uint32_t = 0, intptr_t = 0);
    
    void delete_event(uintptr_t, int16_t);
    
    void execute();
    void invalidate_events(uintptr_t);
    
    
    
private:
    
    void add_event_handler(const struct kevent&);
    int occured();
    static const uint16_t EVLIST_SIZE = 1024;
    file_descriptor kq;
    struct kevent event_list[EVLIST_SIZE];
    std::set<uintptr_t> inv_events;
    std::map<std::pair<uintptr_t, int16_t>, std::function<void(struct kevent&)> > event_handlers;
};

#endif
