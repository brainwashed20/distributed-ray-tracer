#pragma once

//#define WIN32_LEAN_AND_MEAN

// Boost fixes, first (so-called)
#define BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING // Make chrono more header-only;
#define BOOST_CHRONO_HEADER_ONLY // Make it clear we want no Boost.system dependencies;

#define BOOST_SYSTEM_HEADER_ONLY // We want it fully header-only;
#define BOOST_SYSTEM_NO_DEPRECATED // We want no deprecated error handling;
#define BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING // Make chrono header-only on C++03;
#define BOOST_CHRONO_USES_MPL_ASSERT // Use MPL assert instead;
#define BOOST_CHRONO_THREAD_DISABLED // No dependency on threads;


#define BOOST_ERROR_CODE_HEADER_ONLY // Make error code header only and include [libs] in [Externals];


#define BOOST_NO_RTTI // We don't have RTTI support for now;
#define BOOST_NO_TYPEID // And no type support;

// Disable auto-linking
#define BOOST_LIB_DIAGNOSTIC
#define BOOST_ATOMIC_NO_LIB
#define BOOST_LOCALE_NO_LIB
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_REGEX_NO_LIB
#define BOOST_CHRONO_NO_LIB
#define BOOST_IOSTREAMS_NO_LIB
#define BOOST_TIMER_NO_LIB
#define BOOST_PROGRAM_OPTIONS_NO_LIB
#define BOOST_PYTHON_NO_LIB
#define BOOST_SIGNALS_NO_LIB
#define BOOST_SERIALIZATION_NO_LIB
#define BOOST_WAVE_NO_LIB


#include <memory>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <utility>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "System.h"
#include "Utils.h"

namespace RayServer
{
    namespace asio = boost::asio;
}