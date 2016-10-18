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

namespace RayTracer
{
    namespace LocalTests
    {
        void BlockTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.196078, 0.8, 0.196078, 7.0e-6));

            boost::shared_ptr<SolidObject> block = boost::shared_ptr<SolidObject>(new ConcreteBlock(Vector(0.0, 0.0, -160.0), Optics(Color(0.5, 0.5, 0.5))));
            block->RotateX(-18.0);
            block->RotateY(-15.0);
            block->RotateZ(-90.0);
            scene.AddSolidObject(block);

            boost::shared_ptr<SolidObject> sphere = boost::shared_ptr<SolidObject>(new Sphere(Vector(10.0, 3.0, -100.0), 3.5));
            sphere->SetFullMatte(Color(0.35, 0.35, 0.67));
            scene.AddSolidObject(sphere);

            boost::shared_ptr<SolidObject> sphere2 = boost::shared_ptr<SolidObject>(new Sphere(Vector(-10.0, 6.0, -100.0), 2));
            sphere2->SetOpacity(0.5);
            sphere2->SetMatteGlossBalance(0.5, Color(0.4, 0.5, 0.7), Color(0.8, 1.0, 0.7));
            scene.AddSolidObject(sphere2);

            // Add a light source to illuminate the objects in the scene; otherwise we won't see anything!
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+20.0, +20.0, +80.0), Color(0.5, 0.1, 0.1, 0.15))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+100.0, +120.0, -70.0), Color(0.2, 0.5, 0.4, 1.00))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+3.0, +13.0, +80.0), Color(0.6, 0.5, 0.3, 1.20))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            // Generate a PNG file that displays the scene...
            const char *filename = "block.png";
            scene_loaded.SaveImage(filename, 800, 600, 4.5, 4);
            std::cout << "Wrote " << filename << std::endl;
        }

        void SphereTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.196078, 0.8, 0.196078, 7.0e-6));

            const Vector sphereCenter(0.0, 0.0, -40.0);
            const Vector lightDisplacement(-30.0, +120.0, +50.0);

            boost::shared_ptr<SolidObject> sphere = boost::shared_ptr<SolidObject>(new Sphere(sphereCenter, 5.5));
            sphere->SetFullMatte(Color(0.6, 0.2, 0.2));
            scene.AddSolidObject(sphere);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(sphereCenter + lightDisplacement, Color(0.5, 1.0, 0.3))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+100.0, +120.0, -70.0), Color(0.2, 0.5, 0.4, 1.00))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+3.0, +13.0, +80.0), Color(0.6, 0.5, 0.3, 1.20))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            // Generate a PNG file that displays the scene...
            const char *filename = "sphere.png";
            scene_loaded.SaveImage(filename, 800, 500, 3.0, 3);
            std::cout << "Wrote " << filename << std::endl;
        }


        void TorusTest(const char* filename, double glossFactor)
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            const Color white(1.0, 1.0, 1.0);
            Torus* torus1 = new Torus(3.0, 1.0);
            torus1->SetMatteGlossBalance(glossFactor, white, white);
            torus1->Move(+1.5, 0.0, 0.0);

            Torus* torus2 = new Torus(3.0, 1.0);
            torus2->SetMatteGlossBalance(glossFactor, white, white);
            torus2->Move(-1.5, 0.0, 0.0);
            torus2->RotateX(-90.0);

            boost::shared_ptr<SolidObject> doubleTorus = boost::shared_ptr<SolidObject>(
                new SetUnion(Vector(0.0, 0.0, 0.0), torus1, torus2));

            scene.AddSolidObject(doubleTorus);

            doubleTorus->Move(0.0, 0.0, -50.0);
            doubleTorus->RotateX(-45.0);
            doubleTorus->RotateY(-30.0);

            // Add a light source to illuminate the objects 
            // in the scene; otherwise we won't see anything!
            scene.AddLightSource(
                boost::shared_ptr<LightSource>(new LightSource(
                Vector(-45.0, +10.0, +50.0),
                Color(1.0, 1.0, 0.3, 1.0)
                )
                ));

            scene.AddLightSource(
                boost::shared_ptr<LightSource>(new LightSource(
                Vector(+5.0, +90.0, -40.0),
                Color(0.5, 0.5, 1.5, 0.5)
                )
                ));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            // Generate a PNG file that displays the scene...
            scene_loaded.SaveImage(filename, 800, 600, 5.5, 2);
            std::cout << "Wrote " << filename << std::endl;
        }


        void BitDonutTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            Torus* torus = new Torus(3.0, 1.0);

            Sphere* sphere = new Sphere(Vector(-3.0, 0.0, 0.0), 1.5);

            boost::shared_ptr<SolidObject> bitDonut = boost::shared_ptr<SolidObject>(new SetDifference(Vector(), torus, sphere));

            bitDonut->SetMatteGlossBalance(0.5, Color(0.6, 0.8, 0.18), Color(0.196078, 0.6, 0.8));
            bitDonut->Move(0.0, 0.0, -50.0);
            bitDonut->RotateX(-45.0);
            bitDonut->RotateY(-60.0);
            bitDonut->RotateZ(10.0);

            scene.AddSolidObject(bitDonut);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            // Generate a PNG file that displays the scene...
            const char *filename = "donutbite.png";
            scene_loaded.SaveImage(filename, 800, 800, 6.0, 2);
            std::cout << "Wrote " << filename << std::endl;
        }


        void SetIntersectionTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            // Display the intersection of two overlapping spheres.
            const double radius = 1.0;

            Sphere* sphere1 = new Sphere(
                Vector(-0.5, 0.0, 0.0),
                radius
                );

            sphere1->SetFullMatte(Color(1.0, 1.0, 0.5));

            Sphere* sphere2 = new Sphere(
                Vector(+0.5, 0.0, 0.0),
                radius
                );

            sphere2->SetFullMatte(Color(1.0, 0.5, 1.0));

            boost::shared_ptr<SolidObject> isect = boost::shared_ptr<SolidObject>(new SetIntersection(
                Vector(0.0, 0.0, 0.0),
                sphere1,
                sphere2
                ));

            isect->Move(0.0, 0.0, -50.0);
            isect->RotateY(-12.0);
            isect->RotateX(-28.0);

            scene.AddSolidObject(isect);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-5.0, 10.0, +80.0), Color(1.0, 1.0, 1.0))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "intersection.png";
            scene_loaded.SaveImage(filename, 800, 800, 25.0, 2);
            std::cout << "Wrote " << filename << std::endl;
        }


        void SetDifferenceTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            const Color sphereColor(1.0, 1.0, 0.5);

            // Display the intersection of two overlapping spheres.
            Sphere* sphere1 = new Sphere(Vector(-0.5, 0.0, 0.0), 1.0);
            Sphere* sphere2 = new Sphere(Vector(+0.1, 0.0, 0.0), 1.3);

            sphere1->SetFullMatte(sphereColor);
            sphere2->SetFullMatte(sphereColor);

            boost::shared_ptr<SolidObject> diff = boost::shared_ptr<SolidObject>(new SetDifference(Vector(0.0, 0.0, 0.0), sphere1, sphere2));
            diff->Move(1.0, 0.0, -50.0);
            diff->RotateY(-5.0);

            scene.AddSolidObject(diff);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-5.0, 50.0, +20.0), Color(1.0, 1.0, 1.0))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "difference.png";
            scene_loaded.SaveImage(filename, 800, 800, 20.0, 2);
            std::cout << "Wrote " << filename << std::endl;
        }


        void CuboidTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            boost::shared_ptr<SolidObject> cuboid = boost::shared_ptr<SolidObject>(new Cuboid(3.0, 4.0, 2.0));
            cuboid->SetMatteGlossBalance(0.5, Color(0.6, 0.8, 0.18), Color(0.196078, 0.6, 0.8));
            cuboid->Move(0.0, 0.0, -50.0);
            cuboid->RotateX(-50.0);
            cuboid->RotateY(-22.0);

            scene.AddSolidObject(cuboid);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-5.0, 50.0, +20.0), Color(1.0, 1.0, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-5.0, 50.0, +20.0), Color(1.0, 1.0, 1.0))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "cuboid.png";
            scene_loaded.SaveImage(filename, 800, 800, 3.0, 3);
            std::cout << "Wrote " << filename << std::endl;
        }


        void SpheroidTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            boost::shared_ptr<SolidObject> spheroid = boost::shared_ptr<SolidObject>(new Spheroid(4.0, 2.0, 1.0));
            spheroid->Move(0.0, 0.0, -50.0);
            spheroid->RotateX(-12.0);
            spheroid->RotateY(-60.0);

            scene.AddSolidObject(spheroid);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+35.0, +50.0, +20.0), Color(0.2, 1.0, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-47.0, -37.0, +12.0), Color(1.0, 0.2, 0.2))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "spheroid.png";
            scene_loaded.SaveImage(filename, 800, 800, 8.0, 2);
            std::cout << "Wrote " << filename << std::endl;
        }


        void CylinderTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            boost::shared_ptr<SolidObject> cylinder = boost::shared_ptr<SolidObject>(new Cylinder(2.0, 5.0));
            cylinder->SetFullMatte(Color(1.0, 0.5, 0.5));
            cylinder->Move(0.0, 0.0, -50.0);
            cylinder->RotateX(-72.0);
            cylinder->RotateY(-12.0);

            scene.AddSolidObject(cylinder);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+35.0, +50.0, +20.0), Color(1.0, 1.0, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-35.0, +50.0, -20.0), Color(0.2, 1.0, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-47.0, -37.0, +12.0), Color(1.0, 0.2, 0.2))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "cylinder.png";
            scene_loaded.SaveImage(filename, 800, 800, 6.0, 4);
            std::cout << "Wrote " << filename << std::endl;
        }


        void SaturnTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));
            boost::shared_ptr<SolidObject> saturn = boost::shared_ptr<SolidObject>(new Saturn());
            saturn->Move(0.0, 0.0, -100.0);
            saturn->RotateY(-15.0);
            saturn->RotateX(-83.0);
            scene.AddSolidObject(saturn);
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+30.0, +26.0, +20.0), Color(1.0, 1.0, 1.0))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "saturn.png";
            scene_loaded.SaveImage(filename, 800, 550, 4.0, 4);
            std::cout << "Wrote " << filename << std::endl;
        }


        void PolyhedraTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            Optics optics;  // use default optics (white matte finish, opaque)

            boost::shared_ptr<SolidObject>icosahedron = boost::shared_ptr<SolidObject>(new Icosahedron(Vector(-2.0, 0.0, -50.0), 1.0, optics));
            icosahedron->RotateY(-12.0);
            icosahedron->RotateX(-7.0);
            scene.AddSolidObject(icosahedron);

            boost::shared_ptr<SolidObject>dodecahedron = boost::shared_ptr<SolidObject>(new Dodecahedron(Vector(+2.0, 0.0, -50.0), 1.0, optics));
            dodecahedron->RotateX(-12.0);
            dodecahedron->RotateY(-7.0);
            scene.AddSolidObject(dodecahedron);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.1, 0.5, 0.1, 0.5))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "polyhedra.png";
            scene_loaded.SaveImage(filename, 800, 400, 12.0, 4);
            std::cout << "Wrote " << filename << std::endl;
        }


        void DodecahedronOverlapTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

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

            scene.AddSolidObject(isect);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.1, 0.5, 0.1, 0.5))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "overlap.png";
            scene_loaded.SaveImage(filename, 800, 700, 10.0, 3);
            std::cout << "Wrote " << filename << std::endl;
        }


        void AddKaleidoscopeMirrors(RayTracer::Scene& scene, double width, double depth)
        {
            using namespace RayTracer;

            // Create three cuboids, one for each mirror
            const double a = 2.0 * width;
            const double b = 0.1;           // thickness doesn't really matter
            const double c = 2.0 * depth;

            Optics optics;
            optics.SetMatteGlossBalance(1.0, Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0));

            std::vector<boost::shared_ptr<SolidObject> > mirror;
            for (int i = 0; i < 3; ++i)
            {
                mirror.push_back(boost::shared_ptr<SolidObject>(new Cuboid(a, b, c)));
                mirror.back()->SetUniformOptics(optics);

                scene.AddSolidObject(mirror.back());
            }

            // Rotate/translate the three mirrors differently.
            // Use the common value 's' that tells how far 
            // the midpoints are from the origin.
            const double s = b + a / sqrt(3.0);

            // Pre-calculate trig functions used more than once.
            const double angle = RadiansFromDegrees(30.0);
            const double ca = cos(angle);
            const double sa = sin(angle);

            // The bottom mirror just moves down (in the -y direction)
            mirror[0]->Translate(0.0, -s, 0.0);
            mirror[0]->SetTag("bottom mirror");

            // The upper left mirror moves up and left
            // and rotates +60 degrees around z axis.
            mirror[1]->Translate(-s*ca, s*sa, 0.0);
            mirror[1]->RotateZ(+60.0);
            mirror[1]->SetTag("left mirror");

            // The upper right mirror moves up and right
            // and rotates -60 degrees around z axis.
            mirror[2]->Translate(s*ca, s*sa, 0.0);
            mirror[2]->RotateZ(-60.0);
            mirror[2]->SetTag("right mirror");
        }


        void KaleidoscopeTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            AddKaleidoscopeMirrors(scene, 0.5, 10.0);

            Optics optics;  // use default optics (white matte finish, opaque)

            Dodecahedron* dodecahedron = new Dodecahedron(Vector(+0.0, 0.0, -50.0), 1.0, optics);
            dodecahedron->RotateX(-12.0);
            dodecahedron->RotateY(-7.0);

            // Create a sphere that overlaps with the dodecahedron.
            const Vector sphereCenter(Vector(+1.0, 0.0, -50.0));
            Sphere* sphere = new Sphere(sphereCenter, 1.8);

            boost::shared_ptr<SolidObject>isect = boost::shared_ptr<SolidObject>(new SetIntersection(
                sphereCenter,
                dodecahedron,
                sphere));

            isect->RotateZ(19.0);
            isect->Translate(0.0, 0.0, 10.0);

            scene.AddSolidObject(isect);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0), "LS0")));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5), "LS1")));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.1, 0.5, 0.1, 0.5), "LS2")));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(0.0, 0.0, +0.1), Color(0.5, 0.1, 0.1, 0.1), "LS3")));

            scene.AddDebugPoint(419, 300);
            scene.AddDebugPoint(420, 300);
            scene.AddDebugPoint(421, 300);

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "kaleid.png";
            scene_loaded.SaveImage(filename, 1608, 1050, 2.0, 3);
            std::cout << "Wrote " << filename << std::endl;
        }


        void MultipleSphereTest()
        {
            using namespace RayTracer;

            // Put spheres with different optical properties next to each other.
            Scene scene(Color(0.0, 0.0, 0.0));

            boost::shared_ptr<SolidObject>shinySphere = boost::shared_ptr<SolidObject>(new Sphere(Vector(-1.5, 0.0, -50.0), 1.0));

            shinySphere->SetMatteGlossBalance(
                0.3,    // 30% of reflection is shiny, 70% is dull
                Color(1.0, 1.0, 1.0),   // matte color is white
                Color(1.0, 1.0, 1.0)    // gloss color is white
                );
            shinySphere->SetOpacity(0.6);
            scene.AddSolidObject(shinySphere);

            boost::shared_ptr<SolidObject> matteSphere = boost::shared_ptr<SolidObject>(new Sphere(Vector(+1.5, 0.0, -50.0), 1.0));
            matteSphere->SetFullMatte(Color(0.6, 0.6, 0.9));
            matteSphere->SetOpacity(1.0);
            scene.AddSolidObject(matteSphere);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.1, 0.2, 0.1, 0.5))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char* filename = "multisphere.png";
            scene_loaded.SaveImage(filename, 800, 700, 10.0, 1);
            std::cout << "Wrote " << filename << std::endl;
        }

        void ChessBoardTest()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

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

            scene.AddSolidObject(board);

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
                scene.AddSolidObject(sphere);
            }

            // Add light sources.

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +25.0, +50.0), Color(0.4, 0.4, 0.1, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, +40.0), Color(0.5, 0.5, 1.5, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-25.0, +30.0, +40.0), Color(0.3, 0.2, 0.1, 1.0))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char* filename = "chessboard.png";
            scene_loaded.SaveImage(filename, 800, 600, 4.5, 3);
            std::cout << "Wrote " << filename << std::endl;
        }

        void CustomScene()
        {
            using namespace RayTracer;

            Scene scene(Color(0.0, 0.0, 0.0));

            Optics optics;  // use default optics (white matte finish, opaque)

            boost::shared_ptr<SolidObject>icosahedron = boost::shared_ptr<SolidObject>(new Icosahedron(Vector(-2.0, 0.0, -50.0), 1.0, optics));
            icosahedron->RotateY(-12.0);
            icosahedron->RotateX(-7.0);
            scene.AddSolidObject(icosahedron);

            boost::shared_ptr<SolidObject>dodecahedron = boost::shared_ptr<SolidObject>(new Dodecahedron(Vector(+2.0, 0.0, -50.0), 1.0, optics));
            dodecahedron->RotateX(-12.0);
            dodecahedron->RotateY(-7.0);
            scene.AddSolidObject(dodecahedron);

            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(-45.0, +10.0, +50.0), Color(1.0, 1.0, 0.3, 1.0))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+5.0, +90.0, -40.0), Color(0.5, 0.5, 1.5, 0.5))));
            scene.AddLightSource(boost::shared_ptr<LightSource>(new LightSource(Vector(+45.0, -10.0, +40.0), Color(0.1, 0.5, 0.1, 0.5))));

            // save data
            {
                std::ofstream outStream("scene.bin");
                boost::archive::text_oarchive outArchive(outStream);
                outArchive << scene;
            }

            Scene scene_loaded(Color(0.0, 0.0, 0.0));

            // load data    
            {
                std::ifstream inStream("scene.bin");
                boost::archive::text_iarchive inArchive(inStream);
                inArchive >> scene_loaded;
            }

            const char *filename = "scene_1.png";
            scene.SaveImage(filename, 800, 400, 12.0, 4);
            std::cout << "Wrote " << filename << std::endl;
        }

        void UnitTests()
        {


            // SphereTest();
            // BlockTest();
            // BitDonutTest();
            // CuboidTest();
            // DodecahedronOverlapTest();
            // CylinderTest();
            ChessBoardTest();

            // Algebra::UnitTest();

            // ChessBoardTest();

            // MultipleSphereTest();

            // PolyhedraTest();

            // SaturnTest();
            // CylinderTest();
            // SpheroidTest();

            // SetDifferenceTest();
            // SetIntersectionTest();

            // KaleidoscopeTest();
            // TorusTest("torus1.png", 0.0);
            // TorusTest("torus2.png", 0.7);
            // CustomScene();
        }
    }
}