#include <arpa/inet.h>
#include <signal.h>
#include <thread>
#include <cassert>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <exception>
#include <vector>
#include <cassert>
#include <memory.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "proxy.h"
#include "exceptions.h"
#include "http_utils.h"

proxy_server::proxy_server(int port) : works(true), want_to_stop(false), queue(), resolver(), proxy_fd(create_listening_socket(port))
{
    int fds[2];
    if (pipe(fds) == -1)
    {
        throw my_exception("creating a pipe error\n");
    }
    resolver.set_fd(fds[1]);
    pipe_fd.set_fd(fds[0]);
    make_nonblocking(fds[0]);
    make_nonblocking(fds[1]);
    queue.add_event([this](struct kevent& kev) {this->connect_client(kev);}, proxy_fd.get_fd(), EVFILT_READ, EV_ADD);
    queue.add_event([this](struct kevent& kev) {this->on_host_resolved(kev);}, pipe_fd.get_fd(), EVFILT_READ, EV_ADD);
    handle_signal(SIGINT, [this](struct kevent& kev) {hard_stop();});
    handle_signal(SIGTERM, [this](struct kevent& kev) {soft_stop();});
}


void proxy_server::run()
{
    works = true;
    std::cout << "server started" << std::endl;
    try {
        while (works){
            queue.execute();
        }
    } catch(my_exception const & e){
        std::cerr<<e.what();
        hard_stop();
    } catch (...) {
        std::cerr<<"Unhandled exception :(\n";
        hard_stop();
    }
}

void proxy_server::hard_stop()
{
    works = false;
    resolver.stop();
}

void proxy_server::soft_stop()
{
    queue.invalidate_events(proxy_fd.get_fd());
    queue.delete_event(proxy_fd.get_fd(), EVFILT_READ);
    want_to_stop = true;
}

void proxy_server::handle_signal(int sig, std::function<void(struct kevent&)> handler)
{
    queue.add_event(handler, sig, EVFILT_SIGNAL, EV_ADD);
    signal(sig, SIG_IGN);
}

proxy_server::~proxy_server()
{
    resolver.stop();
    std::cout << "server stopped!\n";
}


void proxy_server::finalize()
{
    if(clients.empty() && want_to_stop)
    {
        std::cout<<"finalising prodused by soft stop";
        hard_stop();
    }
}

void proxy_server::disconnect_client(struct kevent& ev)
{
    std::cout<<"disconnect client: " << ev.ident << "\n";
    struct client* client = clients.at(ev.ident).get();
    auto it1 = requests.find(client->get_fd());
    if(it1 != requests.end()) {
        resolver.cancel(it1->second);
        requests.erase(it1);
    }
    auto it2 = responses.find(client->get_fd());
    if(it2 != responses.end()) {
        responses.erase(it2);
    }
    if(client->has_server()) {
        int ser_fd = client->get_server_fd();
        std::cout<<"disconnect server for client: " << ser_fd << "\n";
        queue.delete_event(ser_fd, EVFILT_READ);
        queue.delete_event(ser_fd, EVFILT_WRITE);
        queue.invalidate_events(ser_fd);
        servers.erase(ser_fd);
    }
    queue.delete_event(ev.ident, EVFILT_READ);
    queue.delete_event(ev.ident, EVFILT_WRITE);
    queue.delete_event(ev.ident, EVFILT_TIMER);
    queue.invalidate_events(ev.ident);
    clients.erase(ev.ident);
    finalize();
}

void proxy_server::disconnect_server(struct kevent& event) {
    std::cout<<"disconnect client: " << event.ident << "\n";
    struct server* server = servers.at(event.ident);
    update_timer(server->get_client_fd(), TIMEOUT);
    auto it = responses.find(server->get_fd());
    if(it != responses.end()) {
        responses.erase(it);
    }
    queue.delete_event(server->get_fd(), EVFILT_READ);
    queue.delete_event(server->get_fd(), EVFILT_WRITE);
    queue.invalidate_events(event.ident);
    servers.erase(event.ident);
    server->disconnect();
}

