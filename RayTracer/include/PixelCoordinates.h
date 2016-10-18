#pragma once

namespace RayTracer
{
    // Define types needed to hold a list of pixel coordinates.
    class PixelCoordinates
    {
    public:

        size_t i;
        size_t j;

        PixelCoordinates(size_t _i = 0, size_t _j = 0)
            : i(_i)
            , j(_j)
        {
        }

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & i;
            ar & j;
        }
    };
}