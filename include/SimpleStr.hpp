#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace SimpleStr {

bool startswith(std::string_view s, std::string_view prefix);
bool endswith(std::string_view s, std::string_view suffix);

std::vector<std::string_view> split(std::string_view s, std::string_view delim);

std::string join(const std::vector<std::string_view> &parts,
                 std::string_view sep);

std::string slice(std::string_view s, int start, int end);

std::string replace(std::string_view s, std::string_view from,
                    std::string_view to);
} // namespace SimpleStr
