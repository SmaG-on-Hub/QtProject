#pragma once

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <iostream>
#include <atomic>
#include <vector>
#include <thread>


class ThreadPool
{
public:
    ThreadPool();
    void setMaxThreadsCount(unsigned char tCount);
    unsigned char getThreadsCount() const;
    void tryExecute();
    ~ThreadPool();

    template<typename Function, typename... Args>
    void add(Function&& funcObject, Args&&... args)
    {
       threads_vec_.emplace_back(std::thread(std::forward<Function>(funcObject), std::forward<Args>(args)...));
    }

private:
    std::vector<std::thread> threads_vec_;

    std::atomic_uchar active_threads_ {0};
    unsigned char max_threads_ {2};
};

//std::atomic<unsigned char> ThreadPool::active_threads_;
//unsigned char ThreadPool::MAX_THREADS_ {2};

#endif // THREADPOOL_H
