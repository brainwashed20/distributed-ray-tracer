#pragma once

#include <deque>
#include <mutex>
#include <string>

#include "PixelCoordinates.h"

namespace RayClient
{
    class MutexedQueue
    {
    private:
        std::mutex mQueueMutex;

        std::deque<RayTracer::PixelCoordinates> mQueue;

    public:
        MutexedQueue(const std::deque<RayTracer::PixelCoordinates>& inputData);

        MutexedQueue();

        void PushQueue(const RayTracer::PixelCoordinates&);

        RayTracer::PixelCoordinates PopQueue();

        bool IsQueueEmpty();
    };
}