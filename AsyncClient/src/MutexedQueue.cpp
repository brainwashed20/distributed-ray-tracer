#include "MutexedQueue.h"

namespace RayClient
{
    MutexedQueue::MutexedQueue(const std::deque<RayTracer::PixelCoordinates>& inputData)
    {
        mQueue = inputData;
    }

    MutexedQueue::MutexedQueue()
    {

    }

    void MutexedQueue::PushQueue(const RayTracer::PixelCoordinates& queueElem)
    {
        std::unique_lock<std::mutex> queueLock(mQueueMutex);
        mQueue.push_back(queueElem);
    }

    RayTracer::PixelCoordinates MutexedQueue::PopQueue()
    {
        RayTracer::PixelCoordinates res;

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
        bool isEmpty = false;

        {
            std::unique_lock<std::mutex> queueLock(mQueueMutex);
            isEmpty = mQueue.empty();
        }

        return isEmpty;
    }
}