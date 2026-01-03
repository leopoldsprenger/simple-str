#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace SimpleStr {

inline bool startswith(std::string_view s, std::string_view prefix) {
  return s.substr(0, prefix.size()) == prefix;
}

bool endswith(std::string_view s, std::string_view suffix) {
  if (suffix.size() > s.size())
    return false;
  return s.substr(s.size() - suffix.size(), suffix.length()) == suffix;
}

std::vector<std::string_view> split(std::string_view s, char delim) {
  std::vector<std::string_view> strings;
  size_t start = 0;

  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] != delim)
      continue;

    strings.push_back(s.substr(start, i - start));
    start = i + 1;
  }

  if (start <= s.size())
    strings.push_back(s.substr(start));

  return strings;
}

std::string join(const std::vector<std::string_view> &parts,
                 std::string_view sep) {
  std::string joined_str;

  size_t total_size = 0;
  for (auto &p : parts)
    total_size += p.size();
  total_size += sep.size() * (parts.size() ? parts.size() - 1 : 0);
  joined_str.reserve(total_size);

  for (size_t i = 0; i < parts.size(); i++) {
    joined_str += parts[i];
    if (i < parts.size() - 1) {
      joined_str += sep;
    }
  }

  return joined_str;
}

std::string slice(std::string_view s, int start, int end, int step = 1) {
  assert(step != 0);

  const int n = static_cast<int>(s.size());

  // normalize negative indices
  if (start < 0)
    start += n;
  if (end < 0)
    end += n;

  // clamp to bounds [0, n]
  start = std::clamp(start, 0, n);
  end = std::clamp(end, 0, n);

  std::string result;

  // reserve a reasonable amount to avoid repeated allocations
  if (step > 0) {
    if (start >= end)
      return result;
    result.reserve((end - start + step - 1) / step); // approximate
    for (int i = start; i < end; i += step)
      result += s[i];
  } else { // step < 0
    if (start <= end)
      return result;
    result.reserve((start - end - step - 1) / (-step)); // approximate
    for (int i = start; i > end; i += step)
      result += s[i];
  }

  return result;
}

std::string replace(std::string_view s, std::string_view from,
                    std::string_view to) {
  if (from.empty())
    return std::string(s);

  std::string result;
  size_t pos = 0;

  // estimate reserve size: original string + max extra space if all replaced
  size_t occurrences = 0;
  size_t tmp_pos = 0;
  while (true) {
    size_t match = s.find(from, tmp_pos);
    if (match == s.npos)
      break;
    ++occurrences;
    tmp_pos = match + from.size();
  }
  result.reserve(s.size() + occurrences * to.size());

  while (true) {
    size_t match = s.find(from, pos);
    if (match == s.npos)
      break;

    result += s.substr(pos, match - pos);
    result += to;
    pos = match + from.size();
  }

  result += s.substr(pos);

  return result;
}

} // namespace SimpleStr
