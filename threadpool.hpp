#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>

class ThreadPool;

class ThreadPool{

public:
    friend void thread_work(ThreadPool *it);
    ThreadPool(size_t);

    template<typename F, typename... Args>
    auto submit_task(F &&f, Args&&... args)
    -> std::future<typename std::result_of_t<F(Args...)>>;

    ~ThreadPool();


private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    bool stop;
    std::mutex mtx;
    std::condition_variable cond;

};

ThreadPool::ThreadPool(size_t thread_size = std::thread::hardware_concurrency()) : stop(false) {
    for(size_t i = 0; i < thread_size; ++i) {
        workers.emplace_back(std::thread(thread_work, this));
    }
}

void thread_work(ThreadPool *it) {
    std::function<void()> task;
    {
        std::unique_lock<std::mutex> locker(it->mtx);
        (it->cond).wait(locker, [it]() {
            return !(it->tasks).empty() || it->stop;
        });

        if(it->stop && (it->tasks).empty()) {
            return ;
        }
        task = std::move((it->tasks).front());
        (it->tasks).pop();
    }
    task();
}

template<typename F, typename... Args>
auto ThreadPool::submit_task(F &&f, Args&&...args) -> std::future<std::result_of_t<F(Args...)>> {
    using return_type = std::result_of_t<F(Args...)>;
    
    auto task = std::make_shared<std::packaged_task<return_type()>> (
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(stop) {
            std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace([task]() { (*task)() ;} );
    }
    cond.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock;
        stop = true;
    }
    cond.notify_all();
    for(int i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }
}