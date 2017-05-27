#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <exception>

struct my_exception : std::exception
{
    
public:

    my_exception(std::string error_msg_) : error_msg(error_msg_) {}
    
    const char* what() const noexcept override
    {
        return error_msg.c_str();
    }
    
private:
    
    std::string error_msg;
};


#endif


