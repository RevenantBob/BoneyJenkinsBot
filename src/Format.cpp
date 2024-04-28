#include "Format.h"
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cwctype>
#include <cctype>
#ifdef _MSC_VER
#include <Windows.h>
#endif

#ifdef LINUX
#include <codecvt>
#include <locale>
#include <wchar.h>

int _vscprintf (const char * format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}

int _vscwprintf (const wchar_t * format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vswprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
#define vsprintf_s vsnprintf
#define vswprintf_s vswprintf
#endif

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
#ifdef _MSC_VER
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
#endif

#ifdef LINUX
  std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > convert;
  return convert.to_bytes( str );
#endif
}

std::wstring UTF8ToUTF16(const std::string& str)
{
#ifdef _MSC_VER
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
#endif
#ifdef LINUX
  std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > convert;
  return convert.from_bytes( str );
#endif
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
#ifdef _MSC_VER
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
#endif
}
