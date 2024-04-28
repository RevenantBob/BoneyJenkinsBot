
#include <dpp/dpp.h>
#include "Config.h"
#include "Format.h"
#include "Bot.h"

#ifdef _MSC_VER
#pragma comment(lib, "dpp.lib")
#endif

int main(int argc, char **argv)
{
    Config config;

    auto configResult = config.Load("config.json");
    if(!configResult)
    {
        std::cout << std::format("Failed to load config: {}", configResult.Error());
        return 1;
    }
    
    Bot bot(config);

    return bot.Run();
}