
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
#include "sockets.h"


namespace sockets {
    
    void set_options(socket_wrapper& cur_socket)
    {
        const int set = 1;
        if (setsockopt(cur_socket.get_fd(), SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set)) == -1)
        {
            throw my_exception("setting server socket error\n");
        };
        
        if (fcntl(cur_socket.get_fd(), F_SETFL, O_NONBLOCK) == -1)
        {
            throw my_exception("making socket non-blocking error\n");
        }
    }
    
    
    int create_connect_socket(sockaddr addr)
    {
        socket_wrapper server_socket{socket(AF_INET, SOCK_STREAM, 0)};
        if (server_socket.get_fd() == -1)
        {
            throw my_exception("creating socket for server error\n");
        }
        set_options(server_socket);
        
        std::cout << "connecting for socket: " << server_socket.get_fd() << "\n";
        
        if (connect(server_socket.get_fd(), &addr, sizeof(addr)) == -1 && errno != EINPROGRESS)
        {
            throw my_exception("connecting error\n");
        }
        return server_socket.release();
    }
}

file_descriptor::file_descriptor() : fd(-1) {}
file_descriptor::file_descriptor(file_descriptor&& other) : fd(other.fd)
{
    other.fd = -1;
}
file_descriptor::file_descriptor(int fd) : fd(fd) {}

file_descriptor& file_descriptor::operator=(file_descriptor&& other)
{
    fd = other.fd;
    other.fd = -1;
    return *this;
}

void file_descriptor::set_fd(int new_fd)
{
    fd = new_fd;
}

int file_descriptor::get_fd() const
{
    return fd;
}

int file_descriptor::release()
{
    int temp = fd;
    fd = -1;
    return temp;
}

file_descriptor::~file_descriptor()
{
    if(fd != -1)
    {
        if(close(fd) == -1)
        {
            perror("closing file descriptor error");
            return;
        }
        std::cout << "close fd: " << fd << "\n";
    }
}


socket_wrapper::socket_wrapper(int fd) : file_descriptor(fd) {}

socket_wrapper socket_wrapper::accept(int accept_fd)
{
    int fd = ::accept(accept_fd, nullptr, nullptr);
    if (fd == -1)
    {
        throw my_exception("connecting error");
    }
    return socket_wrapper(fd);
}

ptrdiff_t socket_wrapper::write(std::string const& msg)
{
    ptrdiff_t length = send(fd, msg.data(), msg.size(), MSG_DONTWAIT);
    if (length == -1)
    {
        throw my_exception("writing error");
    }
    return length;
};

std::string socket_wrapper::read(size_t buffer_size)
{
    std::vector<char> buffer(buffer_size);
    ssize_t length = recv(fd, buffer.data(), buffer_size, MSG_DONTWAIT);
    std::cout<<length<<"READ SERVER LENGTH\n";
    if (length == -1) {
        throw my_exception("reading error");
    }
    return std::string(buffer.cbegin(), buffer.cbegin() + length);
}

socket_wrapper::~socket_wrapper() {}



client::client(int accept_fd) : socket(socket_wrapper::accept(accept_fd)), server(nullptr) {}

int client::get_fd() const
{
    return socket.get_fd();
}

int client::get_server_fd() const
{
    return server->get_fd();
}

bool client::has_server() const
{
    return server.get() != nullptr;
}

void client::bind(struct server* new_server)
{
    server.reset(new_server);
    server->bind(this);
}

void client::unbind()
{
    if(server)
    {
        server.reset(nullptr);
    }
}

std::string& client::get_text()
{
    return text;
}

size_t client::get_text_size() const
{
    return text.length();
}

void client::append(std::string& str)
{
    text.append(str);
}

size_t client::read(size_t size)
{
    try
    {
        std::string s{socket.read(size)};
        text.append(s);
        return s.size();
    } catch (...) {
        return 0;
    }
}

size_t client::write()
{
    size_t length = socket.write(text);
    text.erase(text.begin(), text.begin() + length);
    try
    {
        if (server)
        {
            get_message();
        }
        return length;
    } catch (...)
    {
        return 0;
    }
}

void client::get_message()
{
    if (text.size() < BUFFER_SIZE && server)
    {
        server->send_message();
    }
}

void client::send_message()
{
    server->append(text);
    text.clear();
}

std::string client::get_host() const
{
    return server->get_host();
}

client::~client()
{
    unbind();
}

server::server(sockaddr addr) : socket(sockets::create_connect_socket(addr)), client(nullptr) {}

int server::get_fd() const
{
    return socket.get_fd();
}

int server::get_client_fd() const
{
    return client->get_fd();
}

void server::bind(struct client* new_client)
{
    client = new_client;
}

void server::append(std::string& str)
{
    text.append(str);
}

std::string& server::get_text()
{
    return text;
}

bool server::text_empty() const
{
    return text.empty();
}

std::string server::read(size_t size)
{
    try {
        return (text.size() < BUFFER_SIZE) ? text.append({socket.read(size)}) :"";
    } catch (...) {
        return "";
    }
}

size_t server::write()
{
    try
    {
        size_t length = socket.write(text);
        text = text.substr(length);
        if (client)
        {
            get_message();
        }
        return length;
    } catch (...) {
        return 0;
    }
}

void server::get_message()
{
    if (text.size() < BUFFER_SIZE)
    {
        client->send_message();
    }
}

void server::send_message()
{
    client->append(text);
    text.clear();
}

void server::set_host(std::string const& str)
{
    host = str;
}

std::string server::get_host() const
{
    return host;
}

void server::disconnect()
{
    client->unbind();
}

server::~server() {}