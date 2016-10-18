#ifndef _IMAGER_EXCEPTION_H_
#define _IMAGER_EXCEPTION_H_

// An exception thrown by imager code when a fatal error occurs.
class ImagerException
{
public:
	explicit ImagerException(const char *_message = nullptr)
		: message(_message)
	{
	}

	const char *GetMessage() const
	{
		return message;
	}

private:

	const char * const message;
};

/**
 * An exception thrown when multiple intersections lie at the
 * same distance from the vantage point.  SaveImage catches
 * these and marks such pixels as ambiguous.  It performs a second
 * pass later that averages the color values of surrounding 
 * non-ambiguous pixels.
 */
class AmbiguousIntersectionException
{
};

#endif