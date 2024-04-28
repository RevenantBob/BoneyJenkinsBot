#include "Config.h"
#include "json.hpp"
#include <fstream>
#include "Format.h"

using json = nlohmann::json;

Result<Unit, std::string> Config::Load(const std::string &path)
{
    try
    {
        std::ifstream file(path);
    
        json j = json::parse(file);

        if(j.contains("token"))
        {
            _Token = j["token"];
        }

        if(j.contains("servers"))
        {
            for(auto server : j["servers"])
            {
                std::string name;
                std::set<std::string> channels;

                if (server.contains("server"))
                {
                    name = server["server"];

                    auto nameLower = ::ToLower(name);

                    if (server.contains("monitor"))
                    {
                        for (auto channel : server["monitor"])
                        {
                            std::string channelName = channel;

                            auto channelLower = ::ToLower(channelName);
                            channels.insert(channelLower);
                        }

                        _Servers[nameLower] = channels;
                    }
                }

            }
        }
    }
    catch(const std::exception& e)
    {
        return Result<Unit, std::string>::NewError(e.what());
    }
    
    return Result<Unit, std::string>::NewResult({});
}