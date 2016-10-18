#include "Server.h"
#include "Logger.h"
#include "ClientConnection.h"

#include <iostream>

namespace RayServer
{
    Server::Server(asio::io_service& ioService,
        std::deque<std::string> & inputQueue,
        std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> & outputQueue,
        std::condition_variable & notifyNetworkIsDone,
        std::mutex & waitOnNetworkDone) :
            mIoService(ioService),
            mAcceptor(ioService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 120)),
            m_InputQueue(inputQueue),
            m_OutputQueue(outputQueue),
            m_NumOfNetworkPackages(0),
            m_IsWorkDone(false),
            m_NotifyNetworkIsDone(notifyNetworkIsDone),
            m_WaitOnNetworkDone(waitOnNetworkDone)
    {
    }

    void Server::LoadScene(const std::string& serializedScene)
    {
        m_SerializedScene = serializedScene;
        Logger::WriteLog("Serialized scene loaded.");
    }

    void Server::Run()
    {
        Logger::WriteLog("Server started...");
        Logger::WriteLog("Using port: 120");

        m_NumOfNetworkPackages = m_InputQueue.size();

        StartAccept();
    }

    void Server::StartAccept()
    {
        std::shared_ptr<ClientConnection> connection(new ClientConnection(mAcceptor.get_io_service()));

        mAcceptor.async_accept(
            connection->m_TcpSocket,
            boost::bind(
                &Server::HandleAccept,
                this,
                connection,
                asio::placeholders::error
            )
        );
    }

    void Server::HandleAccept(std::shared_ptr<ClientConnection> connection, const boost::system::error_code& errorCode)
    {
        if (!errorCode)
        {
            connection->Start(this);

            // start accepting the next client
            StartAccept();
        }
        else
        {
            std::cout << "Error: " << errorCode.message() << std::endl;
        }
    }

    bool Server::IsWorkDone() const
    {
        return m_IsWorkDone;
    }
}