#pragma once

#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>


template<class T>
class ResourcePool
{
public:
    ResourcePool() {}
    virtual ~ResourcePool() {};

    void push(const T& e)
    {
        std::lock_guard<std::mutex> gl(l);
        q.push(e);
    }

    T pop()
    {
        std::lock_guard<std::mutex> gl(l);
        if (q.empty())
            return T();
        const T& e = q.front();
        q.pop();
        return e;
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> gl(l);
        return q.size();
    }

    bool empty() const { return (size() == 0); }

private:
    std::queue<T> q;
    mutable std::mutex l;
};

template<class _Res, class _Fn>
void multiCompute(ResourcePool<_Res>& resources, _Fn func)
{
    // Get the maximum threads hardware can support
    static const size_t maxHwThreadNum = std::thread::hardware_concurrency();

    // Total threads should not beyond resources number
    const size_t maxThreadNum = resources.size() < maxHwThreadNum ? resources.size() : maxHwThreadNum;
    printf("Max Threads Number: %d\n", (int)maxThreadNum);

    if (resources.empty())
        return;

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < maxThreadNum; i++) {
        // create #maxThreadNum# threads
        threads.push_back(std::thread([=, &resources]() {
            // thread start routine:
            //   - it get element from resources and process it, until resources is empty
            while (1) {
                _Res e = resources.pop();
                if (e.empty())
                    return;
                e.setThreadId(i);
                // call real process function
                func(e);
            }
        }));
    }

    // wait all threads finish
    for (std::thread& t : threads)
    {
        if (t.joinable())
            t.join();
    }
}
