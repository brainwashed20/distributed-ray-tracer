#include "Server.h"
#include "Serialization.h"
#include "RayTracer.h"

#include <thread>
#include <condition_variable>

std::shared_ptr<RayServer::Server> server;
boost::shared_ptr<RayTracer::Scene> scene = boost::shared_ptr<RayTracer::Scene>(new RayTracer::Scene(RayTracer::Color(0.196078, 0.8, 0.196078, 7.0e-6)));
std::deque<std::string> serializedData;
std::deque<std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>>> outputQueue;
std::condition_variable notifyNetworkIsDone;
std::mutex waitOnNetworkDone;

#if defined(_NETWORK_UNIT_TESTS)
std::deque<std::vector<RayTracer::PixelCoordinates>> plainData;
#endif

std::string sceneName;
const size_t pixelsWide = 800;
const size_t pixelsHigh = 600;
const double zoom = 3;
const size_t antiAliasFactor = 3;
const size_t largePixelsWide = antiAliasFactor * pixelsWide;
const size_t largePixelsHigh = antiAliasFactor * pixelsHigh;

BOOST_CLASS_EXPORT_GUID(RayTracer::ChessBoard, "chess_board");
BOOST_CLASS_EXPORT_GUID(RayTracer::Color, "color");
BOOST_CLASS_EXPORT_GUID(RayTracer::ConcreteBlock, "concrete_block");
BOOST_CLASS_EXPORT_GUID(RayTracer::Cuboid, "cuboid");
BOOST_CLASS_EXPORT_GUID(RayTracer::Cylinder, "cylinder");
BOOST_CLASS_EXPORT_GUID(RayTracer::Dodecahedron, "dodecahedron");
BOOST_CLASS_EXPORT_GUID(RayTracer::Icosahedron, "icosahedron");
BOOST_CLASS_EXPORT_GUID(RayTracer::PixelData, "pixel_data");
BOOST_CLASS_EXPORT_GUID(RayTracer::PixelCoordinates, "pixel_coordinates");
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

void SetupChessBoardScene()
{
    using namespace RayTracer;

    sceneName = "chessboard.png";

    // Dimensions of the chess board.

    const double innerSize = 4.00;
    const double xBorderSize = 1.00;
    const double yBorderSize = 1.00;
    const double thickness = 0.25;

    const Color lightSquareColor = Color(0.0, 0.0, 0.0);
    const Color darkSquareColor = Color(0.36, 0.25, 0.20);
    const Color borderColor = Color(0.50, 0.30, 0.10);

    // Create the chess board and add it to the scene.

    boost::shared_ptr<SolidObject>board = boost::shared_ptr<SolidObject>(new ChessBoard(
        innerSize,
        xBorderSize,
        yBorderSize,
        thickness,
        lightSquareColor,
        darkSquareColor,
        borderColor));

    board->Move(-0.35, 0.1, -20.0);

    board->RotateZ(+11.0);
    board->RotateX(-62.0);

    scene->AddSolidObject(board);

    // Put transparent spheres above the chess board.
    struct sphere_info
    {
        Vector center;
        double radius;
    };

    const sphere_info sinfo[] =
    {
        { Vector(-1.8, 0.2, -17.0), 0.30 },
        { Vector(0.0, 0.0, -17.2), 0.72 },
        { Vector(+1.8, -0.2, -17.0), 0.60 }
    };

    const size_t numSpheres = sizeof(sinfo) / sizeof(sinfo[0]);
    for (size_t i = 0; i < numSpheres; ++i)
    {
        boost::shared_ptr<SolidObject>sphere = boost::shared_ptr<SolidObject>(new Sphere(sinfo[i].center, sinfo[i].radius));
        sphere->SetOpacity(0.17);
        sphere->SetMatteGlossBalance(0.95, Color(0.4, 0.5, 0.7), Color(0.8, 1.0, 0.7));
        scene->AddSolidObject(sphere);
    }

    // Add light sources.

    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +25.0, +50.0), Color(0.4, 0.4, 0.1, 1.0))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, +40.0), Color(0.5, 0.5, 1.5, 1.0))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-25.0, +30.0, +40.0), Color(0.3, 0.2, 0.1, 1.0))));
}

