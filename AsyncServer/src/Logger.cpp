#include "Logger.h"

#include "Requirements.h"

#include <fstream>

namespace RayServer
{
    void Logger::WriteLog(const std::string &objLog)
    {
        std::stringstream onStream;

        onStream << "[RayServer] LOG ## " << "At: "
            << Utils::GetUTCAsString() << system::HASHTAG
            << objLog;

        std::cout << onStream.str() << std::endl;

        // Set the stream path
        std::string debugPath(system::LOG_FILE_PATH);

        std::ofstream onDebug(debugPath.c_str(), std::ios::out | std::ios::ate | std::ios::app);

        onDebug << onStream.str() << std::endl;
        onDebug.close();
    }
}