#include "ThreadPool.h"

ThreadsPool::ThreadsPool() {
    cntThreads = std::thread::hardware_concurrency();
    unsigned int threadID;
    for (unsigned int i = 0; i < cntThreads; i++) {
#if defined(_WIN32)  defined(_WIN64)
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, [](void* param) -> unsigned {
            return static_cast<ThreadsPool*>(param)->run(param);
            }, this, 0, NULL);
        threads.emplace_back(thread);
#else
        pthread_t thread;
        pthread_create(&thread, nullptr, [](void* param) -> void* {
            static_cast<ThreadPool*>(param)->run();
            return nullptr;
            }, this);
        threads.push_back(reinterpret_cast<HANDLE>(thread)); 
#endif
    }
}

ThreadsPool::~ThreadsPool() {
    stop = true;
    condition.notify_all();
#if defined(_WIN32)  defined(_WIN64)
    WaitForMultipleObjects(GetThreads(), threads.data(), TRUE, INFINITE);
    for (unsigned int i = 0; i < cntThreads; i++) {
        CloseHandle(threads[i]);
    }
#else
    pthread_join(threads[i], nullptr);
#endif
}

uint32_t ThreadsPool::GetThreads()
{
    return cntThreads;
}

#if defined(_WIN32)  defined(_WIN64)
unsigned __stdcall ThreadsPool::run(void* param)
{
#else
static void* ThreadsPool::run(void* param)
{
#endif
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
    _endthreadex(0);
    return 0;
}

#if defined(_WIN32)  defined(_WIN64)
unsigned __stdcall ThreadsPool::passQ(function<void(uint32_t)> f, uint32_t x)
{
#else
static void* ThreadsPool::passQ(function<void(uint32_t)> f, uint32_t x)
{
#endif
    {
        lock_guard<mutex> lg(m);
        q.push(Task{ f, x });
    }

    condition.notify_one();
    return 0;
}