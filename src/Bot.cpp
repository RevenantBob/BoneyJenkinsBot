#include "Bot.h"
#include "Format.h"
#include <iostream>
#include "CommandParser.h"

Bot::Bot(const Config& config)
    : _Config(config)
    , _Cluster(config.Token(), dpp::i_default_intents | dpp::i_message_content)
{
    _Cluster.on_log(dpp::utility::cout_logger());

    _Cluster.on_slashcommand([this](const dpp::slashcommand_t& event) {
        this->OnSlash(event);
        });

    _Cluster.on_ready([this](const dpp::ready_t& event) {
        this->OnReady(event);
        });

    _Cluster.on_message_create([this](const dpp::message_create_t& event) {
        this->OnMessageCreate(event);
        });

    _Random = std::make_unique<std::mt19937_64>(_RandomDevice());
}

int Bot::Run()
{
    _Cluster.start(dpp::st_wait);

    return 0;
}

void Bot::OnSlash(const dpp::slashcommand_t& event)
{
    auto cmd = event.command.get_command_name();

    if (cmd == "boney"
    || cmd == "boneyjenkins")
    {
        event.reply("Help coming soon, son!");
    }
}

void Bot::OnReady(const dpp::ready_t& event)
{
    try
    {
        std::cout << "OnReady recieved.\r\n";
        auto guilds = _Cluster.current_user_get_guilds_sync();

        auto serversConfig = _Config.Servers();


        for (auto& [id, guild] : guilds)
        {
            std::string name = ::ToLower(guild.name);

            std::cout << std::format("Checking guild: {}.\r\n", name);

            if (serversConfig.contains(name))
            {
                std::cout << std::format("Listening to guild: {}.\r\n", name);

                auto channelMap = serversConfig[name];
                auto channels = _Cluster.channels_get_sync(id);

                for (auto& [channelId, channel] : channels)
                {
                    auto channelName = ::ToLower(channel.name);

                    std::cout << std::format("  Checking channel: {}.\r\n", channelName);

                    if (channelMap.contains(channelName))
                    {
                        _Monitoring.insert(channel.id);
                        std::cout << std::format("Monitoring: {} on {}\r\n", channelName, name);
                    }
                }
            }
        }
    }
    catch(dpp::rest_exception &ex)
    {
        std::string msg = ex.what();

        std::cout << std::format("Exception on_ready: {}\r\n", msg);
    }

    if (dpp::run_once<struct register_bot_commands>()) {
        auto id = _Cluster.me.id;

        _Cluster.global_command_create(dpp::slashcommand("boneyjenkins", "I\'ll tell you the cosmic powers I got from smokin\' the chiba!", id));
        _Cluster.global_command_create(dpp::slashcommand("boney", "I\'ll tell you the cosmic powers I got from smokin\' the chiba!", id));
    }
}

void Bot::OnMessageCreate(const dpp::message_create_t& event)
{
    try
    {
        auto user = event.msg.author.username;
        if (!user.empty())
        {
            auto id = (uint64_t)event.msg.channel_id;

            if (_Monitoring.contains(id))
            {
                std::string content = event.msg.content;
                if (content.size() > 0)
                {
                    if (CommandBeginsWith(content, '.'))
                    {
                        OnCommand(event, user, content);
                    }
                }
            }
        }
    }
    catch(...)
    {

    }
}
bool Bot::CommandBeginsWith(const std::string& message, char token)
{
    size_t i = 0;
    while(i < message.size() && CommandParser::isspace(message[i]))
    {
        ++i;
    }

    if (i < message.size())
    {
        return message[i] == token;
    }
    else
    {
        return false;
    }
}

