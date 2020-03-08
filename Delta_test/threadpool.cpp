#include "threadpool.h"


ThreadPool::ThreadPool()
{

}

void ThreadPool::setMaxThreadsCount(unsigned char tCount)
{
    max_threads_ = tCount;
}

unsigned char ThreadPool::getThreadsCount() const
{
    return active_threads_.load();
}

void ThreadPool::Execute()
{

    while(!tasks_.empty())
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_var_.wait(lock, [&](){return active_threads_.load() < max_threads_;});
        }

        threads_list_.emplace_front(std::move(tasks_.front()));
        tasks_.pop_front();
    }

    for (auto& thread : threads_list_)
        if (thread.joinable())
            thread.join();
}


ThreadPool::~ThreadPool()
{

}
