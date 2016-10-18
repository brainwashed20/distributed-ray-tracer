#pragma once

#include <string>

namespace RayServer
{
    class Logger
    {
    public:
        Logger(const std::string& path);

        static void WriteLog(const std::string& message);
    };
}