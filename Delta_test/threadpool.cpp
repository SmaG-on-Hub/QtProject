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
    return active_threads_;
}

void ThreadPool::tryExecute()
{
    while(!threads_vec_.empty())
    {
        threads_vec_.back().join();
        threads_vec_.pop_back();
    }

    std::cout<< std::endl << "Size of vector: " << threads_vec_.size();
}

ThreadPool::~ThreadPool()
{

}
