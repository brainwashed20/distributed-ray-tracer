#include "MutexedQueue.h"

namespace RayServer
{
    MutexedQueue::MutexedQueue(const std::deque<std::string>& inputData)
    {
        mQueue = inputData;
    }

    void MutexedQueue::PushQueue(const std::string& queueElem)
    {
        std::unique_lock<std::mutex> queueLock(mQueueMutex);
        mQueue.push_back(queueElem);
    }

    std::string MutexedQueue::PopQueue()
    {
        std::string res;

        {
            std::unique_lock<std::mutex> queueLock(mQueueMutex);
            if (!mQueue.empty())
            {
                res = mQueue.front();
                mQueue.pop_front();
            }
        }

        return res;
    }

    bool MutexedQueue::IsQueueEmpty()
    {
        bool isEmpty = true;

        {
            std::unique_lock<std::mutex> queueLock(mQueueMutex);
            bool isEmpty = mQueue.empty();
        }

        return isEmpty;
    }
}