void Bot::OnCommand(const dpp::message_create_t& event, const std::string &from, const std::string& message)
{
    auto cmd = CommandParser::Parse(message);

    if (cmd.size() > 0)
    {
        if (cmd[0] == ".skill")
        {
            uint32_t skill;
            if (cmd.size() > 1)
            {
                skill = std::strtol(cmd[1].c_str(), nullptr, 10);
            }

            if(cmd.size() > 1 && skill != 0)
            {
                std::string name;
                
                for (size_t i = 2; i < cmd.size(); ++i)
                {
                    if (!name.empty())
                    {
                        name += " ";
                    }

                    if (cmd[i].size() > 0)
                    {
                        cmd[i][0] = std::toupper(cmd[i][0]);
                    }

                    name += cmd[i];
                }

                if (name.empty())
                {
                    name = "a skill";
                }

                // Roll the dice
                std::vector<uint64_t> dice(3);

                dice[0] = ((*_Random)() % 6ULL) + 1;
                dice[1] = ((*_Random)() % 6ULL) + 1;
                dice[2] = ((*_Random)() % 6ULL) + 1;

                uint64_t total = dice[0] + dice[1] + dice[2];

                uint64_t fromId = event.msg.author.id;

                RollResult result = RollResult::Failure;

                auto output = std::format("<@{}> rolled {}.\n", fromId, name);
                if (skill > 0)
                {
                    output += "```ansi\n";

                    output += std::format("Effective Skill: \u001b[1;37m{}\u001b[1;0m\n", skill);

                    if (total == 3
                    || total == 4)
                    {
                        int32_t successBy = (int32_t)(skill - total);

                        output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;32mCRITICAL Success by {}\u001b[1;0m\n", total, successBy);

                        result = RollResult::CriticalSuccess;
                    }
                    else if (total == 18)
                    {
                        int32_t failureBy = (int32_t)(total - skill);

                        output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;31mCRITICAL Failure by {}\u001b[1;0m\n", total, failureBy);

                        result = RollResult::CriticalFailure;
                    }
                    else if (total == 17)
                    {
                        if (skill > 15)
                        {
                            int32_t failureBy = (int32_t)(total - skill);

                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;31mCRITICAL Failure by {}\u001b[1;0m\n", total, failureBy);

                            result = RollResult::CriticalFailure;
                        }
                        else
                        {
                            int32_t failureBy = (int32_t)(total - skill);

                            if (failureBy < 0)
                            {
                                failureBy = 0;
                            }

                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;31mFailure by {}\u001b[1;0m\n", total, failureBy);

                            result = RollResult::Failure;
                        }
                    }
                    else if ((int)total <= skill)
                    {
                        int32_t successBy = (int32_t)(skill - total);

                        if (successBy >= 10
                            && total < 7)
                        {
                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;32mCRITICAL Success by {}\u001b[1;0m\n", total, successBy);

                            result = RollResult::CriticalSuccess;
                        }
                        else
                        {
                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;32mSuccess by {}\u001b[1;0m\n", total, successBy);

                            result = RollResult::Success;
                        }
                    }
                    else
                    {
                        int32_t failureBy = (int32_t)(total - skill);

                        if (failureBy >= 10)
                        {
                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;31mCRITICAL Failure by {}\u001b[1;0m\n", total, failureBy);

                            result = RollResult::CriticalFailure;
                        }
                        else
                        {
                            output += std::format("Total: \u001b[1;37m{}\u001b[1;0m - \u001b[1;31mFailure by {}\u001b[1;0m\n", total, failureBy);

                            result = RollResult::Failure;
                        }
                    }
                }

                output += std::format("Dice Rolled: \u001b[1;37m{}, {}, {}\u001b[1;0m\n", dice[0], dice[1], dice[2]);
                output += "```\n";
                output += std::format("*\"{}\"*", TalkShit(from, result));

                auto msg = dpp::message(output);
                std::vector<dpp::snowflake> roles;
                std::vector<dpp::snowflake> users;

                msg.set_allowed_mentions(false, false, false, true, users, roles);
                
                event.reply(msg);
            }
            else
            {
                std::string output = "This is how you use this sh*t, son.\n";
                output += "```ansi\n";
                output += ".skill <level> [Skill Name]";
                output += "```";


                auto msg = dpp::message(output);
                std::vector<dpp::snowflake> roles;
                std::vector<dpp::snowflake> users;

                msg.set_allowed_mentions(false, false, false, true, users, roles);

                event.reply(msg);
            }
        }
    }
}

std::string Bot::TalkShit(const std::string& from, RollResult result)
{
    switch (result)
    {
        case RollResult::CriticalSuccess:
        {
            auto pick = ((*_Random)() % 8ULL);

            switch (pick)
            {
                case 0: return "I ain\'t tryin' to tell y'all how to do your job, but I\'m pretty sure that fool is cheating!";
                case 1: return "F**ked up the party before it even started!";
                case 2: return "I bet the other players is jealous! Make a new channel for #haters!";
                case 3: return "I used to roll dice like that back in the day! Y\'all shoulda seen it!";
                case 4: return "On what?! I was SURE that fool was finna crap out!";
                case 5: return "I\'m double checking the results. Ain\'t no way they rolled this crit!";
                case 6: return "BOOM SHACKALACKA!!!";
                case 7: return "Even Khaled be like, \"Another one!\"";
                default: return "Boom! Headshot!";
            }
        }
        case RollResult::Success:
        {
            auto pick = ((*_Random)() % 7ULL);

            switch (pick)
            {
            case 0: return "Nice roll, son.";
            case 1: return "Success! Now your party ain\'t gonna kick you!";
            case 2: return "Ya did good, kid. Real good. But you\'ll always be second best as long as I\'m around, see?";
            case 3: return "That\'s right. Show 'em how it\'s done.";
            case 4: return "They\'s heatin\' up.";
            case 5: return "My *****! ... ... ****. Why can't I say *****!? **** this censor shit!";
            case 6: return "Fo' sheezy my neezy!";
            default: return "Congratulations.";
            }
        }
        case RollResult::Failure:
        {
            auto pick = ((*_Random)() % 7ULL);

            switch (pick)
            {
            case 0: return "Womp womp";
            case 1: return "You had ONE job!";
            case 2: return "Close, but no cigar!";
            case 3: return "Try blowin\' on your dice like a Nintendo cartridge!";
            case 4: return "Don\'t question me! I didn\'t make up the results!";
            case 5: return "Ya dun goofed!";
            case 6: return "Hope that roll wasn\'t important!";
            default: return "Oops!";
            }
        }
        case RollResult::CriticalFailure:
        {
            auto pick = ((*_Random)() % 8ULL);

            switch (pick)
            {
            case 0: return "PFFFFT! I\'m dyin'!";
            case 1: return "Y'all might want to replace this player!";
            case 2: return "GREAT! Now summon a demon! Ya know you wanna!";
            case 3: return "What, you gonna roll with luck now?!";
            case 4: return "You can\'t make this sh*t up! Critical Damn Failure!";
            case 5: return "Somebody call a doctor, because tha' sh*t is busted!";
            case 6: return "Just go home. Just go. Yeah, out the door behind you. Yup! Right there. Good.";
            case 7: return "OOOPS! BUTTERFINGERS!";
            default: return "Critical!";
            }
        }
        default: return "They rolled dice. I guess that\'s cool.";
    }
}