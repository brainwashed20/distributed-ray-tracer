#ifndef _SOLIDOBJECT_REORIENTABLE_H_
#define _SOLIDOBJECT_REORIENTABLE_H_

#include "SolidObject.h"

namespace RayTracer
{   
    /**
	 * This derived abstract class is specialized for objects (like torus)
	 * that are easy to define in terms of a fixed orientation and position
	 * in space, but for which generalized rotation makes the algebra 
	 * annoyingly difficult. Instead, we allow defining the object in terms
	 * of a new coordinate system <r,s,t> and translate locations and rays
	 * from <x,y,z> camera coordinates into <r,s,t> object coordinates.
	 */
    class SolidObject_Reorientable: public SolidObject
	{
	public:
		explicit SolidObject_Reorientable(const Vector& _center = Vector())
			: SolidObject(_center)
			, rDir(1.0, 0.0, 0.0)
			, sDir(0.0, 1.0, 0.0)
			, tDir(0.0, 0.0, 1.0)
			, xDir(1.0, 0.0, 0.0)
			, yDir(0.0, 1.0, 0.0)
			, zDir(0.0, 0.0, 1.0)
		{
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject>(*this);
            ar & rDir;
            ar & sDir;
            ar & tDir;
            ar & xDir;
            ar & yDir;
            ar & zDir;
        }

        /**
		 * Fills in 'intersectionList' with a list of all the intersections found starting at the specified vantage point in the specified direction.
		 * Any pre-existing content in 'intersectionList' is discarded first.
		 * Returns the number of intersections found, which will have the same value as intersectionList.size().
		 */
        virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual SolidObject& RotateX(double angleInDegrees);
		virtual SolidObject& RotateY(double angleInDegrees);
		virtual SolidObject& RotateZ(double angleInDegrees);

		virtual bool Contains(const Vector& point) const
		{
			return ObjectSpace_Contains(ObjectPointFromCameraPoint(point));
		}

		virtual Optics SurfaceOptics(
			const Vector& surfacePoint,
			const void *context) const
		{
			return ObjectSpace_SurfaceOptics(
				ObjectPointFromCameraPoint(surfacePoint),
				context);
		}

	protected:

		/*
         * The following method is called by AppendAllIntersections, but with 'vantage' and 'direction' vectors transformed from <x,y,z> camera space into <r,s,t> object space.
		 * Intersection objects are returned in terms of object coordinates, and they are automatically translated back into camera coordinates by the caller.
		 */
        virtual void ObjectSpace_AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const = 0;
        
        /**
		 * Returns true if the specified point in object space is on or inside the solid object.
		 * Actually, well-behaved derived classes should provide a tolerance for points slightly outside the object's boundaries and return true then also.
		 * This tolerance handles small floating point rounding  errors that may cause a point that is supposed to be  considered part of the solid to be incorrectly excluded.
		 */
        virtual bool ObjectSpace_Contains(const Vector& point) const = 0;

		virtual Optics ObjectSpace_SurfaceOptics(
			const Vector& surfacePoint,
			const void *context) const
		{
			return GetUniformOptics();
		}

		Vector ObjectDirFromCameraDir(const Vector& cameraDir) const
		{
			return Vector(
				DotProduct(cameraDir, rDir),
				DotProduct(cameraDir, sDir),
				DotProduct(cameraDir, tDir));
		}

		Vector ObjectPointFromCameraPoint(const Vector &cameraPoint) const
		{
			return ObjectDirFromCameraDir(cameraPoint - Center());
		}

		Vector CameraDirFromObjectDir(const Vector& objectDir) const
		{
			return Vector(
				DotProduct(objectDir, xDir),
				DotProduct(objectDir, yDir),
				DotProduct(objectDir, zDir));
		}

		Vector CameraPointFromObjectPoint(const Vector& objectPoint) const
		{
			return Center() + CameraDirFromObjectDir(objectPoint);
		}

		void UpdateInverseRotation()
		{
            /**
			 * See the following Wikipedia articles to explain why
			 * the inverse of a rotation matrix is just its transpose.
			 * http://en.wikipedia.org/wiki/Rotation_matrix
			 * http://en.wikipedia.org/wiki/Orthogonal_matrix
             */
			xDir = Vector(rDir.x, sDir.x, tDir.x);
			yDir = Vector(rDir.y, sDir.y, tDir.y);
			zDir = Vector(rDir.z, sDir.z, tDir.z);
		}

	private:
   
        /**
		 * The members rDir, sDir, tDir are unit vectors in the direction of the <r,s,t> object axes, each expressed in <x,y,z> camera space.
		 * For any point P = <Px,Py,Pz> in camera coordinates, we can determine object-relative coordinates as dot products <(P-C).rDir,(P-C).sDir,(P-C).tDir>,
		 * where C = the center of the object as returned by method Center().
		 * Another way to look at this is that (rDir, sDir, tDir) taken together are really just a 3*3 rotation matrix.
		 */
        Vector  rDir;
		Vector  sDir;
		Vector  tDir;
        
        /**
		 * The members xDir, yDir, zDir are unit vectors in the direction of the <x,y,z> camera axes, each expressed in <r,s,t> object space.
		 * These are maintained in tandem with rDir, sDir, tDir as various rotations take place.  
         * Taken together, they form an inverse  rotation matrix, so (xDir,yDir,zDir) as a 3*3 matrix is calculated as the transpose of the 3*3 matrix (rDir,sDir,tDir).
		 * Because an object is never rotated during the rendering of a given frame, it is more efficient to have both matrices pre-calculated.
		 */
        Vector  xDir;
		Vector  yDir;
		Vector  zDir;
	};
}

#endif