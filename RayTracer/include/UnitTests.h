#ifndef _LOCAL_UNIT_TESTS_H_
#define _LOCAL_UNIT_TESTS_H_

#include "RayTracer.h"
#include "Algebra.h"

#include <iostream>
#include <fstream>

#include "Serialization.h"

namespace RayTracer
{
    namespace LocalTests
    {
        void BlockTest();
        void SphereTest();
        void TorusTest(const char* filename, double glossFactor);
        void BitDonutTest();
        void SetIntersectionTest();
        void SetDifferenceTest();
        void CuboidTest();
        void SpheroidTest();
        void CylinderTest();
        void SaturnTest();
        void PolyhedraTest();
        void DodecahedronOverlapTest();
        void AddKaleidoscopeMirrors(RayTracer::Scene& scene, double width, double depth);
        void KaleidoscopeTest();
        void MultipleSphereTest();
        void ChessBoardTest();
        void UnitTests();
    }
}
#endif