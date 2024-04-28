#include "SystemError.h"
#ifdef _MSC_VER
#include <Windows.h>
#endif
#ifdef LINUX
#include <string.h>
#endif
std::string SystemError::ErrorMessage(uint32_t code)
{
#ifdef _MSC_VER
    LPSTR pStr = NULL;
    DWORD cchMsg = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&pStr,
        0,
        NULL);
    if (cchMsg > 0)
    {
        auto deleter = [](void* p) { ::LocalFree(p); };
        std::unique_ptr<char, decltype(deleter)> ptrBuffer(pStr, deleter);
        return std::string(ptrBuffer.get(), cchMsg);
    }
    else
    {
        return std::string("Unknown error.");
    }
#endif

#ifdef LINUX
    std::string result;
    std::vector<char> buffer(1024);

    auto str = strerror_r(code, buffer.data(), buffer.size());
    if (str != nullptr)
    {
        result = str;
    }

    return result;
#endif
}
