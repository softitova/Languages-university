#include "kqueue_.h"

kqueue_::kqueue_() : kq(kqueue()) {}

void kqueue_::add_event(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
{
    struct kevent kev;
    EV_SET(&kev, ident, filter, flags, fflags, data, udata);
 
    if (kevent(kq.get_fd(), &kev, 1, nullptr, 0, nullptr) == -1)
    {
        throw my_exception("kqueue error during adding event");
    }
}

void kqueue_::add_event(std::function<void(struct kevent&)> handler, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data)
{
    add_event(ident, filter, flags, fflags, data, nullptr);
    sig_handlers[ident] = handler;
}

void kqueue_::change_event(uintptr_t ident, int16_t filter_before, int16_t filter_after)
{
    add_event(ident, filter_before, EV_DELETE, 0, 0, nullptr);
    add_event(ident, filter_after, EV_ADD, 0, 0, nullptr);
}


void kqueue_::delete_event(uintptr_t ident, int16_t filter)
{
    auto it = sig_handlers.find(ident);
    
    if (it != sig_handlers.end())
    {
        add_event(ident, filter, EV_DELETE, 0, 0, nullptr);
        sig_handlers.erase(it);
    }
}

void kqueue_::set_handlers(std::set<int> &server_fd, time_t delta)
{
    std::function<void(struct kevent&)> read_handler = [this, &server_fd, delta](struct kevent& ke)
    {
        if(server_fd.find((int)ke.ident) != server_fd.end())
        {
            int fd = accept((int)ke.ident, nullptr, nullptr);
            fcntl(fd, F_SETFL,O_NONBLOCK);
            add_event(fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
            socket::writer writer = socket::writer(fd);
            writer.set_it(timer.add(delta, timeout::timeout_wrapper(delta, time(NULL) + delta, fd, nullptr)));
            working_fds.insert({fd, writer});
        }
        else
        {
            ssize_t read = working_fds.find(ke.ident)->second.to_read();
            if (!read)
            {
                working_fds.erase(working_fds.find(ke.ident)); // надо ли удалять или забить
            }
            else if (read > 0)
            {
                change_event(ke.ident, EVFILT_READ, EVFILT_WRITE);
            }
        }
    };
    
    std::function<void (struct kevent&)> write_handler = [this](struct kevent& ke)
    {
        if (working_fds.find(ke.ident)-> second.to_write())
        {
            change_event(ke.ident, EVFILT_WRITE, EVFILT_READ);
        }
    };
    
    handlers[EVFILT_READ] = read_handler;
    handlers[EVFILT_WRITE] = write_handler;
}

void kqueue_::update(struct kevent ke)
{
    auto itt = working_fds.find(ke.ident);
    if(itt != working_fds.end())
    {
        socket::writer er = itt->second;
        timer.update(er.get_it(), time(NULL) + er.get_it()->timeout);
    }
}

void kqueue_::clear_fds()
{
    if(!working_fds.empty())
    {
        working_fds.erase(timer.get_min_fd());
        timer.remove(timer.get_min());
    }
}


void kqueue_::solve()
{

    tmout = timer.get_min_time();
    
    int amount = kevent(kq.get_fd(), nullptr, 0, event_list, EVLIST_SIZE, &tmout);
    if (amount == -1)
    {
        //perror("Getting number of events error!\n");
    }
    
    if (amount == 0)
    {
        clear_fds();
    }
   
    std::function<void(struct kevent&)>  handler;
    for (int i = 0; i < amount; ++i)
    {
        update(event_list[i]);
        
        if(handlers.find(event_list[i].filter) != handlers.end())
        {
            handler = handlers.at(event_list[i].filter);
            handler(event_list[i]);
        }
        else if(sig_handlers.find(event_list[i].ident) != sig_handlers.end())
        {
            handler = sig_handlers.at(event_list[i].ident);
            handler(event_list[i]);
        }
    }
}

kqueue_::~kqueue_() {}


