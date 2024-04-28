#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>
#include "Result.h"

class Config
{
public:
    Config()
    {
    }
    Config(const Config& other)
        : _Token(other._Token)
        , _Servers(other._Servers)
    {

    }
    const std::map<std::string, std::set<std::string>> &Servers() const { return _Servers; }
    const std::string Token() const { return _Token; }

    Result<Unit, std::string> Load(const std::string &path);

    Config& operator =(const Config& other)
    {
        _Token = other._Token;
        _Servers = other._Servers;
    }
private:
    std::string _Token;
    std::map<std::string, std::set<std::string>> _Servers;
};