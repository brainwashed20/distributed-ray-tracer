#ifndef _TRIANGLE_MESH_H_
#define _TRIANGLE_MESH_H_

#include "stdafx.h"
#include "SolidObject.h"

namespace RayTracer
{
    /**
     * A solid object consisting of nothing but triangular faces.
     * Faces are added after construction by calling AddPoint()
     * to append a series of vertex points,
     * followed by AddTriangle() to refer to the indices
     * of previously added points.
     */
	class TriangleMesh: public SolidObject
	{
	public:

		TriangleMesh(
			const Vector& center = Vector(),
			bool _isFullyEnclosed = true)
			: SolidObject(center, _isFullyEnclosed)
		{
			SetTag("TriangleMesh");
		}

		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual SolidObject& Translate(double dx, double dy, double dz);
		virtual SolidObject& RotateX(double angleInDegrees);
		virtual SolidObject& RotateY(double angleInDegrees);
		virtual SolidObject& RotateZ(double angleInDegrees);
        
        /**
		 * Appends a new vertex point whose point index is to be 
		 * referenced later by AddTriangle.
		 * expectedIndex is the zero-based value that must match 
		 * the insertion order of the point.
		 * For example, the first call to AddPoint must pass 
		 * expectedIndex==0, the second must pass expectedIndex==1, etc.
		 * This is a sanity check so that the caller avoids insertion 
		 * order mistakes, since the vertex point index will be 
		 * referenced later when calling AddTriangle.
		 */
        void AddPoint(int expectedIndex, double x, double y, double z)
		{
			if(expectedIndex != pointList.size())
			{
				// Sanity check failed that caller is passing 
				// in correct indexes for points.
				throw ImagerException("Point index mismatch.");
			}

			pointList.push_back(Vector(x, y, z));
		}

        /*
		 * Given the vertex point indices of three distinct points 
		 * that have already been added (using a call to AddPoint), 
		 * appends a new triangular face with those three points 
		 * as vertices.  Uses the specified optical properties for this face.
         */
		void AddTriangle(
			int aPointIndex,
			int bPointIndex,
			int cPointIndex,
			const Optics& optics);
        /*
		 * A convenience method for cases where we know we have 
		 * a quadrilateral surface that can be split into two triangles.
		 * The point indices (a,b,c,d) are passed in counterclockwise 
		 * order viewed from outside the surface.
		 * This is important for calculating normal vectors that 
		 * point outward from the solid, not inward.
         */
		void AddQuad(
			int aPointIndex,
			int bPointIndex,
			int cPointIndex,
			int dPointIndex,
			const Optics& optics)
		{
			// We preserve counterclockwise ordering by making two triangles: (a,b,c) and (c,d,a).
			AddTriangle(aPointIndex, bPointIndex, cPointIndex, optics);
			AddTriangle(cPointIndex, dPointIndex, aPointIndex, optics);
		}

        /**
		 * Another convenience method for solids that 
		 * have pentagonal faces. A pentagon can be split into 3 triangles.
		 * As in AddTriangle and AddQuad, the caller must pass the point 
		 * indices in a counterclockwise order as seen from outside the solid.
		 */
        void AddPentagon(
			int aPointIndex,
			int bPointIndex,
			int cPointIndex,
			int dPointIndex,
			int ePointIndex,
			const Optics &optics)
		{
			AddTriangle(aPointIndex, bPointIndex, cPointIndex, optics);
			AddTriangle(cPointIndex, dPointIndex, aPointIndex, optics);
			AddTriangle(dPointIndex, ePointIndex, aPointIndex, optics);
		}

		// Because each triangle can have different optics, we need to override SurfaceOptics() to replace the default behavior having uniform optics for the whole solid.
		virtual Optics SurfaceOptics(
			const Vector& surfacePoint,
			const void *context) const;

        virtual ~TriangleMesh()
        {
            pointList.clear();
            triangleList.clear();
        }

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject>(*this);
            ar & pointList;
            ar & triangleList;
        }

	protected:
		void ValidatePointIndex(size_t pointIndex) const
		{
			if((pointIndex < 0) || (pointIndex >= pointList.size()))
			{
				throw ImagerException("Point index is out of bounds");
			}
		}

        /**
		 * Attempts to find an intersection of the given direction 
		 * passing through the given vantage point with the plane 
		 * that passes through the triangle (A, B, C).
		 * If an intersection can be found, returns true and sets 
		 * the output parameters:
		 *
		 *     u = The scalar multiple such that the intersection 
		 *         point = (u*direction + vantage).
		 *
		 *     v = The component of the vector difference B-A, 
		 *         starting at A, of the intersection point.
		 *
		 *     w = The component of the vector difference C-A, 
		 *         starting at A, of the intersection point.
		 *
		 * Returns false and leaves u, v, w undefined if an 
		 * intersection cannot be found.
		 * Note that it is possible that calling AttemptPlaneIntersection 
		 * may succeed with one ordering of the plane points (A, B, C), 
		 * but may fail on another ordering.
         */
		static bool AttemptPlaneIntersection(
			const Vector& vantage,
			const Vector& direction,
			const Vector& A,
			const Vector& B,
			const Vector& C,
			double &u,
			double &v,
			double &w)
		{
			return Algebra::SolveLinearEquations(
				direction.x, A.x - B.x, A.x - C.x, -(A.x - vantage.x),
				direction.y, A.y - B.y, A.y - C.y, -(A.y - vantage.y),
				direction.z, A.z - B.z, A.z - C.z, -(A.z - vantage.z),
				u,
				v,
				w);
		}

		Vector GetPointFromIndex(int pointIndex) const
		{
			return pointList[pointIndex];
		}

	private:

        struct Triangle
        {
            int a;  // index into pointList for first  vertex of the triangle
            int b;  // index into pointList for second vertex of the triangle
            int c;  // index into pointList for third  vertex of the triangle

            Optics optics;      // surface color of the triangle

            Triangle(
                int _a = 0, int _b = 0, int _c = 0, 
                const Optics& _optics = Optics(Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.0), 1.0))
                : a(_a)
                , b(_b)
                , c(_c)
                , optics(_optics)
            {
            }

            friend class access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & a;
                ar & b;
                ar & c;
                ar & optics;
            }
        };

        // Returns a unit vector at right angles to the triangle, using right-hand rule with respect to A,B,C ordering.
        Vector NormalVector(const Triangle& triangle) const;

        // A list of all the vertex points used to define triangles. A given point may be referenced by one or more triangles.
        std::vector<Vector>       pointList;

        // A list of all the triangles, each of which refers to 3 distinct points in pointList.
        std::vector<Triangle>    triangleList;
	};
}
#endif