#pragma once

#include "Requirements.h"
#include "RayTracer.h"

namespace RayServer
{
    class Message;
    class ClientConnection;

    class Server
    {
        friend class ClientConnection;

    private:

        asio::io_service& mIoService;

        asio::ip::tcp::acceptor mAcceptor;

        void StartAccept();

        void HandleAccept(std::shared_ptr<ClientConnection>, const boost::system::error_code&);
        
        std::string m_SerializedScene;

        std::mutex m_InputQueueMutex;
        std::mutex m_OutputQueueMutex;

        std::deque<std::string> & m_InputQueue;
        std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> & m_OutputQueue;
     
        std::atomic<unsigned int> m_NumOfNetworkPackages;

        std::mutex & m_WaitOnNetworkDone;
        std::condition_variable & m_NotifyNetworkIsDone;

    public:

        Server(asio::io_service& ioService,
            std::deque<std::string> & inputQueue,
            std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> & outputQueue,
            std::condition_variable & notifyNetworkIsDone,
            std::mutex & waitOnNetworkDone);

        std::atomic<bool> m_IsWorkDone;

        void Run();
   
        void LoadScene(const std::string& serializedScene);

        bool IsWorkDone() const;
    };
}