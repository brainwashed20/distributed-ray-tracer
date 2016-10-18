#pragma once

#include "Requirements.h"
#include "ChunksManager.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace RayTracer
{
    class PixelCoordinates;
}

namespace RayClient
{
    class Client : boost::noncopyable
    {
        friend class CunksManager;
        // Pubs
    public:

        // HTTP statuses (mangled with our own)
        enum Statuses
        {
            NO_STATUS = -1,
            AWAITING_WORK = 0,
            RESOLVING_DNS = 1,
            CONNECTING_TO_ENDPOINT = 2,
            FAILED_ON_RESOLVE = 3,
            WRITING_TO_SOCKET = 4,
            FAILED_ON_CONNECT = 5,
            READING_CONTENT = 6,
            FAILED_ON_WRITE_TO_SOCKET = 7,
            FAILED_ON_READ = 8,
            OK_STATUS = 999

        };

        // The only possible constructor
        Client(ChunksManager & chunksManager);
        void AsyncRequest(const std::string &, const std::string & = std::string());
        ~Client();

        Statuses GetStatus();
    private:
        // Methods
        void HandleResolve(const boost::system::error_code &, asio::ip::tcp::resolver::iterator);
        void HandleConnect(const boost::system::error_code &);
        void HandleWriteRequest(const boost::system::error_code &);
        void HandleRead(const boost::system::error_code &);
        void HandleReadContent(const boost::system::error_code &);
        void HandleStop();

        // Pre-determined
        const std::string k_OnServer;

        std::string m_WithStatus;
        std::string m_WithData;
        std::string m_RootPath;

        // Integrity atomics and/or mutexes
        std::atomic<Statuses> m_CurrentStatus;
        std::atomic<bool> m_WorkerStopped;

        // Asio
        asio::io_service & m_IoService;
        asio::ip::tcp::resolver m_DnsResolver;
        asio::ip::tcp::socket m_TcpSocket;
        asio::streambuf m_RequestBuffer;
        asio::streambuf m_ResponseBuffer;

        std::vector<int> m_ReceivedChunk;

        ChunksManager & m_ChunksManager;
    };
}