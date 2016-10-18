#pragma once

#include <deque>
#include <mutex>
#include <vector>

namespace RayServer
{
    class MutexedQueue
    {
    private:
        std::mutex mQueueMutex;

        std::deque<std::string> mQueue;

    public:
        MutexedQueue(const std::deque<std::string>& inputData);

        MutexedQueue();

        void PushQueue(const std::string&);

        std::string PopQueue();

        bool IsQueueEmpty();
    };
}