#include "Client.h"
#include "Serialization.h"
#include "Logger.h"

#include "ChunksManager.h"

#include <thread>
#include <sstream>
#include <mutex>
#include <condition_variable>

int main()
{
    std::string serverAdress = "localhost";

    size_t pixelsWide = 800;
    size_t pixelsHigh = 600;
    double zoom = 3;
    size_t antiAliasFactor = 3;

    boost::asio::io_service ioService;
    std::vector<std::thread> networkThreads;

    std::shared_ptr<RayClient::ChunksManager> chunksManager = std::shared_ptr<RayClient::ChunksManager>(new RayClient::ChunksManager(ioService, serverAdress, pixelsWide, pixelsHigh, zoom, antiAliasFactor));

    auto firstRequest = chunksManager->TryToAcquireNetworkWorker();
    
    if (firstRequest)
    {
        firstRequest->AsyncRequest(RayClient::system::SEND_SCENE);
    }

    ioService.post(std::bind(&RayClient::ChunksManager::AsyncInputProcessor, chunksManager));
    ioService.post(std::bind(&RayClient::ChunksManager::AsyncOutputProcessor, chunksManager));
    
    auto objConcurrency(std::thread::hardware_concurrency());
    for (auto i = 0; i < objConcurrency; ++i)
    {
        networkThreads.push_back(std::thread(std::bind(static_cast<size_t(boost::asio::io_service::*) ()> (&boost::asio::io_service::run), std::ref(ioService))));
    }

    for (auto i = 0; i < objConcurrency; ++i)
    {
        networkThreads[i].join();
    }

    return 0;
}