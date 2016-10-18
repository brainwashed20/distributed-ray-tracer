#include "Server.h"
#include "Serialization.h"
#include "RayTracer.h"

#include <fstream>
#include <sstream>
#include <ostream>

#include "Client.h"
#include "MutexedQueue.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "UnitTests.h"

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

void SetupScene(boost::shared_ptr<RayTracer::Scene> scene)
{
     using namespace RayTracer;
     
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

void ChessBoardTest(const char *outPngFileName, size_t pixelsWide, size_t pixelsHigh, double zoom, size_t antiAliasFactor)
{
    // using namespace RayTracer;
    // 
    // boost::shared_ptr<RayTracer::Scene> scene = boost::shared_ptr<Scene>(new Scene(Color(0.0, 0.0, 0.0)));
    // 
    // SetupScene(scene);
    // 
    // std::stringstream objectBuffer;
    // 
    // 
    // 
    // 
    // // SEND DATA
    // 
    // // boost::archive::text_oarchive outArchive(objectBuffer);
    // // outArchive << *scene;
    // // myClient.HandleSceneReceived(RayClient::Utils::ToVectorChar(objectBuffer));
    // 
    // // create inputPixels
    // int chunkSize = 5000;
    // int numberOfChunks = 0;
    // 
    // std::vector<std::vector<PixelCoordinates>> inputPixels;
    // inputPixels.push_back(std::vector<PixelCoordinates>());
    // 
    // int pixelCount = -1;
    // 
    // const size_t largePixelsWide = antiAliasFactor * pixelsWide;
    // const size_t largePixelsHigh = antiAliasFactor * pixelsHigh;
    // 
    // for (size_t i = 0; i < largePixelsWide; ++i)
    // {
    //     for (size_t j = 0; j < largePixelsHigh; ++j)
    //     {
    //         if (++pixelCount == chunkSize)
    //         {
    //             pixelCount = -1;
    //             inputPixels.push_back(std::vector<PixelCoordinates>());
    //             inputPixels.back().reserve(chunkSize);
    //             inputPixels.back().push_back(PixelCoordinates(i, j));
    //         }
    //         else
    //         {
    //             inputPixels.back().push_back(PixelCoordinates(i, j));
    //         }
    //     }
    // }
    // 
    // for (auto pixels : inputPixels)
    // {
    //     // save data
    //     {
    //         objectBuffer = std::stringstream();
    //         boost::archive::text_oarchive outArchive(objectBuffer);
    //         outArchive << pixels;
    //         myClient.HandlePixelsReceived(RayClient::Utils::ToVectorChar(objectBuffer));
    //     }
    // }
    // 
    // 
    // 
    // 
    // 
    // RayTracer::ImageBuffer outputImageBuffer = RayTracer::ImageBuffer(largePixelsWide, largePixelsHigh, Color(0.0, 0.0, 0.0));
    // 
    // for (int i = 0; i < computedPixels.size(); ++i)
    // {
    //     for (int j = 0; j < computedPixels[i].size(); ++j)
    //     {
    //         PixelData& pixel = outputImageBuffer.Pixel(computedPixels[i][j].second.i, computedPixels[i][j].second.j);
    //         pixel.color = computedPixels[i][j].first.color;
    //         pixel.isAmbiguous = computedPixels[i][j].first.isAmbiguous;
    //     }
    // }




    //for (size_t i = 0; i < largePixelsWide; ++i)
    //{
    //    for (size_t j = 0; j < largePixelsHigh; ++j)
    //    {
    //        PixelCoordinates pixelCoordinates(i, j);
    //
    //        // save data
    //        {
    //            objectBuffer.flush();
    //            boost::archive::text_oarchive outArchive(objectBuffer);
    //            outArchive << pixelCoordinates;
    //        }
    //
    //        PixelCoordinates pixelsCoordinatesLoaded;
    //
    //        // load data    
    //        {
    //            
    //            boost::archive::text_iarchive inArchive(objectBuffer);
    //            inArchive >> pixelsCoordinatesLoaded;
    //        }
    //
    //        imageBuffer.Pixel(pixelsCoordinatesLoaded.i, pixelsCoordinatesLoaded.j) = 
    //            scene_loaded.GetPixelAt(pixelsCoordinatesLoaded.i, pixelsCoordinatesLoaded.j, pixelsWide, pixelsHigh, zoom, antiAliasFactor);
    //    }
    //}
    //

    // scene->HandleAmbigousPixels(outputImageBuffer, pixelsWide, pixelsHigh, zoom, antiAliasFactor);
    // scene->CreateImage("chessboard.png", outputImageBuffer, pixelsWide, pixelsHigh, zoom, antiAliasFactor);
}

int main(int argc, char* argv[])
{
    // ChessBoardTest("chessboard.png", 800, 600, 4.5, 3);

    return 0;
}

