#pragma once

#include <string>

namespace RayClient
{
    class Logger
    {
    public:
        Logger(const std::string& path);

        static void WriteLog(const std::string& message);
    };
}