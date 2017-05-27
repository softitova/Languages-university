#include "task_wrapper.h"

task_wrapper::task_wrapper(file_descriptor& fd, std::mutex& m,std::queue<std::shared_ptr<http_request>>& q, bool& w)
        : global_mutex(&m), works(&w), answers(&q), pipe_fd(&fd) {}
task_wrapper::~task_wrapper()
{
    cv.notify_all();
}

void task_wrapper::add_task(std::shared_ptr<http_request>& request)
{
    std::unique_lock<std::mutex> locker{m};
    task_queue.push(std::move(request));
    cv.notify_all();
    std::cout<<" ONE MORE TASK ADDED\n";

    
//    task_queue.push(std::move(request));
//    std::cout<<task_queue.size() << " size \n ";
   
}

void task_wrapper::execute()
{
    while (works)
    {
        std::cout << "BEFORE MUTEX:\n";
        std::unique_lock<std::mutex> locker(m);
        cv.wait(locker, [&]() {return !task_queue.empty() || !works;});
        if (!works) break;
        
        std::cout << "START RESOLVING ))): " << std::this_thread::get_id() << "\n";
        
        //task_queue.front();
  
        auto request = std::move(task_queue.front());
        if(request->is_canceled()) return;
        task_queue.pop();
        locker.unlock();
        sockaddr result;   
        addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;
        
        size_t i = request->get_host().find(":");
        std::string name(request->get_host().substr(0, i));
        std::string port = i != std::string::npos ? request->get_host().substr(i + 1) : "80";
        if (getaddrinfo(name.c_str(), port.c_str(), &hints, &res)){
            std::cout<< std::endl << "resolving error: " << strerror(errno) << std::endl;;
            locker.lock();
            request->set_state(BAD);
            locker.unlock();
        } else {
            result = *res->ai_addr;
            freeaddrinfo(res);
        }
        request->set_server(result);
        
        std::cout << "resolved\n";
        std::unique_lock<std::mutex> l({*global_mutex});
        //global_mutex->lock();
        
        answers->push(std::move(request));
        //        global_mutex->unlock();
        l.unlock();
        write_to_pipe();
    }

}


void task_wrapper::write_to_pipe() noexcept
{
    char tmp = 1;
    std::unique_lock<std::mutex> locker{*global_mutex};
    if (write(pipe_fd->get_fd(), &tmp, sizeof(tmp)) == -1)
    {
        std::cout<<("sending message from task wrapper error\n");
    }
}








