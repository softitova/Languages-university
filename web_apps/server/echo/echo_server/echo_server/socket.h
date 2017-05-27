#ifndef SOCKET_H
#define SOCKET_H
#include <memory.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <list>
#include "timeout.h"


struct socket {
    
    struct file_descriptor
    {
    
    public:
        file_descriptor& operator=(const file_descriptor&);
        file_descriptor(const file_descriptor&);
        
        file_descriptor();
        file_descriptor(int);
        
        void set_fd(int) noexcept;
        int get_fd() const noexcept;
        
        ~file_descriptor();
        
    private:
        std::shared_ptr<int> fd;
    };
    
    struct writer
    {
        
    public:
        writer(int cur_fd);
        ssize_t to_read();
        bool to_write();
        
        time_t get_fd();
        timeout::q_it get_it();
        void set_it(timeout::q_it);
    
    private:
        
        ssize_t i;
        ssize_t read;
        timeout::q_it it;
    
        static const size_t BUFFER_SIZE = 1024;
        char trash [BUFFER_SIZE];
        
        file_descriptor new_fd;
    };
    
    
};


#endif