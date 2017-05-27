#include "socket.h"

struct my_deleter
{
    void operator()(int *x)
    {
        close(*x);
        delete x;
    }
} deleter_item;


std::shared_ptr<int> create_shared_ptr(int x)
{
    int* y = new int(x);
    try {
        return std::shared_ptr<int>(y, deleter_item);
    } catch(...) {
        delete y;
        throw;
    }
}

socket::file_descriptor::file_descriptor() {}

socket::file_descriptor::file_descriptor(int _fd) : fd(create_shared_ptr(_fd)) {}

socket::file_descriptor::file_descriptor(const file_descriptor& rhs) : fd(rhs.fd) {}

socket::file_descriptor& socket::file_descriptor::operator=(const file_descriptor& other)
{
    fd = other.fd;
    return *this;
}

socket::file_descriptor::~file_descriptor() {}

void socket::file_descriptor::set_fd(int new_fd) noexcept
{
    fd = create_shared_ptr(new_fd);
}

int socket::file_descriptor::get_fd() const noexcept
{
    return *fd;
}

socket::writer::writer(int cur_fd) : i(0), read(0), new_fd(cur_fd) {}

ssize_t  socket::writer::to_read()
{
    i = 0;
    read = recv(new_fd.get_fd(), trash, BUFFER_SIZE, MSG_DONTWAIT);
    return read;
}

bool  socket::writer::to_write()
{
    ssize_t temp = send(new_fd.get_fd(), trash + i, read - i, MSG_DONTWAIT);
    if (temp != -1)
    {
        i += temp;
    }
    return i == read;
}

timeout::q_it socket::writer::get_it()
{
    return it;
}

void socket::writer::set_it(timeout::q_it it_)
{
    it = it_;
}




