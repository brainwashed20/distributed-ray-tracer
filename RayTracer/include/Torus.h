#ifndef _TORUS_H_
#define _TORUS_H_

#include "SolidObject_Reorientable.h"
#include "SolidObject.h"

namespace RayTracer
{
    class Torus : public SolidObject_Reorientable
    {
    public:

        Torus(double _R = 0.0, double _S = 0.0)
            : SolidObject_Reorientable()
            , R(_R)
            , S(_S)
        {
            SetTag("Torus");
        }

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_Reorientable>(*this);
            ar & const_cast<double&>(R);
            ar & const_cast<double&>(S);
        }

    protected:

        virtual void ObjectSpace_AppendAllIntersections(
            const Vector& vantage,
            const Vector& direction,
            std::vector<Intersection>& intersectionList) const;

        virtual bool ObjectSpace_Contains(const Vector& point) const;

        int SolveIntersections(
            const Vector& vantage,
            const Vector& direction,
            double uArray[4]) const;

        Vector SurfaceNormal(const Vector& point) const;

    private:

        // distance from center of hole to center of tube
        const double R;

        // distance from center of tube to outside of tube
        const double S;
    };
}

#endif