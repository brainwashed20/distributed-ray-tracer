#include "Utils.h"
#include "System.h"

#include <boost/date_time.hpp>

#include <fstream>

namespace RayClient
{
    boost::random::mt19937 Utils::s_WithGenerator;
    boost::random::uniform_int_distribution<> Utils::s_OnDistribution(1, 32);
    boost::uuids::basic_random_generator<boost::random::mt19937> Utils::s_OneUuidGenerator;
    const boost::posix_time::ptime Utils::s_ObjEpoch(boost::gregorian::date(1970, 1, 1));
    std::mutex Utils::s_GetUUIDMutex;
    std::mutex Utils::s_ToStringMutex;
    std::string Utils::s_TimeFormat("%Y-%m-%d %H:%M:%S");

    /**
     * Method relies on boost::random to return an integer from a
     * random distribution. The (1, 32) can be adjusted. It resembles
     * a dice with 32 faces. You can set it to 6 to resemble a dice with
     * 6 faces but the randomness entropy will be lower. 32 is a good
     * setting. 64K ain't.
     */
    int Utils::GetRandomInt()
    {
        // Return
        return s_OnDistribution(s_WithGenerator);
    }

    /**
     * Method returns a human readable UTC.
     * Eg: 2014-10-30 18:22:31
     */
    std::string Utils::GetUTCAsString()
    {
        std::string utcString;
        char buffer[128];
        std::time_t utcAsSeconds(static_cast<std::time_t>(GetUTCAsSeconds()));

        // Check
        if (std::strftime(buffer, 128, s_TimeFormat.c_str(), std::gmtime(&utcAsSeconds)))
        {
            // Set
            utcString = std::string(buffer);
        }

        // Return
        return utcString;
    }

    /**
    * Method relies on boost::posix_time to return the device UTC time
    * without taking into account the timezone the device is in. These
    * two times differ on any device that has a non-GMT+0 timezone. The
    * time is measured Unix-wise with the seconds since 01.01.1970.
    */
    unsigned int Utils::GetUTCAsSeconds()
    {
        const boost::posix_time::ptime objUTC(boost::posix_time::second_clock::universal_time());
        const boost::posix_time::time_duration::sec_type objSeconds((objUTC - s_ObjEpoch).total_seconds());
        return static_cast<const unsigned int> (objSeconds);
    }

    /**
    * Method returns the time adjusted for the timezone the device is in,
    * which may differ from the UTC time. Can be used as an alternative
    * time from the device;
    */
    unsigned int Utils::GetTZTAsSeconds()
    {
        const boost::posix_time::ptime objTZT(boost::posix_time::second_clock::local_time());
        const boost::posix_time::time_duration::sec_type objSeconds((objTZT - s_ObjEpoch).total_seconds());
        return static_cast<const unsigned int> (objSeconds);
    }

    /**
    * Method generates an UUID. The Universally Unique Identifier is a
    * random UUID generated by devices, taking in account things such as
    * the MAC address, random entropy, device identifiers and more. This
    * uses version 4 for UUID (the random one and best version).
    */
    std::string Utils::GetUUID()
    {
        std::lock_guard<std::mutex> guardLock(s_GetUUIDMutex);
        // Return
        // return boost::lexical_cast<std::string> (s_OneUuidGenerator());
        return std::string();
    }

    /**
    * Method computs the SHA1 of any given string of data,
    * and returns that hash as a string also to be outputted
    * with stringstreams. (sockets, files). The hash can be
    * also taken as char[] if you need to write it down
    * binary somewhere by reimplementing this method and
    * bypassing the transformation to string form;
    */
    std::string Utils::GetSHA1(const std::vector<char> & objData)
    {
        // Set
        char objHash[20];
        unsigned int objDigest[5];
        boost::uuids::detail::sha1 objSHA1;
        objSHA1.process_bytes(&objData[0], objData.size());
        objSHA1.get_digest(objDigest);

        // For
        for (int objI = 0; objI < 5; ++objI)
        {
            // Set a temporary
            const char * objTemporary(reinterpret_cast<char *> (objDigest));

            // Swappings
            objHash[objI * 4 + 0] = objTemporary[objI * 4 + 3];
            objHash[objI * 4 + 1] = objTemporary[objI * 4 + 2];
            objHash[objI * 4 + 2] = objTemporary[objI * 4 + 1];
            objHash[objI * 4 + 3] = objTemporary[objI * 4 + 0];
        }

        // Set
        std::stringstream onStream;
        onStream << std::hex;

        // For
        for (int objI = 0; objI < 20; ++objI)
        {
            onStream << ((objHash[objI] & 0x000000F0) >> 4)
                << (objHash[objI] & 0x0000000F);
        }

        // Return
        return onStream.str();
    }

    /**
    * Method computs the SHA1 of any given string of data,
    * and returns that hash as a string also to be outputted
    * with stringstreams. (sockets, files). The hash can be
    * also taken as char[] if you need to write it down
    * binary somewhere by reimplementing this method and
    * bypassing the transformation to string form;
    */
    std::string Utils::GetSHA1(const std::string & objData)
    {
        // Set
        char objHash[20];
        unsigned int objDigest[5];
        boost::uuids::detail::sha1 objSHA1;
        objSHA1.process_bytes(objData.c_str(), objData.size());
        objSHA1.get_digest(objDigest);

        // For
        for (int objI = 0; objI < 5; ++objI)
        {
            // Set a temporary
            const char * objTemporary(reinterpret_cast<char *> (objDigest));

            // Swappings
            objHash[objI * 4 + 0] = objTemporary[objI * 4 + 3];
            objHash[objI * 4 + 1] = objTemporary[objI * 4 + 2];
            objHash[objI * 4 + 2] = objTemporary[objI * 4 + 1];
            objHash[objI * 4 + 3] = objTemporary[objI * 4 + 0];
        }

        // Set
        std::stringstream onStream;
        onStream << std::hex;

        // For
        for (int objI = 0; objI < 20; ++objI)
        {
            onStream << ((objHash[objI] & 0x000000F0) >> 4)
                << (objHash[objI] & 0x0000000F);
        }

        // Return
        return onStream.str();
    }

    /**
    * Method to try parsing a string to unsigned int;
    */
    bool Utils::TryParseStringToUInt(const std::string& stringIn, unsigned int& uIntOut)
    {
        // Check if numeric
        if (stringIn.find_first_not_of(system::ALL_DIGITS) == std::string::npos
        &&  !stringIn.empty())
        {
            // Ok, Cast it to unsigned int
            uIntOut = boost::lexical_cast<unsigned int> (stringIn);
            return true;
        }
        return false;
    }

    std::vector<char> Utils::ToVectorChar(const std::stringstream& strStream)
    {
        std::string s(strStream.str());
        return std::vector<char>(s.begin(), s.end());
    }

    std::string Utils::GetUntilSeparator(std::string& input)
    {
        size_t separatorPos;

        separatorPos = input.find(system::SEPARATOR);

        if (separatorPos != std::string::npos)
        {
            std::string extractedStr = input.substr(0, separatorPos);

            input.erase(input.begin(), input.begin() + separatorPos + system::SEPARATOR.size());

            return extractedStr;
        }

        return std::string();
    }
}