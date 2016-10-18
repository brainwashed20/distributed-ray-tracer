#include "ChunksManager.h"
#include "Client.h"
#include "Logger.h"

#include <sstream>

namespace RayClient
{
    ChunksManager::ChunksManager(boost::asio::io_service & ioService, std::string onServer, unsigned int pixelsWide, unsigned int  pixelsHigh, double zoom, unsigned int antiAliasFactor) :
        m_IoService(ioService),
        m_OnServer(onServer),
        m_PixelsWide(pixelsWide),
        m_PixelsHigh(pixelsHigh),
        m_Zoom(zoom),
        m_AntiAliasFactor(antiAliasFactor),
        m_SceneReceived(false),
        m_PixelsProcessDone(false)
    {
        m_Scene = std::shared_ptr<RayTracer::Scene>(new RayTracer::Scene(RayTracer::Color(0.196078, 0.8, 0.196078, 7.0e-6)));
    }

    ChunksManager::~ChunksManager()
    {
        m_IoService.stop();
    }

    void ChunksManager::SaveScene(const std::string& serializedScene)
    {
        // deserialize
        {
            std::stringstream objectBuffer(serializedScene);
            boost::archive::text_iarchive inArchive(objectBuffer);
            inArchive >> *m_Scene;
        }

        m_SceneReceived = true;

        auto networkWorker = TryToAcquireNetworkWorker();

        if (networkWorker)
        {
            networkWorker->AsyncRequest(system::RECEIVED_SCENE);
        }
    }

    void ChunksManager::QueueInputPixels(const std::vector<int>& pixelChunk)
    {
        std::unique_lock<std::mutex> guardLock(m_InputQueueMutex, std::defer_lock);

        if (guardLock.try_lock())
        {
            std::lock_guard<std::mutex> buffer(m_InputBufferQueueMutex);

            m_InputQueue.insert(m_InputQueue.end(), m_InputBufferQueue.begin(), m_InputBufferQueue.end());
            m_InputBufferQueue.clear();

            m_InputQueue.push_back(pixelChunk);

            m_WaitOnInputDataQueue.notify_one();
        }
        else
        {
            std::lock_guard<std::mutex> buffer(m_InputBufferQueueMutex);
            m_InputBufferQueue.push_back(pixelChunk);
        }
    }

    void ChunksManager::QueueOutputPixels(const std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>& pixelChunk)
    {
        std::unique_lock<std::mutex> guardLock(m_OutputQueueMutex, std::defer_lock);

        if (guardLock.try_lock())
        {
            std::lock_guard<std::mutex> buffer(m_OutputBufferQueueMutex);

            // Insert
            m_OutputQueue.insert(m_OutputQueue.end(), m_OutputBufferQueue.begin(), m_OutputBufferQueue.end());
            m_OutputBufferQueue.clear();

            m_OutputQueue.push_back(pixelChunk);

            m_WaitOnOutputDataQueue.notify_one();
        }
        else
        {
            std::lock_guard<std::mutex> buffer(m_OutputBufferQueueMutex);
            m_OutputBufferQueue.push_back(pixelChunk);
        }
    }

    void ChunksManager::AsyncInputProcessor()
    {
        std::unique_lock<std::mutex> guardLock(m_InputQueueMutex);

        const size_t largePixelsWide = m_AntiAliasFactor * m_PixelsWide;
        const size_t largePixelsHigh = m_AntiAliasFactor * m_PixelsHigh;
        std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>> computedChunk;

        while (m_PixelsProcessDone.load(std::memory_order_relaxed) == false)
        {
            m_WaitOnInputDataQueue.wait(guardLock); 

            while (!m_InputQueue.empty())
            {
                auto matrixLines = m_InputQueue.front();

                m_InputQueue.pop_front();

                computedChunk.clear();

                for (const auto& lin : matrixLines)
                {
                    for (unsigned int col = 0; col < largePixelsHigh; ++col)
                    {
                        RayTracer::PixelData pixelColor = m_Scene->GetPixelAt(lin, col, m_PixelsWide, m_PixelsHigh, m_Zoom, m_AntiAliasFactor);

                        computedChunk.push_back(std::make_pair(RayTracer::PixelCoordinates(lin, col), pixelColor));
                    }
                }
                
                QueueOutputPixels(computedChunk);
            }
        }
    }

    void ChunksManager::AsyncOutputProcessor()
    {
        while (m_PixelsProcessDone.load(std::memory_order_relaxed) == false)
        {
            std::unique_lock<std::mutex> guardLock(m_OutputQueueMutex);

            m_WaitOnOutputDataQueue.wait(guardLock);

            for (bool tryToAquireWorker = false; !tryToAquireWorker;)
            {
                auto networkWorker = TryToAcquireNetworkWorker();

                if (networkWorker)
                {
                    if (!m_OutputQueue.empty())
                    {
                        auto outputChunk = m_OutputQueue.back();
                        m_OutputQueue.pop_back();

                        std::stringstream objectBuffer;
                        boost::archive::text_oarchive outArchive(objectBuffer);
                        outArchive << outputChunk;
                        networkWorker->AsyncRequest(system::COMPUTED_PIXELS, objectBuffer.str());

                        Logger::WriteLog("Sending ray traced pixels to server..");

                        tryToAquireWorker = true;
                    }
                }
            }
        }
    }

    const std::shared_ptr<Client> ChunksManager::FinishNetworkWorkers()
    {
        std::lock_guard<std::mutex> guardLock(m_AsyncWorkersMutex);

        std::shared_ptr<Client> networkWorker;
     
        for (auto workersIterator = m_AsyncRequests.begin(); workersIterator != m_AsyncRequests.end(); ++workersIterator)
        {
            switch (workersIterator.operator * ()->GetStatus())
            {
                case Client::FAILED_ON_RESOLVE:
                case Client::FAILED_ON_CONNECT:
                case Client::FAILED_ON_WRITE_TO_SOCKET:
                case Client::FAILED_ON_READ:
                {
                    // Erase object as we really want a fresh state
                    std::shared_ptr<Client> failedWorker(*workersIterator);

                    // Swap instead of erasing to avoid iterator invalidation
                    std::shared_ptr<Client> newWorker(std::shared_ptr<Client>(std::shared_ptr<Client>(new Client(std::ref(*this)))));

                    workersIterator->swap(newWorker);
                    failedWorker.reset();
                    break;
                }

                case Client::AWAITING_WORK:
                case Client::OK_STATUS:
                {
                    networkWorker = workersIterator.operator * ();
                    break;
                }
                   
                default:
                    break;
            }
        }

        return networkWorker;
    }
    const std::shared_ptr<Client> ChunksManager::TryToAcquireNetworkWorker()
    {
        std::shared_ptr<Client> networkWorker;

        while (!(networkWorker))
        {
            networkWorker = FinishNetworkWorkers();

            if (!(networkWorker))
            {
                if (m_AsyncRequests.size() < system::ASYNC_CLIENTS_MAX)
                {
                  
                    std::lock_guard<std::mutex> guardLock(m_AsyncWorkersMutex);
                    networkWorker = std::shared_ptr<Client>(std::shared_ptr<Client>(new Client(std::ref(*this))));

                    m_AsyncRequests.push_back(networkWorker);
                }
                else
                {
                    networkWorker = FinishNetworkWorkers();
                }
            }
        }

        return networkWorker;
    }
}