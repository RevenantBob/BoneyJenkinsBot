#include "Format.h"
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cwctype>
#include <cctype>
#include <Windows.h>

// printf format for wstring. Helper function.
std::string Format(const char* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);

    size_t nCount = _vscprintf(pszFormat, argptr);
    if (nCount == -1)
    {
        return std::string();
    }

    std::vector<char> buf;

    buf.resize(nCount + 1);

    vsprintf_s(buf.data(), nCount + 1, pszFormat, argptr);

    va_end(argptr);

    std::string strOut = buf.data();

    return strOut;
}

// printf format for string. Helper function.
std::wstring Format(const wchar_t* pszFormat, ...)
{
    va_list argptr;
    va_start(argptr, pszFormat);

    size_t nCount = _vscwprintf(pszFormat, argptr);
    if (nCount == -1)
    {
        return std::wstring();
    }

    std::vector<wchar_t> buf;

    buf.resize(nCount + 1);

    vswprintf_s(buf.data(), nCount + 1, pszFormat, argptr);

    va_end(argptr);

    std::wstring strOut = buf.data();

    return strOut;
}

std::string UTF16ToUTF8(const std::wstring& str)
{
    int len = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0, nullptr, nullptr);
    if (len < 1)
    {
        return "";
    }

    std::vector<char> buffer(len);

    len = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), static_cast<LPSTR>(buffer.data()), (int)buffer.size(), nullptr, nullptr);
    if (len < 1)
    {
        return "";
    }

    return std::string(buffer.begin(), buffer.end());
}

std::wstring UTF8ToUTF16(const std::string& str)
{
    int len = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    if (len < 1)
    {
        return L"";
    }

    std::vector<wchar_t> buffer(len);

    len = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), static_cast<LPWSTR>(buffer.data()), (int)buffer.size());
    if (len < 1)
    {
        return L"";
    }

    return std::wstring(buffer.begin(), buffer.end());

}

std::vector<std::wstring> SpliceString(const std::wstring& source, wchar_t delim)
{
    std::wstring line;
    std::vector<std::wstring> entries;
    std::wstringstream ss(source);

    while (std::getline(ss, line, delim))
    {
        entries.push_back(line);
    }

    return entries;
}

std::wstring BufferToHEX(const std::vector<uint8_t>& buffer)
{
    std::wostringstream ss;

    ss << std::hex << std::uppercase << std::setfill(L'0');
    for (int c : buffer)
    {
        ss << std::setw(2) << c;
    }

    return ss.str();
}

std::string ToLower(const std::string& source)
{
    std::string str = source;

    std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return std::tolower(c); });

    return str;
}

std::wstring ToLower(const std::wstring& source)
{
    std::wstring str = source;

    std::transform(str.cbegin(), str.cend(), str.begin(), [](wchar_t c) { return std::towlower(c); });

    return str;
}
std::string ToUpper(const std::string& source)
{
    std::string str = source;

    std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return std::toupper(c); });

    return str;
}
std::wstring ToUpper(const std::wstring& source)
{
    std::wstring str = source;

    std::transform(str.cbegin(), str.cend(), str.begin(), [](wchar_t c) { return std::towupper(c); });

    return str;
}

std::vector<uint8_t> UTF16ToUTF8Bytes(const std::wstring& str)
{
    int len = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0, nullptr, nullptr);
    if (len < 1)
    {
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> buffer(len);

    len = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), static_cast<LPSTR>((char *)buffer.data()), (int)buffer.size(), nullptr, nullptr);
    if (len < 1)
    {
        return std::vector<uint8_t>();
    }

    return buffer;
}