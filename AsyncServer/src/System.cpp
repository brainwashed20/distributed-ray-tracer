#include "System.h"

#include <string>

namespace RayServer
{
    namespace system
    {
        const std::string LOG_FILE_PATH("..\\..\\RAY_SERVER_LOG.txt"); // Write debug to this file;

        const unsigned int WRITE_BUFFER_SIZE(4096);

        // Operating System specifics
        const std::string NIX_EOL("\r\n"); // The NIX style EOF marker for sockets;
        const std::string CR("\r"); // The carriage return;

        // Utils
        const std::string BACKSLASH("/");
        const std::string HASHTAG(" # # ");
        const std::string ALL_DIGITS("0123456789");
        const std::string PIXEL_TO_PROCESS("pixels_to_process");
        const std::string SEND_SCENE("send_scene");
        const std::string COMPUTED_PIXELS("computed_pixels");
        const std::string RECEIVED_SCENE("received_scene");
        const std::string SEPARATOR("###");
        const std::string BYE_BYE("bye");
    }
}