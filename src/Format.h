#pragma once
#include <string>
#include <vector>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdint>

std::string Format(const char* pszFormat, ...);
std::wstring Format(const wchar_t* pszFormat, ...);
std::string UTF16ToUTF8(const std::wstring &str);
std::wstring UTF8ToUTF16(const std::string& str);
std::vector<std::wstring> SpliceString(const std::wstring& source, wchar_t delim);
std::wstring BufferToHEX(const std::vector<uint8_t>& buffer);

std::string ToLower(const std::string& source);
std::wstring ToLower(const std::wstring& source);
std::string ToUpper(const std::string& source);
std::wstring ToUpper(const std::wstring& source);



std::vector<uint8_t> UTF16ToUTF8Bytes(const std::wstring& str);

template<>
struct std::formatter<std::wstring, char>
{
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    FmtContext::iterator format(std::wstring s, FmtContext& ctx) const
    {
        std::ostringstream out;

        auto sout = UTF16ToUTF8(s);
        out << sout;

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

template<>
struct std::formatter<std::string, wchar_t>
{
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    FmtContext::iterator format(std::string s, FmtContext& ctx) const
    {
        std::wostringstream out;
        auto sout = UTF8ToUTF16(s);
        out << sout;
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
