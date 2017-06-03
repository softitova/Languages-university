#include "resolver_new.h"
#include <memory>

resolver::resolver() : works(true) {}
resolver::resolver(size_t max_count) : works(true), thread_limit(max_count) {}
resolver::~resolver()
{
    stop();
}

void resolver::set_fd(int fd) noexcept
{
    pipe_fd.set_fd(fd);
}

int resolver::get_fd() const noexcept
{
    return pipe_fd.get_fd();
}

void resolver::push(std::shared_ptr<http_request> request)
{
    int fd = request->get_client();
    auto it = client_index_map.find(fd);
    size_t i;
    if (it == client_index_map.end())
    {
        if (resolvers.size() < thread_limit)
        {
            i = resolvers.size();
            std::unique_ptr<task_wrapper> ptr = std::make_unique<task_wrapper>(pipe_fd, global_mutex, answers, works);
            task_wrappers.push_back(std::move(ptr));
            resolvers.push_back(std::thread([this, i]() {
                task_wrappers[i]->execute();
            }));
            ++thread_limit;
   
        } else {
            i = next_free;
            //task_wrappers[i]->add_task(request);
            next_free = (next_free + 1) % resolvers.size();
        }
        client_index_map.insert({fd, i});
    } else {
        i = it->second; // or first???
        //task_wrappers[i]->add_task(request);

    }
    task_wrappers[i]->add_task(request);
}

std::shared_ptr<http_request> resolver::pop()
{
    std::unique_lock<std::mutex> locker{global_mutex};
    std::shared_ptr<http_request> request = std::move(answers.front());
    answers.pop();
    
    return request;
}

void resolver::cancel(http_request* request)
{
    if (!works) {
        throw my_exception("Resolver has been already stopped!");
    }
    std::cout<<"canceled\n";
    request->cancel();
}

void resolver::stop()
{
    works = false;
    for (std::thread & r: resolvers)
    {
        r.join();
    }
}
