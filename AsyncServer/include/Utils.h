#pragma once

#include <mutex>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace RayServer
{
    class Utils
    {
    public:
        Utils();

        // Utilities
        static std::string GetSHA1(const std::vector<char> & objData);
        static std::string GetSHA1(const std::string & objData);
        static std::string GetUUID();
        static int GetRandomInt();
        static unsigned int GetUTCAsSeconds();
        static unsigned int GetTZTAsSeconds();
        static std::string GetUTCAsString();
        static bool TryParseStringToUInt(const std::string& stringIn, unsigned int& uIntOut);
        static std::string GetUntilSeparator(std::string& input);

        template<class T>
        static inline std::string ToString(T objVar)
        {
            std::lock_guard<std::mutex> guardLock(s_ToStringMutex);
            return boost::lexical_cast<std::string> (objVar);
        }

    private:
        static boost::random::mt19937 s_WithGenerator;
        static boost::random::uniform_int_distribution<> s_OnDistribution;
        static boost::uuids::basic_random_generator<boost::random::mt19937> s_OneUuidGenerator;
        static const boost::posix_time::ptime s_ObjEpoch;
        static std::mutex s_GetUUIDMutex;
        static std::mutex s_ToStringMutex;
        static std::string s_TimeFormat;
    };
}