#pragma once
#include <optional>
#include <string>
#include <format>

class SystemError
{
public:
    SystemError(const std::string& context, const std::string& message, uint32_t code)
        : m_Context(context)
        , m_Message(message)
        , m_Code(code)
    {

    }

    SystemError(const std::string& context, const std::string& message)
        : m_Context(context)
        , m_Message(message)
    {

    }

    SystemError(const std::string& context, uint32_t code)
        : m_Context(context)
        , m_Code(code)
    {
        m_Message = SystemError::ErrorMessage(code);
    }

    SystemError(const SystemError& error)
        : m_Context(error.m_Context)
        , m_Message(error.m_Message)
        , m_Code(error.m_Code)
    {
    }

    virtual ~SystemError()
    {

    }

    bool HasCode() const { return m_Code.has_value(); }
    std::optional<uint32_t> Code() const { return m_Code; }
    std::string Message() const { return m_Message; }
    std::string Context() const { return m_Context; }

    static std::string ErrorMessage(uint32_t code);

    SystemError& operator =(const SystemError& error)
    {
        m_Context = error.m_Context;
        m_Message = error.m_Message;
        m_Code = error.m_Code;

        return *this;
    }

    std::string Format() const
    {
        if (m_Context.empty())
        {
            if (m_Code.has_value())
            {
                return std::format("{} (0x{:08X})", m_Message, *m_Code);
            }
            else
            {
                return std::format("{}", m_Message);
            }
        }
        else
        {
            if (m_Code.has_value())
            {
                return std::format("{}: {} (0x{:08X})", m_Context, m_Message, *m_Code);
            }
            else
            {
                return std::format("{}: {}", m_Context, m_Message);
            }
        }
    }

private:
    std::string m_Context;
    std::string m_Message;
    std::optional<uint32_t> m_Code;
};