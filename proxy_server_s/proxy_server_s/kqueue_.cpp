#include <iostream>

#include "exceptions.h"
#include "kqueue_.h"



#include <sys/socket.h>
#include <sys/errno.h>
#include <iostream>

kqueue_::kqueue_()  {
    if ((kq = kqueue()).get_fd() == -1) {
        throw my_exception("kqueue creating error");
    }
}


void kqueue_::add_event_handler(const struct kevent& kev) {
    if (kevent(kq.get_fd(), &kev, 1, nullptr, 0, nullptr) == -1)
    {
        throw my_exception("kqueue error");
    }
}

void kqueue_::add_event(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata ) {
    struct kevent kev;
    EV_SET(&kev, ident, filter, flags, fflags, data, udata);
    if (kevent(kq.get_fd(), &kev, 1, nullptr, 0, nullptr) == -1)
    {
        throw my_exception("kqueue error");
    }
}

void kqueue_::add_event(std::function<void(struct kevent&)> handler, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data) {
    event_handlers[std::pair<uintptr_t, int16_t>(ident, filter)] = handler;
    add_event(ident, filter, flags, fflags, data, nullptr);
}


void kqueue_::delete_event(uintptr_t ident, int16_t filter) {
    auto it = event_handlers.find(std::pair<uintptr_t, int16_t>(ident, filter));
    if (it != event_handlers.end()) {
        event_handlers.erase(it);
        add_event(ident, filter, EV_DELETE);
    }
}

int kqueue_::occured() {
    return kevent(kq.get_fd(), nullptr, 0, event_list, EVLIST_SIZE, nullptr);
}


void kqueue_::execute() {
    int amount;
    if ((amount = kevent(kq.get_fd(), nullptr, 0, event_list, EVLIST_SIZE, nullptr)) == -1) {
        perror("getting events amount\n");
    }
    
    inv_events.clear();
    
    for (int i = 0; i < amount; ++i) {
        //bool is_valid = invalid.find(event_list[i].ident) == invalid.end();
        std::pair<uintptr_t, int16_t> event (event_list[i].ident, event_list[i].filter);
        auto it = event_handlers.find(event);
        if (it != event_handlers.end() /*&& is_valid */) {
     
            std::function<void(struct kevent&)> handler = it->second;
            handler(event_list[i]);
        }
    }
}

void kqueue_::invalidate_events(uintptr_t id) {
    inv_events.insert(id);
}

kqueue_::~kqueue_() {}