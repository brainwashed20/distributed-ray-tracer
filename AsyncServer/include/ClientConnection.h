#pragma once

#include "Requirements.h"

namespace RayTracer
{
    class PixelData;
    class PixelCoordinates;
}

namespace RayServer
{
    class ClientConnection :
        public std::enable_shared_from_this <ClientConnection>
    {

    friend class Server;

    public:
        ClientConnection(asio::io_service& ios);

    private:

        // HTTP statuses (mangled with our own)
        enum Statuses
        {
            NO_STATUS = 0,
            AWAITING_WORK = 1,
            RESOLVING_DNS = 2,
            FAILED_ON_RESOLVE = 3,
            CONNECTING_TO_ENDPOINT = 4,
            FAILED_ON_CONNECT = 5,
            WRITING_TO_SOCKET = 6,
            FAILED_ON_WRITE_TO_SOCKET = 7,
            READING_STATUS_LINE = 8,
            FAILED_ON_READ_STATUS_LINE = 9,
            INVALID_HTTP_RESPONSE = 10,
            NON_200_STATUS_CODE = 11,
            READING_HEADERS = 12,
            FAILED_ON_READ_HEADERS = 13,
            NON_200_ETS_RESPONSE = 14,
            READING_CONTENT = 15,
            FAILED_ON_READ_CONTENT = 16,
            OK_STATUS = 200,
            PIXELS_REQUEST = 995,
            COMPUTED_PIXELS = 996,
            SENDING_SCENE = 997,
            SCENE_PROPRELY_SENT = 998,
            INPUT_QUEUE_EMPTY = 999
        };

        // asio
        asio::ip::tcp::socket m_TcpSocket;
        asio::streambuf m_RequestBuffer;
        asio::streambuf m_ResponseBuffer;

        // Methods
        void Start(Server*);
        void HandleRead(Server*, const boost::system::error_code&);
        void HandlePixelRequest(Server*);
        void HandleSceneRequest(Server*);
        void HandleBye(Server*);

        std::atomic<Statuses> m_CurrentStatus;
        bool m_ConnectionStopped;

        std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>> m_ReceivedChunk;

        std::string m_ClientID;
    };
}