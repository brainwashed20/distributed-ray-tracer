#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <deque>
#include <utility>

#include <boost/asio.hpp>

// RayTracer includes
#include "Serialization.h"
#include "RayTracer.h"

namespace RayClient
{
    class ChunksManager
    {
        friend class Client;

    public:
        ~ChunksManager();

        ChunksManager(boost::asio::io_service& ioService, std::string onServer, unsigned int pixelsWide, unsigned int  pixelsHigh, double zoom, unsigned int  antiAliasFactor);

        boost::asio::io_service & m_IoService;
        std::string m_OnServer;

        // Scene config
        std::shared_ptr<RayTracer::Scene> m_Scene;
        unsigned int m_PixelsWide;
        unsigned int m_PixelsHigh;
        double m_Zoom;
        unsigned int m_AntiAliasFactor;

        std::atomic<bool> m_SceneReceived;
        std::atomic<bool> m_PixelsProcessDone;
        std::atomic<bool> m_FinishedNetworkOperations;

        void SaveScene(const std::string& serializedScene);
        void QueueInputPixels(const std::vector<int>& pixelsChunk);
        void QueueOutputPixels(const std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>& pixelsChunk);
        
        std::deque<std::vector<int>> m_InputQueue;
        std::deque<std::vector<int>> m_InputBufferQueue;
        std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> m_OutputQueue;
        std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> m_OutputBufferQueue;
        
        void AsyncInputProcessor();
        void AsyncOutputProcessor();
        
        std::condition_variable m_WaitOnInputDataQueue;
        std::condition_variable m_WaitOnOutputDataQueue;
    
        std::mutex m_OutputQueueMutex;
        std::mutex m_OutputBufferQueueMutex;
        std::mutex m_InputQueueMutex;
        std::mutex m_InputBufferQueueMutex;
        
        std::deque<std::shared_ptr<Client>> m_AsyncRequests;
        
        std::mutex m_EventNetworkMutex;
        std::mutex m_AsyncWorkersMutex;
        std::mutex m_SceneMutex;

        const std::shared_ptr<Client> FinishNetworkWorkers();
        const std::shared_ptr<Client> TryToAcquireNetworkWorker();
    };
}