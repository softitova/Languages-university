#ifndef sockets_hpp
#define sockets_hpp


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
#include "exceptions.h"
#include <iostream>
#include <netinet/in.h>
#include "timeout.h"



const uint32_t BUFFER_SIZE = 20000;

struct file_descriptor {
    
    file_descriptor();
    file_descriptor(int fd);
    file_descriptor(file_descriptor&&);
    file_descriptor& operator=(file_descriptor&&);
    
    void set_fd(int);
    int get_fd() const;
    int release();
    
    ~file_descriptor();
    
protected:
    int fd;
};


struct socket_wrapper : public file_descriptor
{
    
    socket_wrapper(int fd);
    socket_wrapper(socket_wrapper&&) = default;
    
    static socket_wrapper accept(int);
    ptrdiff_t write(std::string const& msg);
    std::string read(size_t buffer_size);
    
    ~socket_wrapper();
};



struct client {
    
    client(int);
    ~client();
    
    int get_fd() const;
    int get_server_fd() const;
    bool has_server() const;
    
    void bind(struct server* new_server);
    void unbind();
    
    void append(std::string&);
    std::string& get_text();
    size_t get_text_size() const;
   
    bool has_capcacity() const;
    
    size_t read(size_t);
    size_t write();
    
    void get_message();
    void send_message();
    std::string get_host() const;
    
   
private:
    std::string text;
    socket_wrapper socket;
    std::unique_ptr<server> server;
};

struct server {
    
    server(sockaddr);
    ~server();
    
    int get_fd() const;
    int get_client_fd() const;
    
    void bind(struct client*);
    
    void append(std::string&);
    std::string& get_text();
    bool text_empty() const;
    
    std::string read(size_t);
    size_t write();
    
    void send_message();
    void get_message();
    
    void set_host(std::string const&);
    std::string get_host() const;
    void disconnect();
    
    
private:
    std::string text, host;
    socket_wrapper socket;
    struct client* client;
};

#endif