#include "ClientConnection.h"
#include "Logger.h"
#include "Utils.h"
#include "Server.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "Serialization.h"

// Aliasing
namespace algo = boost::algorithm;

namespace RayServer
{
    ClientConnection::ClientConnection(boost::asio::io_service& ios) :
        m_TcpSocket(ios),
        m_ConnectionStopped(false),
        m_CurrentStatus(NO_STATUS),
        m_ClientID("unkonwn")
    {

    }

    void ClientConnection::Start(Server* server)
    {
        Logger::WriteLog("Client: " + Utils::ToString(m_TcpSocket.remote_endpoint()) + " connected.");

        m_ClientID = Utils::ToString(m_TcpSocket.remote_endpoint());

        m_CurrentStatus = READING_HEADERS;

        // read headers
        asio::async_read_until(m_TcpSocket,
            m_ResponseBuffer,
            system::NIX_EOL,
            boost::bind(
                &ClientConnection::HandleRead, shared_from_this(),
                server,
                asio::placeholders::error
            )
        );
    }

    void ClientConnection::HandleRead(Server* server, const boost::system::error_code &errorCode)
    {
        if (m_ConnectionStopped)
        {
            Logger::WriteLog(m_ClientID + system::HASHTAG + "connection stopped");

            return;
        }

        // Check
        if (!(errorCode))
        {
            // Process the response headers
            std::istream responseBufferStream(&m_ResponseBuffer);

            // We've reading more
            if (m_ResponseBuffer.size() > 0)
            {
                std::ostringstream contentBuffer;
                contentBuffer << &m_ResponseBuffer;
                std::string content(contentBuffer.str());

                std::string copyContent(content);
                std::string status;
                unsigned int bodySize;
                std::string bodyContent;

                status = Utils::GetUntilSeparator(copyContent);
                if (!status.empty())
                {
                    std::string bodySizeStr = Utils::GetUntilSeparator(copyContent);

                    if (!bodySizeStr.empty())
                    {
                        unsigned int bodySize = std::stoi(bodySizeStr);

                        bodyContent = Utils::GetUntilSeparator(copyContent);
                    }

                    if (server->m_IsWorkDone == true)
                    {
                        HandleBye(server);

                        m_ConnectionStopped = true;

                        Logger::WriteLog("Network workers finished processing!");
                    }

                    if (m_CurrentStatus == INPUT_QUEUE_EMPTY)
                    {
                        const bool isWorkDone(server->m_NumOfNetworkPackages.load(std::memory_order_relaxed) == server->m_OutputQueue.size());

                        if (isWorkDone)
                        {
                            HandleBye(server);

                            Logger::WriteLog("Network workers finished processing!");

                            m_ConnectionStopped = true;

                            server->m_IsWorkDone = true;
                            server->m_NotifyNetworkIsDone.notify_one();
                        }
                    }

                    if (!m_ConnectionStopped)
                    {
                        // Check
                        if (algo::equals(system::COMPUTED_PIXELS, status))
                        {
                            m_CurrentStatus = COMPUTED_PIXELS;

                            m_ReceivedChunk.clear();

                            // deserialize
                            {
                                std::stringstream objectBuffer(bodyContent);
                                boost::archive::text_iarchive inArchive(objectBuffer);
                                inArchive >> m_ReceivedChunk;
                            }

                            // save computed chunk
                            {
                                std::lock_guard<std::mutex> buffer(server->m_OutputQueueMutex);
                                server->m_OutputQueue.push_back(m_ReceivedChunk);
                            }

                            HandlePixelRequest(server);

                            Logger::WriteLog(m_ClientID + system::HASHTAG + "sent computed pixels");
                        }
                        else if (algo::equals(system::SEND_SCENE, status))
                        {
                            m_CurrentStatus = SENDING_SCENE;

                            HandleSceneRequest(server);

                            Logger::WriteLog(m_ClientID + system::HASHTAG + "sending scene...");
                        }
                        else if (algo::equals(system::RECEIVED_SCENE, status))
                        {
                            m_CurrentStatus = SCENE_PROPRELY_SENT;

                            Logger::WriteLog(m_ClientID + system::HASHTAG + "scene received!");

                            HandlePixelRequest(server);
                        }
                        else
                        {
                            Logger::WriteLog(m_ClientID + system::HASHTAG + "Failed on reading headers: unknown header message");

                            m_CurrentStatus = FAILED_ON_READ_HEADERS;
                        }
                    }
                }
                else
                {
                    Logger::WriteLog(m_ClientID + system::HASHTAG + "Status empty");

                    m_CurrentStatus = FAILED_ON_READ_HEADERS;
                }
            }
            else
            {
                // Logger::WriteLog(m_ClientID + system::HASHTAG + "Response empty!");
            }
        }
        else
        {
            m_ConnectionStopped = true;
        }
    }

