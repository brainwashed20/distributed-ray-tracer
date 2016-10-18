#include "Client.h"
#include "Logger.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "Serialization.h"

BOOST_CLASS_EXPORT_GUID(RayTracer::ChessBoard, "chess_board");
BOOST_CLASS_EXPORT_GUID(RayTracer::Color, "color");
BOOST_CLASS_EXPORT_GUID(RayTracer::ConcreteBlock, "concrete_block");
BOOST_CLASS_EXPORT_GUID(RayTracer::Cuboid, "cuboid");
BOOST_CLASS_EXPORT_GUID(RayTracer::Cylinder, "cylinder");
BOOST_CLASS_EXPORT_GUID(RayTracer::Dodecahedron, "dodecahedron");
BOOST_CLASS_EXPORT_GUID(RayTracer::Icosahedron, "icosahedron");
BOOST_CLASS_EXPORT_GUID(RayTracer::PixelData, "pixel_data");
BOOST_CLASS_EXPORT_GUID(RayTracer::ImageBuffer, "image_buffer");
BOOST_CLASS_EXPORT_GUID(RayTracer::Intersection, "intersection");
BOOST_CLASS_EXPORT_GUID(RayTracer::LightSource, "light_source");
BOOST_CLASS_EXPORT_GUID(RayTracer::Optics, "optics");
BOOST_CLASS_EXPORT_GUID(RayTracer::Planet, "planet");
BOOST_CLASS_EXPORT_GUID(RayTracer::SetComplement, "set_complement");
BOOST_CLASS_EXPORT_GUID(RayTracer::SetIntersection, "set_intersection");
BOOST_CLASS_EXPORT_GUID(RayTracer::SetDifference, "set_difference");
BOOST_CLASS_EXPORT_GUID(RayTracer::SetUnion, "set_union");
BOOST_CLASS_EXPORT_GUID(RayTracer::SolidObject_Reorientable, "solid_object_reorientable");
BOOST_CLASS_EXPORT_GUID(RayTracer::Sphere, "sphere");
BOOST_CLASS_EXPORT_GUID(RayTracer::Saturn, "saturn");
BOOST_CLASS_EXPORT_GUID(RayTracer::Spheroid, "spheroid");
BOOST_CLASS_EXPORT_GUID(RayTracer::ThinRing, "thin_ring");
BOOST_CLASS_EXPORT_GUID(RayTracer::ThinDisc, "thin_disc");
BOOST_CLASS_EXPORT_GUID(RayTracer::Torus, "torus");
BOOST_CLASS_EXPORT_GUID(RayTracer::TriangleMesh, "triangle_mesh");
BOOST_CLASS_EXPORT_GUID(RayTracer::Vector, "vector");

BOOST_SERIALIZATION_ASSUME_ABSTRACT(RayTracer::SolidObject_BinaryOperator);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(RayTracer::SolidObject);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(RayTracer::Taggable);

// Aliasing
namespace algo = boost::algorithm;

namespace RayClient
{
    Client::Client(ChunksManager& chunksManager) :
        m_ChunksManager(chunksManager),
        k_OnServer(chunksManager.m_OnServer),
        m_RootPath("..\\res"),
        m_CurrentStatus(NO_STATUS),
        m_WorkerStopped(false),
        m_IoService(chunksManager.m_IoService),
        m_DnsResolver(chunksManager.m_IoService),
        m_TcpSocket(chunksManager.m_IoService)
    {
        // Notify of properly constructed
        m_CurrentStatus = AWAITING_WORK;
    }

    Client::~Client()
    {
        // If not empty, pushback
        (m_WorkerStopped.load(std::memory_order_relaxed) == false) ? (void(HandleStop())) : (void(0));
    }

    /**
    * Used to stop the worker from anything it does at the current moment, and is
    * used by the class internals to close sockets if a given time passes and
    * the connection/socket has not responded;
    */
    void Client::HandleStop()
    {
        // Set these
        m_WorkerStopped = true;
        boost::system::error_code ignoredCode;

        // Close it
        if (m_TcpSocket.is_open())
        {
            m_TcpSocket.shutdown(asio::ip::tcp::socket::shutdown_both, ignoredCode);
            m_TcpSocket.close(ignoredCode);
        }
    }

    /**
    * Method accepts the data to be POST'ed to the given ETS server. The
    * class is configured for GLOT purposes, but the basic framework can be
    * used for any HTTP request to the necessary modifications or
    * abstractizations. It starts a DNS resolve query, initiating the
    * chain of requests;
    */
    void Client::AsyncRequest(const std::string & withStatus, const std::string & withData)
    {
        // Try to resolve the IPv4/v6 for the given hostname
        asio::ip::tcp::resolver::query dnsQuery(k_OnServer, "120");

        m_WithStatus = withStatus;
        m_WithData = withData;

        // Go for it
        m_CurrentStatus = RESOLVING_DNS;
        m_DnsResolver.async_resolve(dnsQuery,
            boost::bind(
                &Client::HandleResolve, this,
                asio::placeholders::error, asio::placeholders::iterator
            )
        );

    }

