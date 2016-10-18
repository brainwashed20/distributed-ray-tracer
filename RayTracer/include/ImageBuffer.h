#ifndef _IMAGE_BUFFER_H_
#define _IMAGE_BUFFER_H_

#include "Color.h"
#include "PixelCoordinates.h"

namespace RayTracer
{
	// The information available for any pixel in an ImageBuffer
	class PixelData
	{
    public:

		Color   color;
		bool    isAmbiguous;

		PixelData()
			: color()
			, isAmbiguous(false)
		{
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & color;
            ar & isAmbiguous;
        }
	};

	/**
     * Holds an image in memory as it is being rendered.
	 * Once calculated, the image in the buffer can be translated 
	 * into a graphics format like PNG.
	 */
    class ImageBuffer
	{
	public:
		ImageBuffer(
			size_t _pixelsWide = 0,
			size_t _pixelsHigh = 0,
			const Color &backgroundColor = Color(0.0, 0.0, 0.0))
			: pixelsWide(_pixelsWide)
			, pixelsHigh(_pixelsHigh)
			, numPixels(_pixelsWide * _pixelsHigh)
		{
			array = new PixelData[numPixels];
		}

        void TransformFromMatrix(std::vector<std::vector<std::pair<RayTracer::PixelData, RayTracer::PixelCoordinates>>>& data)
        {
            array = new PixelData[numPixels];
            
            int totalChunksSize = 0;
            for (int i = 0; i < data.size(); ++i)
            {
                totalChunksSize += data[i].size();
            }

            assert(numPixels == totalChunksSize);
            
            int arrayIndex = -1;

            std::vector<std::pair<RayTracer::PixelData, RayTracer::PixelCoordinates>> input;

            while (!data.empty())
            {
                std::copy(data.back().begin(), data.back().end(), std::back_inserter(input));
                data.pop_back();
            }

            std::sort(input.begin(), input.end(), []
                (const std::pair<RayTracer::PixelData, RayTracer::PixelCoordinates> & a, 
                const std::pair<RayTracer::PixelData, RayTracer::PixelCoordinates> & b) -> bool
                {
                    if (a.second.i == b.second.i)
                    {
                        return a.second.j < b.second.j;
                    }
                    return a.second.i < b.second.i;
                }
            );

            assert(numPixels == input.size());

            for (int i = 0; i < input.size(); ++i)
            {
                array[i] = input[i].first;
            }
        }

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & pixelsWide;
            ar & pixelsHigh;
            ar & numPixels;
            ar & array;
        }

		virtual ~ImageBuffer()
		{
			delete[] array;
			array = nullptr;
			pixelsWide = pixelsHigh = numPixels = 0;
		}

        /**
		 * Returns a read/write reference to the pixel data for the specified column (i) and row (j).
		 * Throws an exception if the coordinates are out of bounds.
		 */
        PixelData& Pixel(size_t i, size_t j) const
		{
			if((i < pixelsWide) && (j < pixelsHigh))
			{
				return array[(j * pixelsWide) + i];
			}
			else
			{
				throw ImagerException("Pixel coordinate(s) out of bounds");
			}
		}

		size_t GetPixelsWide() const
		{
			return pixelsWide;
		}

		size_t GetPixelsHigh() const
		{
			return pixelsHigh;
		}

        /**
		 * Finds the maximum red, green, or blue value in the image.
		 * Used for automatically scaling the image brightness.
		 */
        double MaxColorValue() const
		{
			double max = 0.0;
			
            for(size_t i = 0; i < numPixels; ++i)
			{
				array[i].color.Validate();
				if(array[i].color.red > max)
				{
					max = array[i].color.red;
				}
				if(array[i].color.green > max)
				{
					max = array[i].color.green;
				}
				if(array[i].color.blue > max)
				{
					max = array[i].color.blue;
				}
			}
			
            if(max == 0.0)
			{
				/**
                 * Safety feature: the image is solid black anyway, so there is no point trying to scale it.
				 * If we did, we would end up dividing by zero.
				 */
                max = 1.0;
			}

			return max;
		}

	private:
		size_t  pixelsWide;     // the width of the image in pixels (columns).
		size_t  pixelsHigh;     // the height of the image in pixels (rows).
		size_t  numPixels;      // the total number of pixels.
		PixelData*  array;      // flattened array [pixelsWide * pixelsHigh].
	};
}

#endif