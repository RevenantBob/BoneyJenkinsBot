#pragma once
#include <dpp/dpp.h>
#include <string>
#include <map>
#include <set>
#include "Config.h"
#include <memory>
#include <random>


enum class RollResult
{
    CriticalSuccess,
    Success,
    Failure,
    CriticalFailure
};


class Bot
{
public:
    Bot(const Config &token);

    int Run();

    void OnSlash(const dpp::slashcommand_t& event);
    void OnReady(const dpp::ready_t& event);
    void OnMessageCreate(const dpp::message_create_t& event);
    void OnCommand(const dpp::message_create_t& event, const std::string &from, const std::string &message);
    std::string TalkShit(const std::string &from, RollResult result);
private:
    static bool CommandBeginsWith(const std::string &message, char token);
private:
    Config _Config;

    std::set<uint64_t> _Monitoring;

    dpp::cluster _Cluster;
    std::random_device _RandomDevice;
    std::unique_ptr<std::mt19937_64> _Random;
};