void proxy_server::connect_client(struct kevent& event) {
    std::cout<<"connect client: " << event.ident << "\n";
        client* client_ = new client(proxy_fd.get_fd());
        clients[client_->get_fd()] = std::unique_ptr<client>(client_);
        queue.add_event([this](struct kevent& ev) {this->read_from_client(ev);},
                        client_->get_fd(), EVFILT_READ, EV_ADD);
        queue.add_event([this](struct kevent& ev) {this->disconnect_client(ev);},
                        client_->get_fd(), EVFILT_TIMER, EV_ADD, NOTE_SECONDS, 800);
}

void proxy_server::read_from_client(struct kevent& event) {
    
    if(!check_event(event, [this](struct kevent& ev) { this->disconnect_client(ev);})) return;
        std::cout<<"read from client: " << event.ident << "\n";
        struct client* client = clients.at(event.ident).get();
        update_timer(client->get_fd(), TIMEOUT);
        client->read(event.data);
        if (http_request::is_request_ended(client->get_text()))
        {
            std::unique_ptr<http_request> request(new (std::nothrow) http_request(client->get_text()));
            if(!request)
            {
                client->get_text() = BAD_REQUEST;
                queue.add_event([this](struct kevent& ev) {this->write_to_client(ev);},
                                client->get_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
                return;
            }
            responses[client->get_fd()] = http_response();
            responses[client->get_fd()].set_url(request->get_url());
            if(client->has_server() && client->get_host() == request->get_host())
            {
                client->send_message();
                queue.add_event([this](struct kevent& kev) {this->write_to_server(kev);},
                                client->get_server_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
                return;
            }
            client->unbind();
            int fd = client->get_fd();
            request->set_client(fd);
            requests[fd] = request.get();
            resolver.push(std::move(request));
        }
}

void proxy_server::read_header_from_server(struct kevent& event) {
    std::cout<<"HEHEHEHHEHHE\n";
    //if(!check_event(event, [this](struct kevent& ev) { this->disconnect_server(ev);})) return;
    std::cout<<"read header from server: " << event.ident << "\n";
    struct server* server = servers.at(event.ident);

    update_timer(server->get_client_fd(), TIMEOUT);
    http_response* response = &responses.at(server->get_client_fd());
    std::string msg = server->read(event.data);
    //std::cout<<msg << " MSG sz "<< msg.size()<< "msg    \n";
    response->append_text(msg);
    if(response->get_text().find("400") != std::string::npos)
    {
        queue.invalidate_events(event.ident);
    }
    if (response->is_headert_ended()) {
        std::cout<<" PARSING HEADER WILL BE \n";
        server->send_message();
        queue.add_event([this](struct kevent& kev) {this->read_from_server(kev);}, event.ident, EVFILT_READ, EV_ADD);
        queue.add_event([this](struct kevent& kev){this->write_to_client(kev);}, server->get_client_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
    }
}


void proxy_server::read_from_server(struct kevent& event) {
    if(!check_event(event, [this](struct kevent& ev) { this->disconnect_server(ev);})) return;
    std::cout << "read from server: " << event.ident << "\n";
    struct server* server = servers.at(event.ident);
    update_timer(server->get_client_fd(), TIMEOUT);
    std::string msg = server->read(event.data);
    if (msg.length() > 0) {
        http_response* response = &responses.at(server->get_client_fd());
        response->append_text(msg);
        server->send_message();
        std::cout<< "CONTINUE WRITING TO CLIENT\n";
        queue.add_event(server->get_client_fd(), EVFILT_WRITE, EV_ENABLE);
    }
}



void proxy_server::write_to_server(struct kevent& event) {
    if(!check_event(event, [this](struct kevent& ev) { this->disconnect_server(ev);})) return;
    struct server* server = servers.at(event.ident);
    std::cout << "write to server: " << event.ident << "\n";
    update_timer(server->get_client_fd(), TIMEOUT);
    int err;
    socklen_t len = sizeof(err);
    if (getsockopt((int)(event.ident), SOL_SOCKET, SO_ERROR, &err, &len) == -1 || err != 0) {
        disconnect_server(event);
        std::cout<<("connecting to server error\n");
        return;
    }
    server->write();
    std::cout<<"BEFORE READING HRADER FROM CLIENT\n";
    if (server->text_empty()) {
        std::cout<<"OOOOOOOOOOOOO\n";
        queue.add_event([this](struct kevent& ev) {this->read_header_from_server(ev);}, event.ident, EVFILT_READ, EV_ADD);
        queue.delete_event(event.ident, event.filter);
        queue.add_event([this](struct kevent& ev) {this->write_to_client(ev);}, server->get_client_fd(), EVFILT_WRITE, EV_ADD);
    }
}



void proxy_server::write_to_client(struct kevent& event)
{
    if(!check_event(event, [this](struct kevent& ev) { this->disconnect_client(ev);})) return;
    std::cout << "write to client: " << event.ident << "\n";
    struct client* client = clients.at(event.ident).get();
    update_timer(client->get_fd(),TIMEOUT);
    client->write();
    if (client->has_server()) client->get_message();
    if (client->get_text_size() == 0) {
        queue.add_event(event.ident, event.filter, EV_DISABLE, 0, 0, nullptr);
    }
}

void proxy_server::on_host_resolved(struct kevent& event)
{
    
    std::cout << "on host resoved\n";
    char tmp;
    if (read(pipe_fd.get_fd(), &tmp, sizeof(tmp)) == -1) {
        std::cout<<"reading after resolving error"; // //   mb hard stop;??
        hard_stop();
    }
    std::shared_ptr<http_request> request = resolver.pop();
    std::cout << "host resoved for client: " << request->get_client() << "\n";
    if (request->is_canceled()) return;
    client* client = clients.at(request->get_client()).get();
    update_timer(client->get_fd(), TIMEOUT);
    requests.erase(client->get_fd());
    if (request->get_state() == BAD) {
        client->get_text() = BAD_REQUEST;
        queue.add_event([this](struct kevent& kev) {this->write_to_client(kev);}, client->get_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
        return;
    }
    sockaddr result = request->get_server();
    try {
        struct server* server = new struct server(result);
        server->set_host(request->get_host());
        client->bind(server);
        server->bind(client);
        servers[server->get_fd()] = server;
        queue.add_event([this](struct kevent& kev) {this->write_to_server(kev);}, server->get_fd(), EVFILT_WRITE, EV_ADD);
        client->send_message();
    } catch (...) {
        client->get_text() = BAD_REQUEST;
        queue.add_event(client->get_fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE);
        return;
    }
}



void proxy_server::update_timer(int client_fd, time_t time) {
    queue.add_event(client_fd, EVFILT_TIMER, EV_DELETE, NOTE_SECONDS, time, nullptr);
    queue.add_event(client_fd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, time, nullptr);
}

bool proxy_server::check_event(struct kevent& event, std::function<void(struct kevent&)> handler)
{
    if (event.flags & EV_EOF && event.data == 0) {
        handler(event);
//        std::cout<<"eof- disconnect\n";
        return false;
    }
    return true;
}

void proxy_server::make_nonblocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
    {
        throw my_exception("making socket non-blocking error\n");
    }
}

sockaddr_in proxy_server::create_addr(int port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return addr;
}

int proxy_server::create_listening_socket(int port)
{
    int new_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (new_socket == -1){
        throw my_exception("creating socket error");
    }
    const int set = 1;
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set)) == -1){
        std::cout<<("setting main socket SO_REUSEPORT error\n");
    };
    auto addr = create_addr(port);
    if (bind(new_socket, (sockaddr*) &addr, sizeof(addr)) == -1){
        throw my_exception("binding error\n");
    }
    make_nonblocking(new_socket);
    if (listen(new_socket, SOMAXCONN) == -1){
        throw my_exception("starting listening error\n");
    }
    return new_socket;
}