    void ClientConnection::HandlePixelRequest(Server* server)
    {
        std::string withData;

        // load chunk
        {
            std::lock_guard<std::mutex> buffer(server->m_InputQueueMutex);
            if (!server->m_InputQueue.empty())
            {
                withData = server->m_InputQueue.back();
                server->m_InputQueue.pop_back();
            }
            else
            {
                m_CurrentStatus = INPUT_QUEUE_EMPTY;
            }
        }
        
        if (m_CurrentStatus != INPUT_QUEUE_EMPTY)
        {
            std::ostream writeOnSocket(&m_ResponseBuffer);

            writeOnSocket
                << system::PIXEL_TO_PROCESS << system::SEPARATOR
                << withData.size() << system::SEPARATOR
                << withData << system::SEPARATOR
                << system::NIX_EOL;

            m_CurrentStatus = WRITING_TO_SOCKET;

            asio::async_write(m_TcpSocket,
                m_ResponseBuffer,
                boost::bind(
                    &ClientConnection::HandleRead, shared_from_this(),
                    server, asio::placeholders::error
                )
            );
        }
        else
        {
            bool isWorkDone;

            {
                std::lock_guard<std::mutex> buffer(server->m_InputQueueMutex);
                isWorkDone = (server->m_NumOfNetworkPackages.load(std::memory_order_relaxed) == server->m_OutputQueue.size());
            }

            if (isWorkDone)
            {
                HandleBye(server);

                Logger::WriteLog("Network workers finished processing!");

                Logger::WriteLog("Construncting image...");

                m_ConnectionStopped = true;

                server->m_IsWorkDone = true;

                server->m_NotifyNetworkIsDone.notify_one();
            }
        }
    }

    void ClientConnection::HandleSceneRequest(Server* server)
    {
        std::string withData = server->m_SerializedScene;

        std::ostream writeOnSocket(&m_ResponseBuffer);

        writeOnSocket
            << system::SEND_SCENE << system::SEPARATOR
            << withData.size() << system::SEPARATOR
            << withData << system::SEPARATOR
            << system::NIX_EOL;

        m_CurrentStatus = WRITING_TO_SOCKET;
        
        asio::async_write(m_TcpSocket,
            m_ResponseBuffer,
            boost::bind(
                &ClientConnection::HandleRead, shared_from_this(),
                server, asio::placeholders::error
            )
        );
    }

    void ClientConnection::HandleBye(Server* server)
    {
        std::ostream writeOnSocket(&m_ResponseBuffer);

        writeOnSocket
            << system::BYE_BYE << system::SEPARATOR
            << system::BYE_BYE.size() << system::SEPARATOR
            << system::BYE_BYE << system::SEPARATOR
            << system::NIX_EOL;

        m_CurrentStatus = WRITING_TO_SOCKET;

        asio::async_write(m_TcpSocket,
            m_ResponseBuffer,
            boost::bind(
                &ClientConnection::HandleRead, shared_from_this(),
                server, asio::placeholders::error
            )
        );
    }
}