void SetupDodecadhronScene()
{
    using namespace RayTracer;

    sceneName = "deodecadhron.png";

    Optics optics;
    Dodecahedron* dodecahedron = new Dodecahedron(Vector(0.0, 0.0, -50.0), 1.0, optics);
    dodecahedron->RotateX(-12.0);
    dodecahedron->RotateY(-7.0);

    // Create a sphere that overlaps with the dodecahedron.
    const Vector sphereCenter(Vector(+1.0, 0.0, -50.0));
    Sphere* sphere = new Sphere(sphereCenter, 1.8);

    boost::shared_ptr<SolidObject>isect =
        boost::shared_ptr<SolidObject>(new SetIntersection(sphereCenter, dodecahedron, sphere));

    isect->RotateY(-30.0);

    scene->AddSolidObject(isect);

    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0),  Color(0.5, 0.8, 1.2, 0.5))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.3, 0.1, 0.5, 0.5))));
}

void SetupSphereScene()
{
    using namespace RayTracer;

    sceneName = "sphere.png";

    const Vector sphereCenter(0.0, 0.0, -40.0);
    const Vector lightDisplacement(-30.0, +120.0, +50.0);

    boost::shared_ptr<SolidObject> sphere = boost::shared_ptr<SolidObject>(new Sphere(sphereCenter, 5.5));
    sphere->SetFullMatte(Color(0.2, 0.6, 0.1));
    scene->AddSolidObject(sphere);
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(sphereCenter + lightDisplacement, Color(0.5, 1.0, 0.3))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+100.0, +120.0, -70.0), Color(1.0, 1.0, 0.3, 1.0))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+3.0, +13.0, +80.0), Color(0.5, 0.8, 1.2, 0.5))));
}

void SetupBlockTestScene()
{
    using namespace RayTracer;

    sceneName = "block.png";

    boost::shared_ptr<SolidObject> block = boost::shared_ptr<SolidObject>(new ConcreteBlock(Vector(0.0, 0.0, -160.0), Optics(Color(0.5, 0.5, 0.5))));
    block->RotateX(-18.0);
    block->RotateY(-15.0);
    block->RotateZ(-90.0);
    scene->AddSolidObject(block);

    boost::shared_ptr<SolidObject> sphere = boost::shared_ptr<SolidObject>(new Sphere(Vector(10.0, 3.0, -100.0), 3.5));
    sphere->SetFullMatte(Color(0.35, 0.35, 0.67));
    scene->AddSolidObject(sphere);

    boost::shared_ptr<SolidObject> sphere2 = boost::shared_ptr<SolidObject>(new Sphere(Vector(-10.0, 6.0, -100.0), 2));
    sphere2->SetOpacity(0.5);
    sphere2->SetMatteGlossBalance(0.5, Color(0.4, 0.5, 0.7), Color(0.8, 1.0, 0.7));
    scene->AddSolidObject(sphere2);

    // Add a light source to illuminate the objects in the scene; otherwise we won't see anything!
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+20.0, +20.0, +80.0), Color(0.5, 0.1, 0.1, 0.15))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+100.0, +120.0, -70.0), Color(0.2, 0.5, 0.4, 1.00))));
    scene->AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+3.0, +13.0, +80.0), Color(0.6, 0.5, 0.3, 1.20))));
}

