#include "CommandParser.h"

enum class ParseState
{
    Trim,
    Word,
    DoubleQuoted,
    SingleQuoted,
};

std::vector<std::string> CommandParser::Parse(const std::string& input)
{
    ParseState state = ParseState::Trim;

    std::string current;
    std::vector<std::string> tokens;

    for (size_t i = 0; i < input.size(); ++i)
    {
        uint8_t c = input[i];

        switch (state)
        {
        case ParseState::Trim:
        {
            if (!isspace(c))
            {
                i--;
                switch (c)
                {
                case '\'':
                {
                    state = ParseState::SingleQuoted;
                    break;
                }
                case '\"':
                {
                    state = ParseState::DoubleQuoted;
                    break;
                }
                default:
                {
                    state = ParseState::Word;
                }
                }

            }
            break;
        }
        case ParseState::Word:
        {
            if (isspace(c))
            {
                tokens.push_back(current);
                current.clear();

                i--;
                state = ParseState::Trim;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }
        case ParseState::SingleQuoted:
        {
            if (c == '\'')
            {
                tokens.push_back(current);
                current.clear();
                state = ParseState::Trim;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }
        case ParseState::DoubleQuoted:
        {
            if (c == '\"')
            {
                tokens.push_back(current);
                current.clear();

                state = ParseState::Trim;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }
        }
    }

    if (current.size() > 0)
    {
        tokens.push_back(current);
    }
    return tokens;
}

std::optional<std::string> CommandParser::Token(const std::string& input, size_t& offset)
{
    ParseState state = ParseState::Trim;

    if (offset >= input.size())
    {
        return {};
    }

    std::string current;

    for (size_t i = offset; i < input.size(); ++i)
    {
        uint8_t c = input[i];

        switch (state)
        {
        case ParseState::Trim:
        {
            if (!std::isspace(c))
            {
                i--;
                switch (c)
                {
                case '\'':
                {
                    state = ParseState::SingleQuoted;
                    break;
                }
                case '\"':
                {
                    state = ParseState::DoubleQuoted;
                    break;
                }
                default:
                {
                    state = ParseState::Word;
                }
                }

            }
            break;
        }

        case ParseState::Word:
        {
            if (isspace(c))
            {
                while (isspace(c))
                {
                    i++;
                    c = input[i];
                }

                offset = i;
                return current;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }

        case ParseState::SingleQuoted:
        {
            if (c == '\'')
            {
                do
                {
                    i++;
                    c = input[i];
                } while (std::isspace(c));

                offset = i;
                return current;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }
        case ParseState::DoubleQuoted:
        {
            if (c == '\"')
            {
                do
                {
                    i++;
                    c = input[i];
                } while (std::isspace(c));

                offset = i;
                return current;
            }
            else
            {
                current.push_back(c);
            }
            break;
        }
        }
    }

    if (state == ParseState::Trim)
    {
        return {};
    }

    offset = input.size();
    return current;
}

std::optional<uint32_t> CommandParser::MenuItem(const std::string& input, const std::string& options)
{
    if (input.length() != 1)
    {
        return {};
    }

    uint8_t c = input[0];

    for (size_t i = 0; i < options.length(); ++i)
    {
        if (options[i] == c)
        {
            return (uint32_t)i;
        }
    }

    return {};
}
