#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include <boost/archive/text_iarchive.hpp>                                                                                                                                                                     
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace boost
{
    namespace serialization
    {
        typedef boost::serialization::access access;
    };
};

#endif