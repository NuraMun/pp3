#include "ThreadPool.h"

ThreadsPool::ThreadsPool() {
    cntThreads = std::thread::hardware_concurrency();

    for (uint32_t i = 0; i < cntThreads; i++) {
#if defined(_WIN32)  || defined(_WIN64)
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, [](void* param) -> unsigned {
            return static_cast<ThreadsPool*>(param)->run(param);
            }, this, 0, NULL);
        threads.emplace_back(thread);
#elif defined __linux__
        pthread_t thread;
        int result = pthread_create(&thread, nullptr, [](void* param) -> void* {
            return static_cast<ThreadsPool*>(param)->run(param);  // Pass the param
            }, this);
        if (result == 0) {
            threads.emplace_back(thread);
        }
#endif
    }
}

ThreadsPool::~ThreadsPool() {
    stop = true;
    condition.notify_all();

#if defined (_WIN32) || defined (_WIN64)
    WaitForMultipleObjects(GetThreads(), threads.data(), TRUE, INFINITE);
#endif
    for (uint32_t i = 0; i < cntThreads; i++) {
#if defined(_WIN32)  || defined(_WIN64)
        CloseHandle(threads[i]);

#else
        pthread_join(threads[i], nullptr);

#endif
    }
}

uint32_t ThreadsPool::GetThreads()
{
    return cntThreads;
}

#if defined (_WIN32) || defined (_WIN64)
unsigned __stdcall ThreadsPool::run(void* param)
#elif defined __linux__
void* ThreadsPool::run(void* param)
#endif
{
    while (true) {
        unique_lock<mutex> lk(m);
        condition.wait(lk, [this]() {return !q.empty() || stop; });
        if (!q.empty()) {
            Task task = q.front();
            q.pop();
            lk.unlock();
            task.func(task.x);
        }
        if (stop)
            break;

    }
#if defined(_WIN32) || defined(_WIN64)
    _endthreadex(0);
    return 0;
#else
    return nullptr;
#endif
}

#if defined (_WIN32) || defined (_WIN64)
void ThreadsPool::passQ(function<void(uint32_t)> f, double x)
#elif defined __linux__
void* ThreadsPool::passQ(function<void(uint32_t)> f, double x)
#endif
{
    {
        lock_guard<mutex> lg(m);
        q.push(Task{ f, x });
    }

    condition.notify_one();
#if defined __linux__
    return nullptr;
#endif
}