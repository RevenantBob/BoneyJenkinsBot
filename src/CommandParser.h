#pragma once
#include <optional>
#include <string>
#include <vector>
#include <cstdint>

class CommandParser
{
public:
	static std::vector<std::string> Parse(const std::string& input);
	static std::optional<std::string> Token(const std::string& input, size_t& offset);
	static std::optional<uint32_t> MenuItem(const std::string& input, const std::string& options);

	static bool isspace(char c) { return std::isspace((uint8_t)c); }
};
