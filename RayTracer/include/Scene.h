#ifndef _SCENE_H_
#define _SCENE_H_

#include "Color.h"
#include "LightSource.h"
#include "ImageBuffer.h"
#include "SolidObject.h"
#include "PixelCoordinates.h"

namespace RayTracer
{
	/**
     * The Scene object renders a collection of SolidObjects and ightSources that illuminate them.
     * SolidObjects are added one by one using the method AddSolidObject.
     * Likewise, LightSources are added using AddLightSource.
     */
	class Scene
	{
    private:

        class DebugPoint
        {
        public:

            int     iPixel;
            int     jPixel;

            DebugPoint(int _iPixel = 0, int _jPixel = 0)
                : iPixel(_iPixel)
                , jPixel(_jPixel)
            {
            }
            friend class access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & iPixel;
                ar & iPixel;
            }
        };

        // The color to use for pixels where no solid object intersection was found.
        Color backgroundColor;

        // A list of all the solid objects in the scene.
        std::vector<boost::shared_ptr<SolidObject>> solidObjectList;

        // A list of all the point light sources in the scene.
        std::vector<boost::shared_ptr<LightSource>> lightSourceList;

        /**
         * The refractive index of every point in space
         * that is not explicitly occupied by some object.
         * By default, this is REFRACTION_VACUUM, but may be
         * set to a higher value to simulate the entire
         * scene being immersed in some transparent substance
         * like water.
         */
        double ambientRefraction;

        // Help performance by avoiding constant construction/destruction of intersection lists.
        mutable std::vector<Intersection> cachedIntersectionList;

        std::vector<DebugPoint> debugPointList;
        boost::shared_ptr<DebugPoint> activeDebugPoint;

	public:

		explicit Scene(const Color& _backgroundColor = Color())
			: backgroundColor(_backgroundColor)
			, ambientRefraction(REFRACTION_VACUUM)	
		{
            activeDebugPoint = boost::shared_ptr<DebugPoint>(new DebugPoint());
		}

        ImageBuffer CreateImageBuffer(size_t pixelsWide, size_t pixelsHigh, double zoom, size_t antiAliasFactor);

        PixelData GetPixelAt(size_t iPos, size_t jPos, size_t pixelsWide, size_t pixelsHigh, double zoom, size_t antiAliasFactor);

        void HandleAmbigousPixels(ImageBuffer& buffer, size_t pixelsWide, size_t pixelsHigh, double zoom, size_t antiAliasFactor);

        void CreateImage(const std::string& imageName, ImageBuffer& buffer, size_t pixelsWide, size_t pixelsHigh, double zoom, size_t antiAliasFactor);

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            // Color
            ar & backgroundColor;
            // Lists
            ar & solidObjectList;
            ar & lightSourceList;
            // double
            ar & ambientRefraction;
            // std::vector<Intersection>
            ar & cachedIntersectionList;
            // DebugPointList
            ar & debugPointList;
            // DebugPoint*
            ar & activeDebugPoint;

        }

		virtual ~Scene()
		{
			ClearSolidObjectList();
		}

        /**
		 * Caller must allocate solidObject via operator new.
		 * This class will then own the responsibility of deleting it.
		 */
        SolidObject& AddSolidObject(boost::shared_ptr<SolidObject> solidObject)
		{
			solidObjectList.push_back(solidObject);
            return *solidObject;
		}

		void AddLightSource(boost::shared_ptr<LightSource> lightSource)
		{
			lightSourceList.push_back(lightSource);
		}

        /**
		 * Renders an image of the current scene, with the camera  at <0, 0, 0> and looking into the +z axis, with the +y axis upward.
		 * Writes the image to the specified PNG file, which should have a ".png" extension.
		 * The resulting image will have pixel dimensions pixelsWide wide by pixelsHigh high.
		 * The zoom factor specifies magnification level: use 1.0 to start with, and try larger/smaller values to increase/decrease magnification.
		 * antiAliasFactor specifies what multiplier to use for oversampling.  Note that this causes run time and memory usage to increase O(N^2), so it is best 
         * to use a value between 1 (fastest but most "jaggy") to 4 (16 times slower but results in much smoother images).
		 */
        void SaveImage(
			const char *outPngFileName,
			size_t pixelsWide,
			size_t pixelsHigh,
			double zoom,
			size_t antiAliasFactor) const;

		/**
         * By default, regions of space that are not explicitly occupied by some object have the refractive index of vacuum, or REFRACTION_VACUUM = 1.  
         * The following function allows the caller to override this default.  
         * The value of the 'refraction' parameter must be in the range REFRACTION_MINIMUM to REFRACTION_MAXIMUM.
		 */
        void SetAmbientRefraction(double refraction)
		{
			ValidateRefraction(refraction);
			ambientRefraction = refraction;
		}

		void AddDebugPoint(int iPixel, int jPixel)
		{
			debugPointList.push_back(DebugPoint(iPixel, jPixel));
		}

	private:
		void ClearSolidObjectList();

		int FindClosestIntersection(
			const Vector& vantage,
			const Vector& direction,
			Intersection& intersection) const;

		bool HasClearLineOfSight(
			const Vector& point1,
			const Vector& point2) const;

		Color TraceRay(
			const Vector& vantage,
			const Vector& direction,
			double refractiveIndex,
			Color rayIntensity,
			int recursionDepth) const;

		Color CalculateLighting(
			const Intersection& intersection,
			const Vector& direction,
			double refractiveIndex,
			Color rayIntensity,
			int recursionDepth) const;

		Color CalculateMatte(const Intersection& intersection) const;

		Color CalculateReflection(
			const Intersection& intersection,
			const Vector& incidentDir,
			double refractiveIndex,
			Color rayIntensity,
			int recursionDepth) const;

		Color CalculateRefraction(
			const Intersection& intersection,
			const Vector& direction,
			double sourceRefractiveIndex,
			Color rayIntensity,
			int recursionDepth,
			double& outReflectionFactor) const;

        boost::shared_ptr<SolidObject> PrimaryContainer(const Vector& point) const;

		double PolarizedReflection(
			double n1,              // source material's index of refraction
			double n2,              // target material's index of refraction
			double cos_a1,          // incident or outgoing ray angle cosine
			double cos_a2) const;   // outgoing or incident ray angle cosine

		void ResolveAmbiguousPixel(ImageBuffer& buffer, size_t i, size_t j) const;

        /**
		 * Convert a floating point color component value, 
		 * based on the maximum component value,
		 * to a byte RGB value in the range 0x00 to 0xff.
		 */
        static unsigned char ConvertPixelValue(
			double colorComponent,
			double maxColorValue)
		{
			int pixelValue =
				static_cast<int> (255.0 * colorComponent / maxColorValue);

			// Clamp to the allowed range of values 0..255.
			if(pixelValue < 0)
			{
				pixelValue = 0;
			}
			else if(pixelValue > 255)
			{
				pixelValue = 255;
			}

			return static_cast<unsigned char>(pixelValue);
		}
	};
}

#endif