    void Client::HandleResolve(const boost::system::error_code &errorCode, asio::ip::tcp::resolver::iterator endpointIterator)
    {
        if (m_WorkerStopped.load(std::memory_order_relaxed) == true)
        {
            return;
        }

        // Check
        if (!(errorCode)
        && endpointIterator != asio::ip::tcp::resolver::iterator())
        {
            // Try a connection to the endpoints
            m_CurrentStatus = CONNECTING_TO_ENDPOINT;
            asio::async_connect(m_TcpSocket,
                endpointIterator++,
                boost::bind(
                    &Client::HandleConnect,
                    this, asio::placeholders::error
                )
            );
        }
        else
        {
            HandleStop();

            m_CurrentStatus = FAILED_ON_RESOLVE;

            Logger::WriteLog("HandleResolve: Network failed on resolve! Error code is: " + Utils::ToString(errorCode.value()));
        }
    }

    void Client::HandleConnect(const boost::system::error_code &errorCode)
    {
        // Check
        if (m_WorkerStopped.load(std::memory_order_relaxed) == true)
        {
            return;
        }

        if (!(errorCode))
        {
            std::ostream writeOnSocket(&m_RequestBuffer);

            writeOnSocket
                << m_WithStatus << system::SEPARATOR
                << m_WithData.size() << system::SEPARATOR
                << m_WithData << system::SEPARATOR
                << system::NIX_EOL << system::NIX_EOL;

            m_CurrentStatus = WRITING_TO_SOCKET;
            asio::async_write(m_TcpSocket,
                m_RequestBuffer,
                boost::bind(
                    &Client::HandleWriteRequest, this,
                    asio::placeholders::error
                )
            );
        }
        else
        {
            HandleStop();

            m_CurrentStatus = FAILED_ON_CONNECT;

            Logger::WriteLog("HandleConnect: Network failed on connect! Error code is: " + Utils::ToString(errorCode.value()));
        }
    }

    void Client::HandleWriteRequest(const boost::system::error_code &errorCode)
    {
        // Check
        if (m_WorkerStopped.load(std::memory_order_relaxed) == true)
        {
            return;
        }

        // Check
        if (!(errorCode))
        {
            // We need to read the socket now
            m_CurrentStatus = READING_CONTENT;

            asio::async_read_until(m_TcpSocket,
                m_ResponseBuffer,
                system::NIX_EOL,
                boost::bind(
                    &Client::HandleRead,
                    this, asio::placeholders::error
                )
            );
        }
        else
        {
            // Log this down
            HandleStop();

            m_CurrentStatus = FAILED_ON_WRITE_TO_SOCKET;

            Logger::WriteLog("HandleWriteRequest: Network failed on write! Error code is: " + Utils::ToString(errorCode.value()));
        }
    }

    void Client::HandleRead(const boost::system::error_code &errorCode)
    {
        // Check
        if (m_WorkerStopped.load(std::memory_order_relaxed) == true)
        {
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

                    // Check
                    if (algo::equals(system::PIXEL_TO_PROCESS, status))
                    {
                        m_CurrentStatus = OK_STATUS;

                        m_ReceivedChunk.clear();

                        // deserialize
                        {
                            std::stringstream objectBuffer(bodyContent);
                            boost::archive::text_iarchive inArchive(objectBuffer);
                            inArchive >> m_ReceivedChunk;
                        }

                        m_ChunksManager.QueueInputPixels(m_ReceivedChunk);

                        Logger::WriteLog("Received new pixels coordinates to process!");

                    }
                    else if (algo::equals(system::SEND_SCENE, status))
                    {
                        m_CurrentStatus = OK_STATUS;

                        m_ChunksManager.SaveScene(bodyContent);

                        Logger::WriteLog("Received scene!");
                    }
                    else if (algo::equals(system::BYE_BYE, status))
                    {
                        m_CurrentStatus = OK_STATUS;

                        m_ChunksManager.m_PixelsProcessDone = true;

                        Logger::WriteLog("Job Done!");
                    }
                }
            }
        }
        else
        {
            // Log this down
            HandleStop();

            m_CurrentStatus = FAILED_ON_READ;

            Logger::WriteLog("HandleRead: Network failed on read! Error code is: " + Utils::ToString(errorCode.value()));
        }
    }

    Client::Statuses Client::GetStatus()
    {
        return m_CurrentStatus;
    }
}