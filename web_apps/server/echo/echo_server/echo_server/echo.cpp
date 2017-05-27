#include "echo.h"

int create_listening_socket(int port) {
    
    int new_socket = socket(PF_INET, SOCK_STREAM, 0);
    
    if (new_socket == -1)
    {
        throw my_exception("Creating socket error occurred");
    }
    
    const int set = 1;
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set)) == -1)
    {
        throw my_exception("Setting main socket SO_REUSEPORT failed");
    };
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(new_socket, (sockaddr*) &addr, sizeof(addr)) == -1)
    {
        throw my_exception("Binding error occured!");
    }
    
    fcntl(new_socket, F_SETFL,O_NONBLOCK);
    
    if (listen(new_socket, SOMAXCONN) == -1)
    {
        throw my_exception("Listening error occurred");
    }
    
    return new_socket;
}

echo_server::echo_server() : work(true), stopped(false), queue() {}

echo_server::echo_server(std::set<int> ports) :  work(true), stopped(false), queue()
{
    for(int port : ports)
    {
        int cur_sock = create_listening_socket(port);
        server_fd.insert(cur_sock);
        std::cerr << port;
        queue.add_event([](struct kevent& ke){},cur_sock, EVFILT_READ, EV_ADD, 0, 0);
    }
}

void echo_server::handle_signal(int sig, std::function<void(struct kevent&)> handler)
{
    queue.add_event(handler, sig, EVFILT_SIGNAL, EV_ADD);
    signal(sig, SIG_IGN);
}

void echo_server::start()
{
    try
    {
        time_t delta = 5;
        queue.set_handlers(server_fd, delta);
        
        while (work)
        {
            queue.solve();
        }
    } catch(my_exception e) {
        std::cerr<<e.what();
        soft_stop();
    }
}

void echo_server::stop() // TODO: fix it
{
    work = false;
    //soft_stop();
}

void echo_server::soft_stop()
{
    std::cout << "\nSoft stop!\n";
    for(int cur_fd : server_fd)
    {
        queue.delete_event(cur_fd, EVFILT_READ);
    }
    stopped = true;
    work = false;
}

echo_server::~echo_server()
{
std::cout << "Server stopped!\n";
}