void SerializeData()
{
    using namespace RayTracer;

    std::stringstream objectBuffer;

    {
        boost::archive::text_oarchive outArchive(objectBuffer);
        outArchive << *scene;
    }

    server->LoadScene(objectBuffer.str());

    std::vector<int> chunk;
    
    // std::vector<std::vector<int>> allChunks;

    // create inputPixels
    int linesToSend = 15;
    
    const size_t largePixelsWide = antiAliasFactor * pixelsWide;
    const size_t largePixelsHigh = antiAliasFactor * pixelsHigh;
    
    for (size_t i = 0; i < largePixelsWide; ++i)
    {
        if (i && i % linesToSend == 0)
        {
            chunk.shrink_to_fit();
    
            // serialize
            {
                objectBuffer = std::stringstream();
                boost::archive::text_oarchive outArchive(objectBuffer);
                outArchive << chunk;
    
                serializedData.push_back(objectBuffer.str());
            }

        #if defined(_NETWORK_UNIT_TESTS)
            plainData.push_back(chunk);
        #endif

            // allChunks.push_back(chunk);

            chunk.clear();
    
            chunk.push_back(i);
        }
        else
        {
            chunk.push_back(i);
        }
    }

    if (!chunk.empty())
    {
        chunk.shrink_to_fit();

        // serialize
        {
            objectBuffer = std::stringstream();
            boost::archive::text_oarchive outArchive(objectBuffer);
            outArchive << chunk;

            serializedData.push_back(objectBuffer.str());
        }
    }
}

void ComputeReceivedDataAndCreateImage()
{
    using namespace RayTracer;

    std::unique_lock<std::mutex> guardLock(waitOnNetworkDone);

    while (server->IsWorkDone() == false)
    {
        notifyNetworkIsDone.wait(guardLock);

        RayTracer::ImageBuffer outputImageBuffer = RayTracer::ImageBuffer(largePixelsWide, largePixelsHigh, Color(0.0, 0.0, 0.0));

        std::stringstream objectBuffer;

        while (!outputQueue.empty())
        {
            auto completedChunk = outputQueue.back();

            outputQueue.pop_back();

            for (const auto& computedPixel : completedChunk)
            {
                PixelData& pixel = outputImageBuffer.Pixel(computedPixel.first.i, computedPixel.first.j);
                pixel.color = computedPixel.second.color;
                pixel.isAmbiguous = computedPixel.second.isAmbiguous;
            }
        }

        scene->HandleAmbigousPixels(outputImageBuffer, pixelsWide, pixelsHigh, zoom, antiAliasFactor);
        scene->CreateImage(sceneName, outputImageBuffer, pixelsWide, pixelsHigh, zoom, antiAliasFactor);
    }
}

#if defined(_NETWORK_UNIT_TESTS)
    void UnitTest()
    {
        SerializeData();

        std::vector<std::pair<RayTracer::PixelCoordinates, RayTracer::PixelData>> computedChunk;

        while (!plainData.empty())
        {
            auto pixelChunk = plainData.front();

            plainData.pop_front();

            computedChunk.clear();

            for (const auto& pixelCoord : pixelChunk)
            {
                RayTracer::PixelData pixelColor = scene->GetPixelAt(pixelCoord.i, pixelCoord.j, pixelsWide, pixelsHigh, zoom, antiAliasFactor);

                computedChunk.push_back(std::make_pair(pixelCoord, pixelColor));
            }

            computedChunk.shrink_to_fit();
            outputQueue.push_back(computedChunk);
        }

        ComputeReceivedDataAndCreateImage();

        std::cout << "Unit tests done..";
    }
#endif

int main()
{
    std::string serverAdress = "localhost";
    
    boost::asio::io_service ioService;
    std::vector<std::thread> networkThreads;

    server = std::shared_ptr<RayServer::Server>(new RayServer::Server(std::ref(ioService), std::ref(serializedData), std::ref(outputQueue), std::ref(notifyNetworkIsDone), std::ref(waitOnNetworkDone)));
    
    // SetupChessBoardScene();

    // SetupDodecadhronScene();

    // SetupSphereScene();

    SetupBlockTestScene();

    SerializeData();

    server->Run();

    auto objConcurrency(1);
    for (auto i = 0; i < objConcurrency; ++i)
    {
        networkThreads.push_back(std::thread(std::bind(static_cast<size_t(boost::asio::io_service::*) ()> (&boost::asio::io_service::run), std::ref(ioService))));
    }

    std::thread lastStep(ComputeReceivedDataAndCreateImage);
    
    for (auto i = 0; i < objConcurrency; ++i)
    {
        networkThreads[i].join();
    }

    lastStep.join();

    // UnitTest();

    return 0;
}