#pragma once
#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <functional>
#include <Windows.h>

using namespace std;
#if defined(_WIN32)  defined(_WIN64)
#include <Windows.h>
#else
#include <pthread.h>
#endif

struct Task {
    function<void(uint32_t)> func;
    uint32_t x;
    Task() {
        x = 0;
    }
    Task(function<void(uint32_t)>& func_, uint32_t& x_) {
        x = x_;
        func = func_;
    }
};

class ThreadsPool {
private:
#if defined (_WIN32) || defined (_WIN64)
    vector<HANDLE> threads;
#else
    vector<pthread_t> threads;
#endif
    uint32_t cntThreads = 0;
    queue <Task> q;
    condition_variable condition;
    mutex m;
    bool stop = false;

public:

    ThreadsPool();
    ~ThreadsPool();
    uint32_t GetThreads();
#if defined (_WIN32) || defined (_WIN64)
    unsigned __stdcall run(void* param);
    unsigned __stdcall passQ(function<void(uint32_t)> f, uint32_t p1);
#else
    static void* run(void* param);
    static void* passQ(function<void(uint32_t)> f, uint32_t p1);
#endif
    
};