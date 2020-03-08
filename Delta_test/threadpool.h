#pragma once

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <atomic>
#include <list>
#include <thread>


class ThreadPool
{
public:
    ThreadPool();
    void setMaxThreadsCount(unsigned char tCount);
    unsigned char getThreadsCount() const;
    void Execute();
    ~ThreadPool();

    template<typename Function, typename... Args>
    void add(Function&& funcObject, Args&&... args)
    {
        std::function<void()> func = std::bind(funcObject, args...);
        tasks_.emplace_back([&, func_to_thread = std::move(func)]()
        {
            ++active_threads_;
            func_to_thread();
            --active_threads_;
            cond_var_.notify_one();
        });

        std::cout<<"Item is added to queue!\n";
    }

private:
    std::list<std::function<void()>> tasks_;
    std::list<std::thread> threads_list_;

    std::condition_variable cond_var_;
    std::atomic_uchar active_threads_ {0};
    std::mutex mutex_;
    unsigned char max_threads_ {2};
};

#endif // THREADPOOL_H
