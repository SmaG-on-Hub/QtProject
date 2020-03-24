#include "threadpool.h"


ThreadPool::ThreadPool()
{

}

void ThreadPool::setMaxThreadsCount(unsigned char tCount)
{
    unsigned char max_hard_threads = std::thread::hardware_concurrency();
    max_threads_ = tCount > max_hard_threads ? max_hard_threads : tCount;
}

unsigned char ThreadPool::getThreadsCount() const
{
    return active_threads_.load();
}

void ThreadPool::Execute()
{

    std::cout<< "Number of files to copy:" << tasks_.size() << "\n\n"<< std::endl;

    while(!tasks_.empty())
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_var_.wait(lock, [&](){return active_threads_.load() < max_threads_;});
        }

        threads_list_.emplace_front(std::move(tasks_.front()));
        tasks_.pop_front();
        ++active_threads_;
    }

    for (auto& thread : threads_list_)
        if (thread.joinable())
            thread.join();
}


ThreadPool::~ThreadPool()
{

}
